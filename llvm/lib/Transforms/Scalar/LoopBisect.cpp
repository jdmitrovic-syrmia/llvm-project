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

    return false;
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
