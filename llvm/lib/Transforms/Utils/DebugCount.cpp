#include "llvm/Transforms/Utils/DebugCount.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringMap.h"

using namespace llvm;

PreservedAnalyses DebugCountPass::run(Function &F, FunctionAnalysisManager &AM) {
    StringMap<int> DebugMap;

    for(inst_iterator I = inst_begin(F), End = inst_end(F); I != End; ++I) {
        if(I->isDebugOrPseudoInst() && isa<CallInst>(*I)) {
            CallInst& CI = cast<CallInst>(*I);
            DebugMap[CI.getCalledFunction()->getName()]++;
        }
    }

    for(auto &I : DebugMap) {
        llvm::outs() << I.getKey() << ':' << I.getValue() << '\n';
    }

    return PreservedAnalyses::all();
}
