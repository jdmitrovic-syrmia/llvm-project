#ifndef LLVM_TRANSFORMS_UTILS_DEBUGREMOVAL_H
#define LLVM_TRANSFORMS_UTILS_DEBUGREMOVAL_H

#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ADT/SmallVector.h"

namespace llvm {
class DebugRemovalPass : public PassInfoMixin<DebugRemovalPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};
} // namespace llvm

#endif // LLVM_TRANSFORMS_UTILS_DEBUGREMOVAL_H
