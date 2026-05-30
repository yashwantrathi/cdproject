# DESIGN.md — IRClone: Cross-Language LLVM Clone Detector

> *This document covers the system architecture, design rationale, key algorithmic decisions, and alternatives considered during the development of IRClone.*

---

## 1. Problem Statement

Existing clone detection tools (NiCad, CCFinder, SourcererCC) operate on source text or language-specific ASTs. This makes them fundamentally incapable of detecting **semantic clones across language boundaries** — for example, recognising that a C bubble sort and a Fortran bubble sort implement the same algorithm.

The root cause: source code is syntax, and syntax is language-specific.

**IRClone's answer:** move the comparison layer down to **LLVM Intermediate Representation (IR)** — the compiler's internal bytecode that all LLVM-frontend languages share. At this level, a 32-bit addition is always `add i32`, an array load is always `getelementptr` followed by `load`, and a conditional branch is always `br i1`. The surface syntax of the source language becomes irrelevant.

---

## 2. System Architecture

IRClone follows a **linear analysis pipeline** with clearly separated, independently testable modules.

```
┌──────────────────────────────────────────────────────────────────┐
│                          INPUT LAYER                             │
│   Source files: .c / .cpp / .rs / .f90                          │
└──────────────────────────┬───────────────────────────────────────┘
                           │
                           ▼
┌──────────────────────────────────────────────────────────────────┐
│                    COMPILATION LAYER                             │
│   clang / clang++ / rustc --emit=llvm-ir / flang-new            │
│   Produces: function1.ll, function2.ll                           │
└──────────────────────────┬───────────────────────────────────────┘
                           │
                           ▼
┌──────────────────────────────────────────────────────────────────┐
│                    NORMALIZATION LAYER                           │
│   IRNormalizer.cpp                                               │
│   • Strip debug info (!dbg, !tbaa, !llvm.dbg.*)                 │
│   • Remove attributes (nounwind, uwtable, mustprogress...)       │
│   • Filter external declarations (core::, std::, _ZN...)        │
│   • Strip Rust panic/unwind infrastructure                       │
└──────────────────────────┬───────────────────────────────────────┘
                           │
                           ▼
┌──────────────────────────────────────────────────────────────────┐
│                    FINGERPRINT LAYER                             │
│                                                                  │
│  ┌────────────────────┐    ┌────────────────────────────────┐   │
│  │    CFG Builder     │    │         DFG Builder            │   │
│  │                    │    │                                │   │
│  │  • Basic blocks    │    │  • Instruction* → node ID map  │   │
│  │  • Successor edges │    │  • Use-def edges               │   │
│  │  • Cyclomatic CC   │    │  • Output: DOT + PNG           │   │
│  │  • Opcode freq map │    │                                │   │
│  └────────┬───────────┘    └───────────────┬────────────────┘   │
│           │                                │                    │
│           └────────────┬───────────────────┘                    │
│                        │  Combined Fingerprint                   │
└────────────────────────┼───────────────────────────────────────┘
                         │
                         ▼
┌──────────────────────────────────────────────────────────────────┐
│                    SIMILARITY ENGINE                             │
│   • Cosine similarity on opcode frequency vectors               │
│   • CFG structural weight (node/edge count delta)               │
│   • Threshold classification: Strong / Partial / Different      │
└──────────────────────────┬───────────────────────────────────────┘
                           │
                           ▼
┌──────────────────────────────────────────────────────────────────┐
│                       OUTPUT LAYER                               │
│   CLI (stdout)  •  CSV (results/)  •  Graphviz PNGs             │
│   Web Dashboard (React + Flask)    •  Heatmap / Bar charts       │
└──────────────────────────────────────────────────────────────────┘
```

---

## 3. Core Design Decisions

### 3.1 Why LLVM IR and not ASTs?

