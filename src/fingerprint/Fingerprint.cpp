#include "fingerprint/Fingerprint.h"

#include <iostream>

void Fingerprint::setFunctionName(const std::string& name) {
    functionName = name;
}

void Fingerprint::addInstruction(const std::string& opcode) {
    instructions.push_back(opcode);
}

void Fingerprint::printFingerprint() {

    std::cout << "\n====== FUNCTION FINGERPRINT ======" << std::endl;

    std::cout << "Function: " << functionName << "\n" << std::endl;

    for (const auto& instr : instructions) {
        std::cout << instr << std::endl;
    }
}
std::vector<std::string> Fingerprint::getInstructions() {
    return instructions;
}
std::string Fingerprint::getFunctionName() {
    return functionName;
}
void Fingerprint::setSourceFile(
    const std::string& file
) {
    sourceFile = file;
}

std::string Fingerprint::getSourceFile() {
    return sourceFile;
}
void Fingerprint::setFingerprintHash(
    const std::string& hash
) {
    fingerprintHash = hash;
}

std::string Fingerprint::getFingerprintHash() {
    return fingerprintHash;
}