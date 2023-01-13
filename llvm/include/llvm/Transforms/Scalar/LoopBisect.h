#ifndef LLVM_TRANSFORMS_SCALAR_LOOPBISECT_H
#define LLVM_TRANSFORMS_SCALAR_LOOPBISECT_H

#include "llvm/Pass.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/ValueMapper.h"

namespace llvm {
class Loop;
class LPMUpdater;

// This class divides innermost loop into two loops.
class LoopBisect {
public:
    LoopBisect(LoopInfo &LI, ScalarEvolution &SE, DominatorTree &DT)
        : LI(LI), SE(SE), DT(DT) {}

    bool run(Loop &L) const;

private:
    LoopInfo &LI;
    ScalarEvolution &SE;
    DominatorTree &DT;

    // Determines if a provided loop is a candidate
    // for bisecting
    bool isCandidate(const Loop &L) const;

    // Splits loop in half by traversing halfway through
    // a loop, then cloning the loop while adjusting
    // loop bounds of both original and cloned loops.
    bool splitLoopInHalf(Loop &L) const;

    // Clone loop and insert it before InsertBefore basic block.
    // Pred represents the predecessor of loop L.
    Loop *cloneLoop(Loop &L, BasicBlock &InsertBefore, BasicBlock &Pred) const;


    // Compute the point of splitting of loop L. Instruction that is returned
    // calculates the split point.
    Instruction *computeSplitPoint(const Loop &L,
                                   Instruction *InsertBefore) const;

    // Get latch comparison instruction.
    ICmpInst *getLatchCmpInst(const Loop &L) const;

    // Update DominatorTree after cloning loop.
    void updateDominatorTree(const Loop &OrigLoop, const Loop &ClonedLoop,
                             BasicBlock &InsertBefore, BasicBlock &Pred,
                             ValueToValueMapTy &VMap) const;

    void dumpFunction(const StringRef Msg, const Function &F) const;
};

class LoopBisectPass : public PassInfoMixin<LoopBisectPass> {
public:
    PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                          LoopStandardAnalysisResults &AR, LPMUpdater &U);
};
} // end namespace llvm

#endif // LLVM_TRANSFORMS_SCALAR_LOOPBISECT_H
