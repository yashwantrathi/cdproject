#ifndef IRPARSER_H
#define IRPARSER_H

#include <string>
#include <vector>

#include "fingerprint/Fingerprint.h"

class IRParser {
public:
    void parseFile(const std::string& filename);

    std::vector<Fingerprint> getFingerprints();

private:
    std::vector<Fingerprint> fingerprints;
};

#endif