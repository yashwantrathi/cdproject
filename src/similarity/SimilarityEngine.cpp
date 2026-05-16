#include "similarity/SimilarityEngine.h"

double SimilarityEngine::compare(
    const std::vector<std::string>& fp1,
    const std::vector<std::string>& fp2
) {

    int matches = 0;

    int minSize = std::min(fp1.size(), fp2.size());

    for (int i = 0; i < minSize; i++) {

        if (fp1[i] == fp2[i]) {
            matches++;
        }
    }

    int maxSize = std::max(fp1.size(), fp2.size());

    if (maxSize == 0)
        return 0.0;

    return (double(matches) / maxSize) * 100.0;
}