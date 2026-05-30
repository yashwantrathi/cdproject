#ifndef DFGBUILDER_H
#define DFGBUILDER_H

#include "llvm/IR/Function.h"

class DFGBuilder {

public:

    void buildDFG(llvm::Function &F, bool verbose = false);

};

#endif
