#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <vector>
#include <string>

class Fingerprint {
public:
    void setFunctionName(const std::string& name);

    void addInstruction(const std::string& opcode);

    void printFingerprint();
    std::vector<std::string> getInstructions();
    std::string getFunctionName();
private:
    std::string functionName;

    std::vector<std::string> instructions;
};

#endif