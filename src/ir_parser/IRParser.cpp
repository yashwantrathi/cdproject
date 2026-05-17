#include "ir_parser/IRParser.h"
#include "fingerprint/Fingerprint.h"
#include <iostream>

#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "cfg_builder/CFGBuilder.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/Support/MemoryBuffer.h"
using namespace llvm;

void IRParser::parseFile(const std::string& filename, bool verbose){

    LLVMContext context;
    SMDiagnostic error;

    std::unique_ptr<Module> module;

if (filename.find(".bc") != std::string::npos) {

    auto bufferOrError =
        MemoryBuffer::getFile(filename);

    if (!bufferOrError) {
        std::cerr << "Failed to open bitcode file!"
                  << std::endl;
        return;
    }

    auto moduleOrError =
        parseBitcodeFile(
            bufferOrError->get()->getMemBufferRef(),
            context
        );

    if (!moduleOrError) {
        std::cerr << "Failed to parse LLVM Bitcode!"
                  << std::endl;
        return;
    }

    module = std::move(*moduleOrError);
}
else {

    module = parseIRFile(filename, error, context);
}
    if (!module) {
        std::cerr << "Failed to parse LLVM IR!" << std::endl;
        return;
    }
   if(verbose)
    std::cout << "LLVM IR Parsed Successfully!\n" << std::endl;

    int functionCount = 0;
    int instructionCount = 0;
    
    for (Function &F : *module) {

        if (F.isDeclaration())
            continue;

        functionCount++;
        Fingerprint fp;
        fp.setFunctionName(F.getName().str());
        if(verbose)
    std::cout << "\nFunction: "
              << F.getName().str()
              << std::endl;

        for (BasicBlock &BB : F) {

            for (Instruction &I : BB) {

                instructionCount++;

                std::string opcode = I.getOpcodeName();
if(verbose)
  std::cout << "Opcode: " << opcode << std::endl;

fp.addInstruction(opcode);
            }
        }
       if(verbose){ 
        fp.printFingerprint();
        CFGBuilder cfg;
       
cfg.buildCFG(F, true);
       }
        fingerprints.push_back(fp);
    }
     
    if(verbose) {

    std::cout << "\n========== SUMMARY ==========" << std::endl;
    std::cout << "Functions Found: " << functionCount << std::endl;
    std::cout << "Instructions Found: " << instructionCount << std::endl;
}
   
}
std::vector<Fingerprint> IRParser::getFingerprints() {
    return fingerprints;
}