| Approach | Pros | Cons |
|---|---|---|
| Source-level text | Simple, fast | Syntax-dependent, language-specific |
| Language ASTs | Rich structural info | Each language needs a separate parser |
| **LLVM IR** | **Language-agnostic, compiler-validated** | **Verbose IR; normalization required** |
| Bytecode (JVM/WASM) | Compact | Limited language support |
| Machine code | Truly binary-level | Too low-level; register allocation noise |

LLVM IR sits at the ideal abstraction level: above hardware-specific details, but below language-specific syntax. Every major systems language (C, C++, Rust, Fortran, Swift, Zig, Kotlin Native) has an LLVM frontend, making it the most broadly applicable choice.

### 3.2 Why Opcode Frequency Vectors?

Opcode frequency vectors represent each function as a histogram of instruction types (e.g., `{add: 3, load: 7, store: 4, icmp: 2, br: 3}`). This was chosen over raw opcode sequences because:

- **Order-invariant:** Two implementations of the same algorithm might emit the same opcodes in a slightly different order depending on the compiler's instruction scheduling.
- **Compact:** A vector of ~50 LLVM opcode types compresses an arbitrarily large function into a fixed-length representation.
- **Cosine similarity:** Cosine similarity on frequency vectors is well-understood, parameter-free, and handles functions of different sizes gracefully (unlike Hamming distance).

The primary weakness is that frequency vectors are **topology-blind** — two completely different algorithms can produce similar histograms if they happen to use the same primitive operations (e.g., Bubble Sort vs Merge Sort both do heavy `load`/`store`). This is mitigated by the CFG structural weight component.

### 3.3 CFG Structural Weighting

The final similarity score blends two signals:

```
final_score = (0.7 × opcode_cosine_similarity) + (0.3 × cfg_topology_score)
```

The `cfg_topology_score` is computed as:

```
cfg_topology_score = 1.0 - (|Δnodes| + |Δedges|) / (max_nodes + max_edges)
```

This penalizes pairs where one function has a nested loop (4–5 nodes) and the other is a simple linear function (1–2 nodes), even if their opcode distributions happen to be similar.

### 3.4 Data Flow Graph Design

The DFG captures **instruction-level use-def dependencies**. Each LLVM `Instruction*` is assigned a stable unique ID (via a `std::map<Instruction*, std::string>`), and a directed edge `A → B` is drawn whenever B is an operand of A. This produces a clean dependency graph showing exactly which computations feed into which — useful for visually verifying that two functions have the same computational backbone.

An early design used index-based IDs, which produced disconnected graphs when IR instructions were non-linearly ordered. The `Instruction*` pointer-keyed map fixed this by guaranteeing stable IDs regardless of traversal order.

### 3.5 Normalization Strategy

LLVM IR varies significantly between languages even for equivalent logic:

| IR Pattern | Language | Action |
|---|---|---|
| `!dbg`, `!tbaa`, `!srcloc` metadata | All | Strip entirely |
| `nounwind`, `uwtable`, `mustprogress` attrs | All | Strip entirely |
| `_ZN` (C++ mangled names) | C++ | Keep, normalize function name for display |
| `core::panicking`, `core::slice::` calls | Rust | Strip call instructions |
| Rust bounds-check `br i1` blocks | Rust | Keep (causes known FN with C) |
| Fortran array descriptor temporaries | Fortran | Strip allocas for descriptors |

The normalization deliberately does **not** strip branch instructions added by Rust's safety checks, because doing so would incorrectly inflate Rust/C similarity. This is an acknowledged limitation documented in the evaluation.

---

## 4. Module Breakdown

### `IRParser` (`src/ir_parser/`)
Loads an LLVM IR file using `llvm::parseIRFile()`, traverses all non-declaration functions, and hands them to the fingerprint modules.

### `IRNormalizer` (`src/normalizer/`)
A pre-processing pass over the raw IR that filters metadata, strips attributes, and removes external function declarations to produce a stable, language-neutral IR representation.

