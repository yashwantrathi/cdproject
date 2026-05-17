#include "ir_parser/IRParser.h"
#include "similarity/SimilarityEngine.h"
#include "normalizer/Normalizer.h"
#include <iostream>
#include <fstream>
#include <filesystem>

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"
#define CYAN "\033[36m"

int main(int argc, char* argv[]) {
bool verbose = false;
double threshold = 70.0;
int totalComparisons = 0;
int clonesFound = 0;
    
if (std::string(argv[1]) == "--batch") {

    std::vector<std::string> files;

    for (const auto &entry :
         std::filesystem::directory_iterator("../testcases")) {

        std::string path = entry.path().string();

        if (path.find(".ll") != std::string::npos) {
            files.push_back(path);
        }
    }

    SimilarityEngine engine;
    std::ofstream csvFile("../results/results.csv");

csvFile << "Function1,Function2,Similarity,Clone\n";

    std::cout << CYAN
              << "\n========== BATCH MODE ==========\n"
              << RESET
              << std::endl;

    for (size_t i = 0; i < files.size(); i++) {

        for (size_t j = i + 1; j < files.size(); j++) {

            IRParser p1;
            IRParser p2;

            p1.parseFile(files[i], false);
            p2.parseFile(files[j], false);

            auto fp1 = p1.getFingerprints();
            auto fp2 = p2.getFingerprints();

            for (auto &f1 : fp1) {
              if (f1.getFunctionName() == "main")
    continue;
                for (auto &f2 : fp2) {
                  if (f2.getFunctionName() == "main")
    continue;
                    Normalizer normalizer;

                    auto norm1 =
                        normalizer.normalize(f1.getInstructions());

                    auto norm2 =
                        normalizer.normalize(f2.getInstructions());

                    double similarity =
                        engine.compare(norm1, norm2);

                    std::cout << f1.getFunctionName()
                              << " vs "
                              << f2.getFunctionName()
                              << std::endl;

                    std::cout << "Similarity: "
                              << similarity
                              << "%"
                              << std::endl;
                    csvFile << f1.getFunctionName()
        << ","
        << f2.getFunctionName()
        << ","
        << similarity
        << ",";          

                    if (similarity >= threshold) {

                        std::cout << GREEN
                                  << "Clone Detected!"
                                  << RESET
                                  << std::endl;
                                  csvFile << "YES\n";
                    }
                    else {

                        std::cout << RED
                                  << "No Clone Detected!"
                                  << RESET
                                  << std::endl;
                                  csvFile << "NO\n";
                    }

                    std::cout << std::endl;
                }
            }
        }
    }
csvFile.close();
    return 0;
}

    if (argc < 2) {
    std::cout << "Usage: ./clone_detector <file1.ll> <file2.ll> [--verbose]" << std::endl;
    return 1;
}

if (argc >= 4) {

    std::string arg = argv[3];

    if (arg == "--verbose") {
        verbose = true;
    }
    else {
        threshold = std::stod(arg);
    }
}

if (argc == 5) {

    std::string arg4 = argv[4];

    if (arg4 == "--verbose") {
        verbose = true;
    }
}
    IRParser parser1;
    IRParser parser2;

    parser1.parseFile(argv[1], verbose);
    parser2.parseFile(argv[2], verbose);

    std::vector<Fingerprint> fp1 = parser1.getFingerprints();
    std::vector<Fingerprint> fp2 = parser2.getFingerprints();

    SimilarityEngine engine;
    std::ofstream outFile("../results/output.txt");
    std::ofstream csvFile("../results/results.csv");

csvFile << "Function1,Function2,Similarity,Clone\n";
       
   std::cout << CYAN
          << "=================================\n"
          << " LLVM Clone Detection System\n"
          << "=================================\n"
          << RESET
          << std::endl;

std::cout << CYAN
          << "\n========== CROSS FILE COMPARISON ==========\n"
          << RESET;

std::cout << "Threshold: "
          << threshold
          << "%\n"
          << std::endl;

    for (auto &f1 : fp1) {
     if (f1.getFunctionName() == "main")
    continue;
        for (auto &f2 : fp2) {
          if (f2.getFunctionName() == "main")
    continue;
            Normalizer normalizer;

std::vector<std::string> norm1 =
    normalizer.normalize(f1.getInstructions());

std::vector<std::string> norm2 =
    normalizer.normalize(f2.getInstructions());

double similarity = engine.compare(norm1, norm2);
totalComparisons++;
            std::cout << f1.getFunctionName()
                      << " vs "
                      << f2.getFunctionName()
                      << std::endl;

            std::cout << "Similarity: "
                      << similarity
                      << "%"
                      << std::endl;
                      
outFile << f1.getFunctionName()
        << " vs "
        << f2.getFunctionName()
        << "\n";

outFile << "Similarity: "
        << similarity
        << "%\n";
csvFile << f1.getFunctionName()
        << ","
        << f2.getFunctionName()
        << ","
        << similarity
        << ",";

            if (similarity >= threshold){
                clonesFound++;
                csvFile << "YES\n";
                std::cout << GREEN
          << "Clone Detected!"
          << RESET
          << std::endl;
                outFile << "Clone Detected!\n";
            }
        else {
            

    std::cout << RED
              << "No Clone Detected!"
              << RESET
              << std::endl;
    
    outFile << "No Clone Detected!\n";
    csvFile << "NO\n";
}    

            std::cout << std::endl;
        }
        outFile << "\n";
    }
std::cout << CYAN
          << "\n========== FINAL STATS ==========\n"
          << RESET;

std::cout << "Total Comparisons: "
          << totalComparisons
          << std::endl;

std::cout << "Clones Found: "
          << clonesFound
          << std::endl;    
outFile.close();
csvFile.close();
    return 0;
}