# IMPLEMENTATION.md — IRClone: Cross-Language LLVM Clone Detector

> *This document covers the LLVM API usage, normalization passes, fingerprint algorithm, similarity scoring formula, and key implementation decisions made during development.*

---

## 1. Technology Baseline

The analysis engine is written in **C++17** and links against the **LLVM 18** static libraries. All LLVM analysis is performed at the Module/Function/BasicBlock/Instruction hierarchy level using the `llvm-c++` API.

**Core LLVM headers used:**

```cpp
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/CFG.h"
```

**Build system:** CMake 3.20+ using `llvm-config --cxxflags --ldflags --libs` to inject the correct compilation and linkage flags.

---

## 2. Compilation to LLVM IR

Before the analysis engine sees any input, source files must be compiled to human-readable LLVM IR (`.ll` format). The web backend (`app.py`) handles this via `subprocess` calls; the CLI pipeline expects pre-compiled `.ll` files.

### Command Reference

```bash
# C
clang -S -emit-llvm -O0 -Xclang -disable-O0-optnone source.c -o source.ll

# C++
clang++ -S -emit-llvm -O0 -Xclang -disable-O0-optnone source.cpp -o source.ll

# Rust
rustc --emit=llvm-ir --edition 2021 -C opt-level=0 source.rs -o source.ll

# Fortran
flang-new -S -emit-llvm -O0 source.f90 -o source.ll
```

The `-disable-O0-optnone` flag is critical for C/C++: without it, `clang -O0` marks every function `optnone`, which prevents later LLVM normalization passes from running. For Rust, `opt-level=0` is the equivalent flag.

---

## 3. IR Loading and Parsing

The entry point for all IR analysis is `IRParser::loadModule()`:

```cpp
// src/ir_parser/IRParser.cpp
std::unique_ptr<llvm::Module> IRParser::loadModule(
    const std::string& filepath, llvm::LLVMContext& ctx)
{
    llvm::SMDiagnostic err;
    auto mod = llvm::parseIRFile(filepath, err, ctx);
    if (!mod) {
        err.print("IRClone", llvm::errs());
        return nullptr;
    }
    return mod;
}
```

Once the `Module` is loaded, the parser traverses its function list:

```cpp
for (llvm::Function& F : *module) {
    // Skip external declarations (no body)
    if (F.isDeclaration()) continue;
    // Skip LLVM intrinsics
    if (F.getName().startswith("llvm.")) continue;
    // Skip Rust standard library functions
    if (F.getName().contains("core::") || 
        F.getName().contains("std::"))  continue;

    procesFunction(F);
}
```

Skipping external declarations is important: languages like Rust include hundreds of `declare` stubs for standard library functions in their IR output. Including these would pollute the fingerprint with irrelevant entries.

---

## 4. IR Normalization

The `IRNormalizer` module transforms raw, language-specific IR into a clean, stable representation. It operates at the string level on the `.ll` file text before parsing, and at the `Module` level after parsing.

### 4.1 Metadata Stripping (Text Pre-Pass)

The normalizer reads the raw `.ll` file and strips the following using regex patterns:

| Pattern | Rationale |
|---|---|
| `!dbg !N` | DWARF debug location info — varies by compiler, not semantic |
| `!tbaa !N` | Type-based alias analysis hints — C++ specific |
| `!srcloc !N` | Fortran source location metadata |
| `!llvm.dbg.cu` | Compilation unit debug records |
| Attribute groups `#N { ... }` | Function attribute sets — contain lang-specific attrs |
| `attributes #N` declarations | Same |

### 4.2 Module-Level Function Filtering (Post-Parse)

After the module is loaded, the normalizer removes functions that should not participate in fingerprinting:

```cpp
// Rust runtime/panic infrastructure
const std::vector<std::string> RUST_SKIP_PREFIXES = {
    "core::panicking::",
    "core::slice::",
    "core::fmt::",
    "rust_begin_unwind",
    "__rust_alloc",
    "_ZN4core",          // Core crate mangled names
};

// C++ standard library
const std::vector<std::string> CPP_SKIP_PREFIXES = {
    "_ZNSt",             // std:: mangled
    "_ZNK",              // const member functions
    "__cxa_",            // C++ ABI runtime
};
```

**Design note:** The normalizer intentionally does *not* remove Rust's bounds-check `br` instructions (the `panic` call sites are removed, but the `br i1` guard remains). This is a deliberate choice to not over-normalize: removing the branch would artificially inflate Rust/C similarity scores. See EVALUATION.md §4 for the accuracy impact.

---

## 5. Fingerprint Extraction

Each function is represented as a `Fingerprint` structure:

```cpp
// include/fingerprint/Fingerprint.h
struct Fingerprint {
    std::string functionName;
    std::string sourceFile;
    int totalInstructions;
    std::map<std::string, int> opcodeFreq;  // opcode → count
    int basicBlockCount;
    int edgeCount;
    int cyclomaticComplexity;
};
```

