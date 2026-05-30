#include "cfg_builder/CFGBuilder.h"

#include <iostream>

#include "llvm/IR/CFG.h"
#include <fstream>
using namespace llvm;

void CFGBuilder::buildCFG(Function &F, bool exportGraph) {
blockCount = 0;
edgeCount = 0;
branchCount = 0;
complexityScore = 0;
complexityLevel = "";
std::string complexityLevel;
    std::cout << "\n====== CFG ======\n" << std::endl;
    std::ofstream dotFile;

if(exportGraph) {

    std::string filename =
        "../results/cfg_" +
        F.getName().str() +
        ".dot";

    dotFile.open(filename);

    dotFile << "digraph CFG {\n";
}
    
int blockCounter = 0;
   for (BasicBlock &BB : F) {
blockCount++;
    std::cout << "BasicBlock: ";
std::string currentBlock;

if (BB.hasName()) {
    currentBlock = BB.getName().str();
}
else {
    currentBlock =
        "Block" + std::to_string(blockCounter);
}

std::cout << currentBlock;

    std::cout << std::endl;
blockCounter++;
    for (BasicBlock *Succ : successors(&BB)) {
branchCount++;
        edgeCount++;
        std::cout << "  -> ";

        if (Succ->hasName()) {
    std::cout << Succ->getName().str();
}
else {
    std::cout << "Block?";
}

        std::cout << std::endl;

        if(exportGraph) {

            std::string from =
                BB.hasName() ?
                BB.getName().str() :
                "unnamed";

            std::string to =
                Succ->hasName() ?
                Succ->getName().str() :
                "unnamed";
            std::string nodeLabel = from + "\\n";

for (Instruction &I : BB) {

    nodeLabel +=
        std::string(I.getOpcodeName());

    nodeLabel += "\\n";
}

dotFile << "\""
        << from
        << "\" [label=\""
        << nodeLabel
        << "\"];\n";
            dotFile << "\"" << from
                    << "\" -> \""
                    << to
                    << "\";\n";
        }
    }
}
complexityScore =
    blockCount +
    edgeCount +
    branchCount;

if(complexityScore <= 10) {
    complexityLevel = "LOW";
}

else if(complexityScore <= 20) {
    complexityLevel = "MEDIUM";
}

else {
    complexityLevel = "HIGH";
}
std::cout << "\nCFG Metrics:\n";

std::cout << "Basic Blocks: "
          << blockCount
          << std::endl;

std::cout << "Edges: "
          << edgeCount
          << std::endl;
          std::cout << "Branches: "
          << branchCount
          << std::endl;

std::cout << "Complexity Score: "
          << complexityScore
          << std::endl;

std::cout << "Complexity Level: "
          << complexityLevel
          << std::endl;
    if(exportGraph) {

    dotFile << "}\n";
    dotFile.close();

    std::string command =
        "dot -Tpng ../results/cfg_" +
        F.getName().str() +
        ".dot -o ../results/cfg_" +
        F.getName().str() +
        ".png";

    system(command.c_str());
}
}
int CFGBuilder::getBlockCount() {
    return blockCount;
}

int CFGBuilder::getEdgeCount() {
    return edgeCount;
}
int CFGBuilder::getComplexityScore() {
    return complexityScore;
}

std::string CFGBuilder::getComplexityLevel() {
    return complexityLevel;
}