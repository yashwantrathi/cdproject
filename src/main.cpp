#include "ir_parser/IRParser.h"
#include "similarity/SimilarityEngine.h"
#include "normalizer/Normalizer.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <functional>
#include <vector>
#include <string>

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"
#define CYAN "\033[36m"

// Function to escape JSON strings
std::string escapeJSON(const std::string& s) {
    std::string res;
    for (char c : s) {
        if (c == '"') res += "\\\"";
        else if (c == '\\') res += "\\\\";
        else if (c == '\b') res += "\\b";
        else if (c == '\f') res += "\\f";
        else if (c == '\n') res += "\\n";
        else if (c == '\r') res += "\\r";
        else if (c == '\t') res += "\\t";
        else res += c;
    }
    return res;
}

int main(int argc, char* argv[]) {
    bool verbose = false;
    double threshold = 65.0;
    bool batch_mode = false;
    bool multi_mode = false;
    bool json_output = false;
    std::vector<std::string> files;

    if (argc < 2) {
        std::cout << "Usage: ./clone_detector <file1.ll> <file2.ll> [--verbose] [threshold]\n"
                  << "       ./clone_detector --batch [--json]\n"
                  << "       ./clone_detector --multi <file1.ll> <file2.ll> ... [--json]\n"
                  << "       ./clone_detector --cfg <file.ll> <function_name>\n"
                  << "       ./clone_detector --dfg <file.ll> <function_name>" << std::endl;
        return 1;
    }

    std::string first_arg = argv[1];
    if (first_arg == "--cfg" || first_arg == "--dfg") {
        if (argc < 4) {
            std::cerr << "Usage: ./clone_detector " << first_arg << " <file.ll> <function_name>" << std::endl;
            return 1;
        }
        std::string filename = argv[2];
        std::string targetFunc = argv[3];
        
        IRParser parser;
        parser.generateGraph(filename, targetFunc, first_arg == "--cfg");
        
        return 0;
    } else if (first_arg == "--batch") {
        batch_mode = true;
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--json") json_output = true;
            else if (arg == "--verbose") verbose = true;
            else threshold = std::stod(arg);
        }
        for (const auto &entry : std::filesystem::directory_iterator("../testcases")) {
            std::string path = entry.path().string();
            if (path.find(".ll") != std::string::npos) {
                files.push_back(path);
            }
        }
    } else if (first_arg == "--multi") {
        multi_mode = true;
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--json") json_output = true;
            else if (arg == "--verbose") verbose = true;
            else if (arg.find(".ll") != std::string::npos) files.push_back(arg);
            else threshold = std::stod(arg);
        }
    } else {
        files.push_back(argv[1]);
        if (argc >= 3) {
            std::string second_arg = argv[2];
            if (second_arg.find(".ll") != std::string::npos) {
                files.push_back(second_arg);
                for (int i = 3; i < argc; ++i) {
                    std::string arg = argv[i];
                    if (arg == "--json") json_output = true;
                    else if (arg == "--verbose") verbose = true;
                    else threshold = std::stod(arg);
                }
            }
        }
    }

    if (files.size() < 2) {
        std::cerr << "Error: Need at least 2 files to compare." << std::endl;
        return 1;
    }

    SimilarityEngine engine;
    int totalComparisons = 0;
    int clonesFound = 0;

    std::ofstream csvFile("../results/results.csv", std::ios::app);
    std::ofstream outFile("../results/output.txt");
    std::ofstream dbFile("../results/signature_db.txt", std::ios::app);

    if (!json_output) {
        std::cout << CYAN << "=================================\n"
                  << " LLVM Clone Detection System\n"
                  << "=================================\n" << RESET << std::endl;
        if (batch_mode) std::cout << CYAN << "\n========== BATCH MODE ==========\n" << RESET << std::endl;
        else if (multi_mode) std::cout << CYAN << "\n========== MULTI-FILE MODE ==========\n" << RESET << std::endl;
        else std::cout << CYAN << "\n========== CROSS FILE COMPARISON ==========\n" << RESET;
        std::cout << "Threshold: " << threshold << "%\n" << std::endl;
    } else {
        std::cout << "[\n";
    }

    bool first_json = true;

    for (size_t i = 0; i < files.size(); i++) {
        for (size_t j = i + 1; j < files.size(); j++) {
            IRParser p1;
            IRParser p2;

            p1.parseFile(files[i], verbose && !json_output);
            p2.parseFile(files[j], verbose && !json_output);

            auto fp1 = p1.getFingerprints();
            auto fp2 = p2.getFingerprints();

            for (auto &f1 : fp1) {
                if (f1.getFunctionName() == "main") continue;
                for (auto &f2 : fp2) {
                    if (f2.getFunctionName() == "main") continue;

                    Normalizer normalizer;
                    auto norm1 = normalizer.normalize(f1.getInstructions());
                    auto norm2 = normalizer.normalize(f2.getInstructions());

                    std::stringstream ss1;
                    for (const auto &inst : norm1) ss1 << inst;
                    std::size_t hash1 = std::hash<std::string>{}(ss1.str());
                    f1.setFingerprintHash(std::to_string(hash1));

                    std::stringstream ss2;
                    for (const auto &inst : norm2) ss2 << inst;
                    std::size_t hash2 = std::hash<std::string>{}(ss2.str());
                    f2.setFingerprintHash(std::to_string(hash2));

                    double similarity = engine.compare(norm1, norm2);
                    totalComparisons++;

                    std::string resultType;
                    if (similarity >= threshold) {
                        resultType = "Strong Clone";
                        clonesFound++;
                    } else if (similarity >= 25.0) {
                        resultType = "Partial Similarity";
                    } else {
                        resultType = "Different Logic";
                    }

                    if (json_output) {
                        if (!first_json) std::cout << ",\n";
                        std::cout << "  {\n"
                                  << "    \"file1\": \"" << escapeJSON(f1.getSourceFile()) << "\",\n"
                                  << "    \"func1\": \"" << escapeJSON(f1.getFunctionName()) << "\",\n"
                                  << "    \"file2\": \"" << escapeJSON(f2.getSourceFile()) << "\",\n"
                                  << "    \"func2\": \"" << escapeJSON(f2.getFunctionName()) << "\",\n"
                                  << "    \"similarity\": " << similarity << ",\n"
                                  << "    \"result\": \"" << resultType << "\"\n"
                                  << "  }";
                        first_json = false;
                    } else {
                        std::cout << f1.getSourceFile() << "::" << f1.getFunctionName() << "\nvs\n"
                                  << f2.getSourceFile() << "::" << f2.getFunctionName() << std::endl;
                        std::cout << "Similarity: " << similarity << "%" << std::endl;

                        if (resultType == "Strong Clone") std::cout << GREEN << "Strong Clone Detected!" << RESET << std::endl;
                        else if (resultType == "Partial Similarity") std::cout << CYAN << "Partial Structural Similarity" << RESET << std::endl;
                        else std::cout << RED << "Different Algorithmic Logic" << RESET << std::endl;
                        std::cout << std::endl;

                        outFile << f1.getFunctionName() << " vs " << f2.getFunctionName() << "\n";
                        outFile << "Similarity: " << similarity << "%\n";
                        outFile << resultType << "\n\n";

                        dbFile << "Function: " << f1.getFunctionName() << "\nHash: " << f1.getFingerprintHash() << "\nSource: " << f1.getSourceFile() << "\n\n";
                        dbFile << "Function: " << f2.getFunctionName() << "\nHash: " << f2.getFingerprintHash() << "\nSource: " << f2.getSourceFile() << "\n";
                        dbFile << "--------------------------\n";
                    }

                    csvFile << "\"" << f1.getFunctionName() << "\",\"" << f2.getFunctionName() << "\"," << similarity << "," << resultType << "\n";
                }
            }
        }
    }

    if (json_output) {
        std::cout << "\n]\n";
    } else {
        std::cout << CYAN << "\n========== FINAL STATS ==========\n" << RESET;
        std::cout << "Total Comparisons: " << totalComparisons << std::endl;
        std::cout << "Clones Found: " << clonesFound << std::endl;
    }

    csvFile.close();
    outFile.close();
    dbFile.close();

    return 0;
}