### 5.1 Opcode Frequency Collection

```cpp
// src/fingerprint/Fingerprint.cpp
void Fingerprint::extract(const llvm::Function& F) {
    for (const llvm::BasicBlock& BB : F) {
        basicBlockCount++;
        for (const llvm::Instruction& I : BB) {
            // getOpcodeName() returns "add", "load", "store", "br", etc.
            std::string opcode = I.getOpcodeName();
            opcodeFreq[opcode]++;
            totalInstructions++;
        }
        // Count outgoing edges for cyclomatic complexity
        edgeCount += std::distance(
            llvm::succ_begin(&BB), llvm::succ_end(&BB));
    }
    // Cyclomatic complexity: E - N + 2
    cyclomaticComplexity = edgeCount - basicBlockCount + 2;
}
```

The resulting `opcodeFreq` map for a simple bubble sort looks like:

```
{alloca: 4, store: 12, load: 18, icmp: 4, br: 5,
 getelementptr: 8, add: 4, sub: 2, ret: 1}
```

---

## 6. Control Flow Graph (CFG) Builder

### 6.1 Graph Construction

```cpp
// src/cfg_builder/CFGBuilder.cpp
void CFGBuilder::buildAndExport(const llvm::Function& F,
                                 const std::string& outputPath)
{
    std::ofstream dotFile(outputPath + ".dot");
    dotFile << "digraph \"CFG_" << F.getName().str() << "\" {\n";
    dotFile << "  node [shape=box, fontname=\"JetBrains Mono\"];\n";

    int bbIndex = 0;
    std::map<const llvm::BasicBlock*, int> bbIds;

    for (const llvm::BasicBlock& BB : F) {
        bbIds[&BB] = bbIndex++;
        // Build label from first/last instruction
        std::string label = buildBlockLabel(BB);
        dotFile << "  node" << bbIds[&BB]
                << " [label=\"" << label << "\"];\n";
    }

    // Draw edges
    for (const llvm::BasicBlock& BB : F) {
        for (const llvm::BasicBlock* succ : llvm::successors(&BB)) {
            dotFile << "  node" << bbIds[&BB]
                    << " -> node" << bbIds[succ] << ";\n";
        }
    }
    dotFile << "}\n";
    dotFile.close();

    // Render to PNG
    std::string cmd = "dot -Tpng " + outputPath + ".dot -o " + outputPath + ".png";
    system(cmd.c_str());
}
```

### 6.2 Block Label Generation

Each node label shows the first two and last instruction of the basic block, making the graph human-readable:

```
┌─────────────────────┐
│ BB0: entry          │
│  %i = alloca i32    │
│  ...                │
│  br label %BB1      │
└─────────────────────┘
```

---

## 7. Data Flow Graph (DFG) Builder

The DFG shows which instructions depend on which, exposing the computational skeleton of a function.

### 7.1 Node ID Assignment

A critical early design bug caused disconnected graphs because node IDs were assigned from a counter that reset per basic block. The fix was to use a stable `std::map<Instruction*, std::string>` keyed on the pointer value:

```cpp
// src/dfg_builder/DFGBuilder.cpp
std::map<const llvm::Instruction*, std::string> nodeIds;

// First pass: assign unique IDs to all instructions
int counter = 0;
for (const llvm::BasicBlock& BB : F) {
    for (const llvm::Instruction& I : BB) {
        nodeIds[&I] = "n" + std::to_string(counter++);
    }
}
```

### 7.2 Edge Drawing (Use-Def Chains)

```cpp
// Second pass: draw use-def edges
for (const llvm::BasicBlock& BB : F) {
    for (const llvm::Instruction& I : BB) {
        std::string userId = nodeIds[&I];
        
        for (const llvm::Use& use : I.operands()) {
            // Only draw edges for instruction operands (not constants/args)
            if (const auto* operandInst =
                    llvm::dyn_cast<llvm::Instruction>(use.get())) {
                std::string producerId = nodeIds[operandInst];
                dotFile << "  " << producerId
                        << " -> " << userId << ";\n";
            }
        }
    }
}
```

---

## 8. Similarity Engine

### 8.1 Cosine Similarity on Opcode Vectors

Given two fingerprints A and B with opcode frequency maps, the cosine similarity is:

```
           Σ (A[op] × B[op])
cos(A,B) = ───────────────────────────
           √(Σ A[op]²) × √(Σ B[op]²)
```

Implementation:

