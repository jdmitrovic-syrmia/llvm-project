#include "llvm/Transforms/Scalar/LoopBisect.h"

using namespace llvm;

#define DEBUG_TYPE "loop-bisect"

static const char *VerboseDebug = DEBUG_TYPE "-verbose";

bool LoopBisect::run(Loop &L) const {
    LLVM_DEBUG(dbgs() << "Entering " << __func__ << '\n');

    if(!isCandidate(L)) {
        LLVM_DEBUG(dbgs() << "Loop: " << L.getName()
                   << "is not a candidate for bisecting");

        return false;
    }

    LLVM_DEBUG(dbgs() << "Loop: " << L.getName()
                << "is a candidate for bisecting");

    return splitLoopInHalf(L);
}

bool LoopBisect::isCandidate(const Loop &L) const {
    // Loop must be in simplified form in order to be bisected.
    if(!L.isLoopSimplifyForm())
        return false;

    // Loop must be cloneable in order to be bisected.
    if(!L.isSafeToClone())
        return false;

    // If there are multiple exiting blocks, loop is not a candidate for bisection.
    if(!L.getExitingBlock())
        return false;

    // If there are multiple exit blocks, loop is not a candidate for bisection.
    if(!L.getExitBlock())
        return false;

    // Only innermost loops are candidates for bisecting.
    if(!L.getSubLoops().empty())
        return false;

    return true;
}

bool LoopBisect::splitLoopInHalf(Loop &L) const {
    assert(L.isLoopSimplifyForm() && "Loop needs to be in simplified form");
    assert(L.isSafeToClone() && "Loop must be cloneable");
    assert(L.getSubLoops().empty() && "Only innermost loop can be split");

    LLVM_DEBUG(dbgs() << "Splitting loop " << L.getName() << "\n");

    BasicBlock *Preheader = L.getLoopPreheader();
    Instruction *Split = computeSplitPoint(L, Preheader->getTerminator());
    BasicBlock *Pred = Preheader;
    BasicBlock *InsertBefore = SplitBlock(Preheader, Preheader->getTerminator(),
                                          &DT, &LI);

    const Function &F = *L.getHeader()->getParent();
    DEBUG_WITH_TYPE(VerboseDebug, dumpFunction("After splitting preheader:\n", F));

    // Clone the original loop and insert the cloned loop before the original.
    Loop *ClonedLoop = cloneLoop(L, *InsertBefore, *Pred);

    // Modify the upper bound of cloned loop.
    ICmpInst *LatchCmpInst = getLatchCmpInst(*ClonedLoop);
    assert(LatchCmpInst && "Unable to find latch instruction in cloned loop");
    LatchCmpInst->setOperand(1, Split);
    
    // Modify the lower bound of original loop.
    PHINode *IndVar = L.getInductionVariable(SE);
    assert(IndVar && "Unable to find induction variable Phi node");
    IndVar->setIncomingValueForBlock(L.getLoopPreheader(), Split);

    return true;
}

void LoopBisect::dumpFunction(const StringRef Msg, const Function &F) const {
    dbgs() << Msg;
    F.dump();
}

Loop *myCloneLoopWithPreheader(BasicBlock *Before, BasicBlock *LoopDomBB,
                               Loop *OrigLoop, ValueToValueMapTy &VMap,
                               const Twine &NameSuffix, LoopInfo *LI,
                               SmallVectorImpl<BasicBlock *> &Blocks,
                               const DominatorTree &DT,
                               SmallVector<DominatorTree::UpdateType, 8> &TreeUpdates);

Loop *LoopBisect::cloneLoop(Loop &L, BasicBlock &InsertBefore, BasicBlock &Pred) const {
    Function &F = *L.getHeader()->getParent();
    SmallVector<BasicBlock *, 4> ClonedLoopBlocks;
    ValueToValueMapTy VMap;
    SmallVector<DominatorTree::UpdateType, 8> TreeUpdates;

    Loop * NewLoop = myCloneLoopWithPreheader(&InsertBefore, &Pred, &L, VMap, "",
                                              &LI, ClonedLoopBlocks, DT, TreeUpdates);

    assert(NewLoop && "Ran out of memory");
    DEBUG_WITH_TYPE(VerboseDebug, {
                        dbgs() << "Created new loop: " << NewLoop->getName() << "\n";
                        dumpFunction("After cloning loop:\n", F);
                        dbgs() << "DomTree updates: \n";
                        for(auto u : TreeUpdates) {
                            u.print(dbgs().indent(2));
                            dbgs() << "\n";
                        }
                    });

    // Update instructions referencing the original loop basic blocks to
    // reference the corresponding block in the cloned loop.
    VMap[L.getExitBlock()] = &InsertBefore;
    remapInstructionsInBlocks(ClonedLoopBlocks, VMap);
    DEBUG_WITH_TYPE(VerboseDebug,
                    dumpFunction("After instruction remapping:\n", F));

    Pred.getTerminator()->replaceUsesOfWith(&InsertBefore,
                                            NewLoop->getLoopPreheader());

    // Apply updates to dominator tree.
    DomTreeUpdater DTU(DT, DomTreeUpdater::UpdateStrategy::Lazy);
    DTU.applyUpdates(TreeUpdates);
    DTU.flush(); 
    
    // Verification that the dominator tree and the loops are correct.
    #ifndef NDEBUG
        assert(DT.verify(DominatorTree::VerificationLevel::Full) &&
               "Dominator tree is invalid");
        L.verifyLoop();
        NewLoop->verifyLoop();
        if(Loop *Parent = L.getParentLoop())
            Parent->verifyLoop();
        LI.verify(DT);
    #endif

    return NewLoop;
}