### `Fingerprint` (`src/fingerprint/`)
Extracts the opcode frequency map for a given function. Stores metadata: function name, source file path, total instruction count, and the frequency vector.

### `CFGBuilder` (`src/cfg_builder/`)
Traverses a function's `BasicBlock` list, extracts successor edges using `llvm::successors(&BB)`, computes cyclomatic complexity (`edges - nodes + 2`), and writes a DOT file. Invokes `dot -Tpng` via `system()` to generate the PNG.

### `DFGBuilder` (`src/dfg_builder/`)
Assigns a unique string ID to each `Instruction*`, then iterates over each instruction's `Use` list to draw `operand → user` edges. Writes to DOT and renders PNG.

### `SimilarityEngine` (`src/similarity/`)
Takes two `Fingerprint` objects and computes cosine similarity on their opcode maps, then blends with the CFG topology score. Returns a `ComparisonResult` containing the score, label, and source metadata.

### `main.cpp`
Orchestrates the full pipeline, handles CLI flag parsing (`--cfg`, `--dfg`, `--threshold`, `--verbose`), and formats the final output table.

---

## 5. Web Architecture

The web component is decoupled from the analysis engine via a REST API.

```
React Frontend (Vite, port 5173)
        │
        │  HTTP POST /api/upload
        │  HTTP POST /api/compare_testcases
        │  HTTP POST /api/generate_graph
        │  HTTP GET  /api/testcases
        │
Flask Backend (port 5000)
        │
        │  subprocess.run(['clang', '-S', '-emit-llvm', ...])
        │  subprocess.run(['./build/clone_detector', ...])
        │
clone_detector binary (C++ / LLVM)
```

The Flask backend manages the compilation step (invoking `clang`/`rustc`/`flang-new`), calls the compiled binary, parses its stdout, and returns JSON to the React frontend.

---

## 6. Alternative Approaches Considered and Rejected

### 6.1 AST-Level Clone Detection
**Rejected.** ASTs are language-specific by definition. Building AST parsers for four different languages (C, C++, Rust, Fortran) would require four separate, non-trivial parser implementations. LLVM IR provides the same semantic information without per-language parsers.

### 6.2 Token-Based Clone Detection
**Rejected.** Token streams can detect Type-1 (exact) and Type-2 (renamed variable) clones but are completely blind to Type-3 (structural) and Type-4 (semantic) clones — which are the most interesting and the entire motivation of this project.

### 6.3 Graph Edit Distance (GED)
**Considered but deferred.** GED on CFGs would give perfect structural comparison but runs in exponential time in the general case. The current cosine + topology hybrid runs in O(n) per function pair. GED is listed as a future improvement.

### 6.4 ML-Based Embedding (Code2Vec / InferCode)
**Considered for future work.** Training a model to embed functions into a latent space where similarity is a distance metric would handle optimization-level variation and type-widening clones. Requires a labelled dataset of cross-language clone pairs, which this project's benchmark corpus could eventually seed.

---

## 7. Scalability Considerations

| Aspect | Current State | Path to Scale |
|---|---|---|
| Comparison complexity | O(n²) pairwise | Locality-sensitive hashing (LSH) to prune candidates |
| Function count per file | Unbounded | Parallelise per-function analysis with OpenMP |
| Web uploads | Local temp files | S3/MinIO for file storage |
| Graph rendering | Synchronous `system()` | Async task queue (Celery + Redis) |

---

## 8. Summary

IRClone's core design principle is *"compare programs where their languages are already equal"*. By moving the comparison layer to LLVM IR, the project achieves genuine language-independence without sacrificing semantic depth. The modular pipeline ensures each stage can be replaced, improved, or extended independently — the similarity engine is not aware of the IR parser, and the web layer is not aware of the compilation details.

---

*See [IMPLEMENTATION.md](IMPLEMENTATION.md) for the LLVM API-level details of each module.*
