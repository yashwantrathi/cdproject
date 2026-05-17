#ifndef NORMALIZER_H
#define NORMALIZER_H

#include <vector>
#include <string>

class Normalizer {
public:
    std::vector<std::string> normalize(
        const std::vector<std::string>& instructions
    );
};

#endif