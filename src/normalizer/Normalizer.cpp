#include "normalizer/Normalizer.h"

std::vector<std::string> Normalizer::normalize(
    const std::vector<std::string>& instructions
) {

    std::vector<std::string> filtered;

    for (const auto& instr : instructions) {

        // Ignore noisy instructions
        if (instr == "alloca" ||
            instr == "load" ||
            instr == "store") {

            continue;
        }

        filtered.push_back(instr);
    }

    return filtered;
}