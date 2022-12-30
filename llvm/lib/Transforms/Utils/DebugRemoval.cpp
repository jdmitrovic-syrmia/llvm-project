#include "llvm/Transforms/Utils/DebugRemoval.h"

using namespace llvm;

PreservedAnalyses DebugRemovalPass::run(Function &F, FunctionAnalysisManager &AM) {
    SmallVector<Instruction*, 128> DebugI;

    for(inst_iterator I = inst_begin(F), End = inst_end(F); I != End; ++I) {
        if(I->isDebugOrPseudoInst()) {
            DebugI.push_back(&*I);
        }
    }

    for(Instruction* I : DebugI) {
        I->dropAllReferences();
        I->eraseFromParent();
    }

    return PreservedAnalyses::none();
}
