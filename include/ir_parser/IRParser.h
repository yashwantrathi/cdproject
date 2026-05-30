#ifndef IRPARSER_H
#define IRPARSER_H

#include <string>
#include <vector>

#include "fingerprint/Fingerprint.h"

class IRParser {
public:
    void parseFile(const std::string& filename, bool verbose=false);

    std::vector<Fingerprint> getFingerprints();

    void generateGraph(const std::string& filename, const std::string& targetFunction, bool isCFG);

private:
    std::vector<Fingerprint> fingerprints;
};

#endif