#ifndef LLVM_TRANSFORMS_SCALAR_LOOPBISECT_H
#define LLVM_TRANSFORMS_SCALAR_LOOPBISECT_H

#include "llvm/Analysis/LoopAnalysisManager.h"


namespace llvm {
class Loop;
class LPMUpdater;

class LoopBisect {
public:
    LoopBisect(LoopInfo &LI) : LI(LI) {}

    bool run(Loop &L) const;

private:
    LoopInfo &LI;
};

class LoopBisectPass : public PassInfoMixin<LoopBisectPass> {
public:
    PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                          LoopStandardAnalysisResults &AR, LPMUpdater &U);
};
} // end namespace llvm

#endif // LLVM_TRANSFORMS_SCALAR_LOOPBISECT_H
