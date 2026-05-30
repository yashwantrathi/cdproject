#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <vector>
#include <string>

class Fingerprint {
public:
    void setFunctionName(const std::string& name);
void setSourceFile(const std::string& file);

std::string getSourceFile();
    void addInstruction(const std::string& opcode);

    void printFingerprint();
    std::vector<std::string> getInstructions();
    std::string getFunctionName();
    void setFingerprintHash(const std::string& hash);

std::string getFingerprintHash();
private:
std::string sourceFile;
    std::string functionName;
    std::string fingerprintHash;

    std::vector<std::string> instructions;
};

#endif