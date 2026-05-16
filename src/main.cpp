#include "ir_parser/IRParser.h"
#include "similarity/SimilarityEngine.h"

#include <iostream>

int main() {

    IRParser parser;

    parser.parseFile("../testcases/sample.ll");

    std::vector<Fingerprint> fps = parser.getFingerprints();

    if (fps.size() < 2) {
        std::cout << "Need at least 2 functions for comparison." << std::endl;
        return 0;
    }

    SimilarityEngine engine;

    double similarity = engine.compare(
        fps[0].getInstructions(),
        fps[1].getInstructions()
    );

    std::cout << "\n========== SIMILARITY RESULT ==========" << std::endl;

    std::cout << fps[0].getFunctionName()
              << " vs "
              << fps[1].getFunctionName()
              << std::endl;

    std::cout << "Similarity: "
              << similarity
              << "%"
              << std::endl;

    return 0;
}