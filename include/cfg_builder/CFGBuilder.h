#ifndef CFGBUILDER_H
#define CFGBUILDER_H

#include "llvm/IR/Function.h"

class CFGBuilder {
    int blockCount = 0;
int edgeCount = 0;
int branchCount = 0;

int complexityScore = 0;

std::string complexityLevel;
int getBlockCount();

int getEdgeCount();
int getComplexityScore();

std::string getComplexityLevel();
public:
    void buildCFG(llvm::Function &F, bool exportGraph=false);
};

#endif