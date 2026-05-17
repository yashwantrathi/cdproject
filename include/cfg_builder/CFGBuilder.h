#ifndef CFGBUILDER_H
#define CFGBUILDER_H

#include "llvm/IR/Function.h"

class CFGBuilder {
public:
    void buildCFG(llvm::Function &F, bool exportGraph=false);
};

#endif