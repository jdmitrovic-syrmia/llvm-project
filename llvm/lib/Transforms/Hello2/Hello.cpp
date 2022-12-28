#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
    struct Hello2 : public FunctionPass {
        static char ID;
        Hello2() : FunctionPass(ID) {}

        bool runOnFunction(Function &F) override {
            errs() << "Hello2: ";
            errs().write_escaped(F.getName()) << '\n';
            return false;
        }
    };
}

char Hello2::ID = 0;
static RegisterPass<Hello2> X("hello2", "Hello World Pass 2.0",
                              false,
                              false);
