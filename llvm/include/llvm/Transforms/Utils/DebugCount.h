#ifndef LLVM_TRANSFORMS_UTILS_DEBUGCOUNT_H
#define LLVM_TRANSFORMS_UTILS_DEBUGCOUNT_H

#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include <unordered_map>

namespace llvm {
class DebugCountPass : public PassInfoMixin<DebugCountPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};
} // namespace llvm

#endif // LLVM_TRANSFORMS_UTILS_DEBUGCOUNT_H
