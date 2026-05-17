#include "cfg_builder/CFGBuilder.h"

#include <iostream>

#include "llvm/IR/CFG.h"
#include <fstream>
using namespace llvm;

void CFGBuilder::buildCFG(Function &F, bool exportGraph) {

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
    

   for (BasicBlock &BB : F) {

    std::cout << "BasicBlock: ";

    if (BB.hasName())
        std::cout << BB.getName().str();
    else
        std::cout << "(unnamed)";

    std::cout << std::endl;

    for (BasicBlock *Succ : successors(&BB)) {

        std::cout << "  -> ";

        if (Succ->hasName())
            std::cout << Succ->getName().str();
        else
            std::cout << "(unnamed)";

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

            dotFile << "\"" << from
                    << "\" -> \""
                    << to
                    << "\";\n";
        }
    }
}
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