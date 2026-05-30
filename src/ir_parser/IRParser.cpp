#include "ir_parser/IRParser.h"
#include "fingerprint/Fingerprint.h"
#include <iostream>

#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "cfg_builder/CFGBuilder.h"
#include "dfg_builder/DFGBuilder.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Demangle/Demangle.h"
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

        std::string demangled = llvm::demangle(F.getName().str());
        std::string mangled = F.getName().str();

        if(demangled.empty()) {
            demangled = mangled;
        }

        // Filter out compiler boilerplate and standard library functions
        // Check mangled names for C++ stdlib and compiler internals
        if (mangled.find("_ZSt") == 0 || 
            mangled.find("_ZNSt") == 0 || 
            mangled.find("_ZN9__gnu_cxx") == 0 ||
            mangled.find("_ZNKSt") == 0 ||
            mangled.find("__cxx_global_var_init") == 0 ||
            mangled.find("_GLOBAL__sub_I_") == 0) {
            continue;
        }

        // Check demangled names for Rust internals
        if (demangled.find("core::") == 0 ||
            demangled.find("alloc::") == 0 ||
            demangled.find("std::") == 0 ||
            demangled.find("_$LT$") != std::string::npos ||
            demangled.find("__") == 0 ||
            demangled.find("llvm.") == 0 ||
            demangled.find("rust_eh_personality") != std::string::npos ||
            demangled.find("_QQ") == 0) {
            continue;
        }

        functionCount++;
        Fingerprint fp;
        fp.setSourceFile(filename);
        fp.setFunctionName(demangled);
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
        DFGBuilder dfgBuilder;

dfgBuilder.buildDFG(F, verbose);
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

void IRParser::generateGraph(const std::string& filename, const std::string& targetFunction, bool isCFG) {
    LLVMContext context;
    SMDiagnostic error;
    std::unique_ptr<Module> module;

    if (filename.find(".bc") != std::string::npos) {
        auto bufferOrError = MemoryBuffer::getFile(filename);
        if (!bufferOrError) {
            std::cerr << "Failed to open bitcode file!" << std::endl;
            return;
        }
        auto moduleOrError = parseBitcodeFile(bufferOrError->get()->getMemBufferRef(), context);
        if (!moduleOrError) {
            std::cerr << "Failed to parse LLVM Bitcode!" << std::endl;
            return;
        }
        module = std::move(*moduleOrError);
    } else {
        module = parseIRFile(filename, error, context);
    }
    
    if (!module) {
        std::cerr << "Failed to parse LLVM IR!" << std::endl;
        return;
    }

    for (Function &F : *module) {
        if (F.isDeclaration()) continue;
        
        std::string demangled = llvm::demangle(F.getName().str());
        if (demangled.empty()) {
            demangled = F.getName().str();
        }

        if (demangled == targetFunction || F.getName().str() == targetFunction) {
            if (isCFG) {
                CFGBuilder cfg;
                cfg.buildCFG(F, true);
                std::cout << "GRAPH_GENERATED:../results/cfg_" << F.getName().str() << ".png" << std::endl;
            } else {
                DFGBuilder dfgBuilder;
                dfgBuilder.buildDFG(F, true); // true for verbose/exporting graph
                std::cout << "GRAPH_GENERATED:../results/dfg_" << F.getName().str() << ".png" << std::endl;
            }
            return;
        }
    }
    std::cerr << "Function " << targetFunction << " not found in " << filename << std::endl;
}