```cpp
// src/similarity/SimilarityEngine.cpp
double SimilarityEngine::cosineSimilarity(
    const std::map<std::string, int>& vecA,
    const std::map<std::string, int>& vecB)
{
    double dot = 0.0, normA = 0.0, normB = 0.0;

    // Collect all opcodes from both maps
    std::set<std::string> allOpcodes;
    for (auto& [op, _] : vecA) allOpcodes.insert(op);
    for (auto& [op, _] : vecB) allOpcodes.insert(op);

    for (const auto& op : allOpcodes) {
        double a = vecA.count(op) ? vecA.at(op) : 0.0;
        double b = vecB.count(op) ? vecB.at(op) : 0.0;
        dot   += a * b;
        normA += a * a;
        normB += b * b;
    }

    if (normA == 0.0 || normB == 0.0) return 0.0;
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}
```

### 8.2 CFG Topology Score

```cpp
double SimilarityEngine::cfgTopologyScore(
    const Fingerprint& A, const Fingerprint& B)
{
    double nodeDelta = std::abs(A.basicBlockCount - B.basicBlockCount);
    double edgeDelta = std::abs(A.edgeCount - B.edgeCount);
    double maxNodes  = std::max(A.basicBlockCount, B.basicBlockCount);
    double maxEdges  = std::max(A.edgeCount, B.edgeCount);

    if (maxNodes + maxEdges == 0) return 1.0;
    return 1.0 - (nodeDelta + edgeDelta) / (maxNodes + maxEdges);
}
```

### 8.3 Final Score and Classification

```cpp
double SimilarityEngine::compare(const Fingerprint& A, const Fingerprint& B,
                                  double threshold)
{
    double opcodeSim = cosineSimilarity(A.opcodeFreq, B.opcodeFreq);
    double cfgSim    = cfgTopologyScore(A, B);

    // Weighted blend: 70% opcode similarity, 30% structural similarity
    double finalScore = (0.70 * opcodeSim + 0.30 * cfgSim) * 100.0;

    std::string label;
    if (finalScore >= threshold)       label = "STRONG CLONE DETECTED";
    else if (finalScore >= 40.0)       label = "PARTIAL SIMILARITY";
    else                               label = "DIFFERENT — No Clone";

    return finalScore;
}
```

---

## 9. CLI Interface

### Flag Parsing

```cpp
// src/main.cpp (simplified)
int main(int argc, char* argv[]) {
    if (argc < 3) { printUsage(); return 1; }

    std::string file1    = argv[1];
    std::string file2    = argv[2];
    double threshold     = 70.0;
    bool verbose         = false;
    std::string cfgFunc  = "";
    std::string dfgFunc  = "";

    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--verbose")          verbose = true;
        if (arg == "--threshold" && i+1 < argc) threshold = std::stod(argv[++i]);
        if (arg == "--cfg" && i+1 < argc)       cfgFunc   = argv[++i];
        if (arg == "--dfg" && i+1 < argc)       dfgFunc   = argv[++i];
    }

    // ... pipeline execution ...
}
```

---

## 10. Build Configuration (CMakeLists.txt)

The CMake configuration uses `llvm-config` to automatically pick up the correct LLVM 18 installation:

```cmake
cmake_minimum_required(VERSION 3.20)
project(IRClone CXX)

set(CMAKE_CXX_STANDARD 17)

# Locate LLVM 18
find_package(LLVM 18 REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(clone_detector ${SOURCES})

# Link LLVM components needed
llvm_map_components_to_libnames(LLVM_LIBS
    core support irreader analysis)

target_link_libraries(clone_detector ${LLVM_LIBS})
target_include_directories(clone_detector PRIVATE include/)
```

---

## 11. Web Backend: Flask API

The Flask server (`web/backend/app.py`) bridges the React frontend and the C++ binary. Key routes:

| Route | Method | Description |
|---|---|---|
| `/api/testcases` | GET | Lists all benchmark files |
| `/api/testcase_content` | GET | Returns raw source code for preview |
| `/api/compare_testcases` | POST | Compiles files → runs binary → returns JSON |
| `/api/upload` | POST | Accepts user-uploaded files, same pipeline |
| `/api/generate_graph` | POST | Runs `--cfg`/`--dfg` flag, returns Base64 PNG + DOT text |

The compilation step in `app.py` runs `clang`/`rustc`/`flang-new` via `subprocess.run()`, writes the `.ll` output to a temp directory, then passes the paths to `clone_detector`. The stdout of the binary is parsed into structured JSON.

---

## 12. Error Handling

| Error Case | Handling |
|---|---|
| IR file not found | `parseIRFile()` returns null; prints diagnostic via `SMDiagnostic` |
| Graphviz not installed | `system("dot ...")` returns non-zero; error logged, no PNG generated |
| Rust compilation failure | Flask catches `subprocess.CalledProcessError`, returns 400 with stderr |
| Empty function body | Fingerprint returns zero-length opcode map; similarity score = 0.0 |
| Two identical files | Cosine similarity = 1.0, CFG score = 1.0 → 100% |

---

*See [EVALUATION.md](EVALUATION.md) for how the system performs against the benchmark corpus.*
