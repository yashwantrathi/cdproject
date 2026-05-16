#ifndef SIMILARITYENGINE_H
#define SIMILARITYENGINE_H

#include <vector>
#include <string>

class SimilarityEngine {
public:
    double compare(
        const std::vector<std::string>& fp1,
        const std::vector<std::string>& fp2
    );
};

#endif