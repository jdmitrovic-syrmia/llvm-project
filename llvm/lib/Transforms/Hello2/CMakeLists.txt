add_llvm_library( LLVMHello2 MODULE
  Hello.cpp
  PLUGIN_TOOL
  opt
  )
