#include "ir_parser/IRParser.h"
#include "fingerprint/Fingerprint.h"
#include <iostream>

#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"

#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

void IRParser::parseFile(const std::string& filename) {

    LLVMContext context;
    SMDiagnostic error;

    std::unique_ptr<Module> module = parseIRFile(filename, error, context);

    if (!module) {
        std::cerr << "Failed to parse LLVM IR!" << std::endl;
        return;
    }

    std::cout << "LLVM IR Parsed Successfully!\n" << std::endl;

    int functionCount = 0;
    int instructionCount = 0;
    
    for (Function &F : *module) {

        if (F.isDeclaration())
            continue;

        functionCount++;
        Fingerprint fp;
        fp.setFunctionName(F.getName().str());
        std::cout << "\nFunction: " << F.getName().str() << std::endl;

        for (BasicBlock &BB : F) {

            for (Instruction &I : BB) {

                instructionCount++;

                std::string opcode = I.getOpcodeName();

std::cout << "Opcode: " << opcode << std::endl;

fp.addInstruction(opcode);
            }
        }
        fp.printFingerprint();
        fingerprints.push_back(fp);
    }
     
    std::cout << "\n========== SUMMARY ==========" << std::endl;
    std::cout << "Functions Found: " << functionCount << std::endl;
    std::cout << "Instructions Found: " << instructionCount << std::endl;
   
}
std::vector<Fingerprint> IRParser::getFingerprints() {
    return fingerprints;
}