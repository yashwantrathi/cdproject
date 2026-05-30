#include "dfg_builder/DFGBuilder.h"

#include <iostream>
#include <fstream>
#include <map>
#include <string>

using namespace llvm;

void DFGBuilder::buildDFG(Function &F, bool verbose) {

    if (!verbose) return;

    std::cout << "\n====== DFG ======\n";

    std::ofstream dotFile(
        "../results/dfg_" +
        F.getName().str() +
        ".dot"
    );

    dotFile << "digraph DFG {\n";

    int nodeId = 0;
    std::map<Instruction*, std::string> nodeMap;

    // First pass: Assign unique IDs and declare nodes
    for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
            std::string id = std::string(I.getOpcodeName()) + "_" + std::to_string(nodeId++);
            nodeMap[&I] = id;
            dotFile << "    \"" << id << "\" [label=\"" << I.getOpcodeName() << "\"];\n";
        }
    }

    // Second pass: Draw edges based on dependencies
    for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
            std::string currentId = nodeMap[&I];

            for (Use &U : I.operands()) {
                if (Instruction *Dep = dyn_cast<Instruction>(U)) {
                    if (nodeMap.find(Dep) != nodeMap.end()) {
                        std::string depId = nodeMap[Dep];

                        std::cout << Dep->getOpcodeName() << " -> " << I.getOpcodeName() << std::endl;

                        dotFile << "    \"" << depId << "\" -> \"" << currentId << "\";\n";
                    }
                }
            }
        }
    }

    dotFile << "}\n";

    dotFile.close();

    std::string command =
        "dot -Tpng ../results/dfg_" +
        F.getName().str() +
        ".dot -o ../results/dfg_" +
        F.getName().str() +
        ".png";

    system(command.c_str());
}