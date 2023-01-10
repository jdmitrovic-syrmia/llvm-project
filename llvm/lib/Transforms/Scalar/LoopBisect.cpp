#include "llvm/Transforms/Scalar/LoopBisect.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"

using namespace llvm;

#define DEBUG_TYPE "loop-bisect"

bool LoopBisect::run(Loop &L) const {
    LLVM_DEBUG(dbgs() << "Entering " << __func__ << '\n');

    if(!isCandidate(L)) {
        LLVM_DEBUG(dbgs() << "Loop: " << L.getName()
                   << "is not a candidate for bisecting");

        return false;
    }

    LLVM_DEBUG(dbgs() << "Loop: " << L.getName()
                << "is a candidate for bisecting");

    return true;
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

PreservedAnalyses LoopBisectPass::run(Loop &L, LoopAnalysisManager &AM,
                                      LoopStandardAnalysisResults &AR,
                                      LPMUpdater &U) {
    LLVM_DEBUG(dbgs() << "Entering LoopBisectPass::run\n");
    LLVM_DEBUG(dbgs() << "Loop: "; L.dump(); dbgs() << "\n");

    bool Changed = LoopBisect(AR.LI).run(L);

    if(!Changed) {
        return PreservedAnalyses::all();
    }

    return llvm::getLoopPassPreservedAnalyses();
}