Instruction *LoopBisect::computeSplitPoint(const Loop &L,
                                           Instruction *InsertBefore) const {
    Optional<Loop::LoopBounds> Bounds = L.getBounds(SE);
    assert(Bounds.has_value() && "Unable to retrieve loop bounds");
    
    Value &IVInitial = Bounds->getInitialIVValue();
    Value &IVFinal = Bounds->getFinalIVValue();
    auto *Sub = BinaryOperator::Create(Instruction::Sub,
                                       &IVFinal, &IVInitial, 
                                       "", InsertBefore);

    return BinaryOperator::Create(Instruction::UDiv, 
                                  Sub, ConstantInt::get(IVFinal.getType(), 2), 
                                  "", InsertBefore);
}

ICmpInst *llvm::LoopBisect::getLatchCmpInst(const Loop &L) const {
    if(BasicBlock *Latch = L.getLoopLatch())
    // Checking whether terminator is a branch instruction
        if(BranchInst *BI = dyn_cast_or_null<BranchInst>(Latch->getTerminator()))
            // Checking whether the branch is a comparison instruction
            if(BI->isConditional())
                return dyn_cast<ICmpInst>(BI->getCondition());

    return nullptr;
}

PreservedAnalyses LoopBisectPass::run(Loop &L, LoopAnalysisManager &AM,
                                      LoopStandardAnalysisResults &AR,
                                      LPMUpdater &U) {
    LLVM_DEBUG(dbgs() << "Entering LoopBisectPass::run\n");
    LLVM_DEBUG(dbgs() << "Loop: "; L.dump(); dbgs() << "\n");

    bool Changed = LoopBisect(AR.LI, AR.SE, AR.DT).run(L);

    if(!Changed) {
        return PreservedAnalyses::all();
    }

    return llvm::getLoopPassPreservedAnalyses();
}

Loop *myCloneLoopWithPreheader(BasicBlock *Before, BasicBlock *LoopDomBB,
                               Loop *OrigLoop, ValueToValueMapTy &VMap,
                               const Twine &NameSuffix, LoopInfo *LI,
                               SmallVectorImpl<BasicBlock *> &Blocks,
                               const DominatorTree &DT,
                               SmallVector<DominatorTree::UpdateType, 8> &TreeUpdates) {
    Function *F = OrigLoop->getHeader()->getParent();
    Loop *ParentLoop = OrigLoop->getParentLoop();
    DenseMap<Loop *, Loop *> LMap;

    Loop *NewLoop = LI->AllocateLoop();
    LMap[OrigLoop] = NewLoop;

    if(ParentLoop)
        ParentLoop->addChildLoop(NewLoop);
    else
        LI->addTopLevelLoop(NewLoop);

    BasicBlock *OrigPH = OrigLoop->getLoopPreheader();
    assert(OrigPH && "No preheader in original loop");
    BasicBlock *NewPH = CloneBasicBlock(OrigPH, VMap, NameSuffix, F);

    VMap[OrigPH] = NewPH;
    Blocks.push_back(NewPH);

    // Update LoopInfo.
    if(ParentLoop)
        ParentLoop->addBasicBlockToLoop(NewPH, *LI);

    for(Loop *CurrLoop : OrigLoop->getLoopsInPreorder()) {
        Loop *&NewLoop = LMap[CurrLoop];

        if(!NewLoop) {
            NewLoop = LI->AllocateLoop();

            Loop *OrigParentLoop = CurrLoop->getParentLoop();
            assert(OrigParentLoop && "Could not find the original parent loop");
            Loop *NewParentLoop = LMap[OrigParentLoop];
            assert(NewParentLoop && "Could not find the new parent loop");

            NewParentLoop->addChildLoop(NewLoop);
        }
    }

    for(BasicBlock *BB : OrigLoop->getBlocks()) {
        Loop *CurrLoop = LI->getLoopFor(BB);
        Loop *&NewLoop = LMap[CurrLoop];
        assert(NewLoop && "New loop is expected to be allocated");

        BasicBlock *NewBB = CloneBasicBlock(BB, VMap, NameSuffix, F);
        VMap[BB] = NewBB;

        NewLoop->addBasicBlockToLoop(NewBB, *LI);
        if(BB == CurrLoop->getHeader())
            NewLoop->moveToHeader(NewBB);
            
        // Update DomTree and
        // get the immediate dominator for the original block.
        BasicBlock *IDomBB = DT.getNode(BB)->getIDom()->getBlock();
        assert(VMap[IDomBB] && 
               "Expecting immediate dominator in the value map!");
        // Insert an edge between the mapped immediate dominator
        // and the new block.
        TreeUpdates.emplace_back(DominatorTree::UpdateType(DominatorTree::Insert,
                                 cast<BasicBlock>(VMap[IDomBB]), NewBB));


        Blocks.push_back(NewBB);
    }

    F->splice(Before->getIterator(), F, NewPH->getIterator());
    F->splice(Before->getIterator(), F, NewLoop->getHeader()->getIterator(),
              F->end());

    // Update the dominators for new and old preheader.
    TreeUpdates.emplace_back(DominatorTree::Delete, LoopDomBB, OrigPH);
    TreeUpdates.emplace_back(DominatorTree::Insert, LoopDomBB, NewPH);

    // Update the exiting block for the new loop.
    TreeUpdates.emplace_back(DominatorTree::Insert, 
                             cast<BasicBlock>(VMap[OrigLoop->getExitingBlock()]), 
                                              OrigPH);

    return NewLoop;
}
