#include "llvm/Transforms/Utils/DebugCount.h"

using namespace llvm;

PreservedAnalyses DebugCountPass::run(Function &F, FunctionAnalysisManager &AM) {
    std::unordered_map<const char*, int> DebugMap;

    for(inst_iterator I = inst_begin(F), End = inst_end(F); I != End; ++I) {
        if(I->isDebugOrPseudoInst() && isa<CallInst>(*I)) {
            CallInst& CI = cast<CallInst>(*I);
            DebugMap[CI.getCalledFunction()->getName().data()]++;
        }
    }

    for(auto I : DebugMap) {
        llvm::outs() << I.first << ':' << I.second << '\n';
    }

    return PreservedAnalyses::all();
}
