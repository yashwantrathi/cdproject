#include "normalizer/Normalizer.h"

std::vector<std::string>
Normalizer::normalize(
    const std::vector<std::string>& instructions
) {

    std::vector<std::string> normalized;

    for (const auto &inst : instructions) {

        // Ignore memory allocation noise
        if (inst == "alloca")
            continue;

        // Ignore debug-related instructions
        if (inst == "dbg")
            continue;

        // Ignore lifetime instructions
        if (inst == "lifetime.start")
            continue;

        if (inst == "lifetime.end")
            continue;

        // Ignore compiler-specific LLVM noise and Rust/Fortran artifacts
        if (inst == "sext" ||
            inst == "trunc" ||
            inst == "phi" ||
            inst == "insertvalue" ||
            inst == "freeze" ||
            inst == "unreachable" ||
            inst == "fence" ||
            inst == "zext" ||
            inst == "bitcast" ||
            inst == "select" ||
            inst == "extractvalue") {

            continue;
        }

        // Ignore memory-access related noise
        if (inst == "getelementptr")
            continue;

        // Normalize arithmetic instructions (Collapse consecutive ones to reduce Fortran array math noise)
        if (inst == "add" || inst == "fadd" ||
            inst == "sub" || inst == "fsub" ||
            inst == "mul" || inst == "fmul" ||
            inst == "sdiv" || inst == "udiv" || inst == "fdiv") {
            
            if (normalized.empty() || normalized.back() != "ARITHMETIC") {
                normalized.push_back("ARITHMETIC");
            }
        }
        // Normalize branches
        else if (inst == "br" || inst == "switch") {
            normalized.push_back("BRANCH");
        }
        // Normalize comparisons
        else if (inst == "icmp" || inst == "fcmp") {
            normalized.push_back("COMPARE");
        }
        // Normalize memory accesses
        else if (inst == "load" || inst == "store") {
            normalized.push_back("MEM_ACCESS");
        }
        // Normalize calls
        else if (inst == "call" || inst == "invoke") {
            normalized.push_back("CALL");
        }
        // Normalize returns
        else if (inst == "ret") {
            normalized.push_back("RETURN");
        }
        // Keep important instructions
        else {
            normalized.push_back(inst);
        }
    }

    // Generate instruction patterns
    std::vector<std::string> patterns;

    for (size_t i = 0; i + 1 < normalized.size(); i++) {

        patterns.push_back(
            normalized[i]
            + "_"
            + normalized[i + 1]
        );
    }

    // Add patterns into final vector
    normalized.insert(
        normalized.end(),
        patterns.begin(),
        patterns.end()
    );

    return normalized;
}