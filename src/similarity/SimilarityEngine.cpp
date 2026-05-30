#include "similarity/SimilarityEngine.h"
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <set>

double SimilarityEngine::compare(
    const std::vector<std::string>& fp1,
    const std::vector<std::string>& fp2
) {
    if (fp1.empty() && fp2.empty()) return 100.0;
    if (fp1.empty() || fp2.empty()) return 0.0;

    std::unordered_map<std::string, int> count1;
    std::unordered_map<std::string, int> count2;
    std::set<std::string> uniqueOps;

    for (const auto& op : fp1) {
        count1[op]++;
        uniqueOps.insert(op);
    }
    for (const auto& op : fp2) {
        count2[op]++;
        uniqueOps.insert(op);
    }

    // Jaccard similarity (intersection / union) on multisets
    int intersectionCount = 0;
    int unionCount = 0;
    
    for (const auto& op : uniqueOps) {
        intersectionCount += std::min(count1[op], count2[op]);
        unionCount += std::max(count1[op], count2[op]);
    }
    
    double jaccard = (unionCount == 0) ? 0.0 : (double)intersectionCount / unionCount;

    // Cosine similarity
    double dotProduct = 0.0;
    double norm1 = 0.0;
    double norm2 = 0.0;

    for (const auto& op : uniqueOps) {
        dotProduct += (double)count1[op] * count2[op];
        norm1 += (double)count1[op] * count1[op];
        norm2 += (double)count2[op] * count2[op];
    }

    double cosine = 0.0;
    if (norm1 > 0 && norm2 > 0) {
        cosine = dotProduct / (std::sqrt(norm1) * std::sqrt(norm2));
    }

    // Longest Common Subsequence (LCS)
    int m = fp1.size();
    int n = fp2.size();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (fp1[i-1] == fp2[j-1]) {
                dp[i][j] = dp[i-1][j-1] + 1;
            } else {
                dp[i][j] = std::max(dp[i-1][j], dp[i][j-1]);
            }
        }
    }
    double lcs_ratio = 0.0;
    if (std::max(m, n) > 0) {
        lcs_ratio = (double)dp[m][n] / std::max(m, n);
    }

    // Length Ratio Penalty
    double len1 = fp1.size();
    double len2 = fp2.size();
    double lengthRatio = std::min(len1, len2) / std::max(len1, len2);

    // Weighted similarity: 30% Jaccard, 20% Cosine, 40% LCS, 10% Length Ratio
    double similarity = (0.3 * jaccard + 0.2 * cosine + 0.4 * lcs_ratio + 0.1 * lengthRatio) * 100.0;

    return similarity;
}