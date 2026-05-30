<div align="center">

# ⚡ IRClone

### Cross-Language Code Clone Detector via LLVM IR

*A compiler-based static analysis tool that detects semantically equivalent code across C, C++, Rust, and Fortran by fingerprinting their shared LLVM Intermediate Representation.*

---

[![LLVM](https://img.shields.io/badge/LLVM-18-blue?style=flat-square&logo=llvm)](https://llvm.org)
[![Language](https://img.shields.io/badge/Language-C%2B%2B17-orange?style=flat-square&logo=cplusplus)](https://isocpp.org)
[![Python](https://img.shields.io/badge/Python-3.10+-green?style=flat-square&logo=python)](https://python.org)
[![React](https://img.shields.io/badge/React-18-61dafb?style=flat-square&logo=react)](https://react.dev)
[![License](https://img.shields.io/badge/License-Academic-purple?style=flat-square)](./LICENSE)

**Demo:** `[INSERT DEMO VIDEO LINK HERE]` &nbsp;|&nbsp; **Drive:** `[INSERT GOOGLE DRIVE LINK HERE]`

</div>

---

## Table of Contents

- [Background](#background)
- [What This Tool Does](#what-this-tool-does)
- [Architecture at a Glance](#architecture-at-a-glance)
- [Deliverables Checklist](#deliverables-checklist)
- [Supported Languages](#supported-languages)
- [Tech Stack](#tech-stack)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Build](#build)
  - [Run Evaluation](#run-evaluation)
  - [Web Dashboard](#web-dashboard)
- [CLI Usage](#cli-usage)
- [Understanding the Output](#understanding-the-output)
- [Documentation](#documentation)
- [Evaluation Summary](#evaluation-summary)
- [Known Limitations](#known-limitations)
- [Author](#author)

---

## Background

Source-level clone detectors — tools like NiCad, CCFinder, and iClones — are tightly coupled to language syntax. They cannot, by design, detect that a C bubble sort and a Fortran bubble sort are the same algorithm implemented in two different languages.

**IRClone** sidesteps this problem entirely. Instead of comparing source text, it compares **LLVM Intermediate Representation (IR)** — the language-agnostic bytecode that all LLVM-frontend languages (C, C++, Rust, Fortran via `flang`) compile down to. At the IR level, a loop is a loop, an addition is an addition, and an array access is a `getelementptr` instruction regardless of whether it came from C or Fortran.

The approach: compile → normalize IR → extract structural fingerprints → compare.

---

## What This Tool Does

1. **Accepts source files** in C, C++, Rust, or Fortran as input (via CLI or web dashboard).
2. **Compiles them to LLVM IR** using `clang`, `clang++`, `rustc --emit=llvm-ir`, or `flang-new`.
3. **Normalizes the IR** by stripping compiler-injected boilerplate, debug metadata, and language-specific runtime calls.
4. **Extracts CFG and DFG fingerprints** per function — opcode frequency vectors, basic-block topology, cyclomatic complexity, and instruction-level data dependencies.
5. **Computes pairwise similarity scores** using cosine similarity on opcode frequency vectors, further weighted by CFG structural similarity.
6. **Reports clone pairs** — labelled as *Strong Clone* (≥ 70%), *Partial Similarity* (40–69%), or *Different* (< 40%).
7. **Generates visual graphs** (Graphviz PNG) of CFG and DFG for any selected function.

---

## Architecture at a Glance

```
  Source Files (C / C++ / Rust / Fortran)
           │
           ▼
  ┌─────────────────────┐
  │  Compiler Frontend  │  clang / clang++ / rustc / flang-new
  └─────────────────────┘
           │  LLVM IR (.ll)
           ▼
  ┌─────────────────────┐
  │   IR Normalizer     │  Strip metadata, debug info, lang-specific calls
  └─────────────────────┘
           │  Normalized IR
           ▼
  ┌──────────────────────────────────────┐
  │          Fingerprint Engine          │
  │  ┌───────────────┐ ┌──────────────┐ │
  │  │  CFG Builder  │ │  DFG Builder │ │
  │  └───────────────┘ └──────────────┘ │
  │       (basic blocks, edges)  (use-def chains) │
  └──────────────────────────────────────┘
           │  Fingerprints
           ▼
  ┌─────────────────────┐
  │  Similarity Engine  │  Cosine similarity + CFG topology weight
  └─────────────────────┘
           │  Scores
           ▼
  ┌────────────────────────────────────┐
  │       Output Layer                 │
  │  CLI report · CSV · CFG/DFG PNGs  │
  │  Web Dashboard (React + Flask)     │
  └────────────────────────────────────┘
```

---

## Deliverables Checklist

| # | Deliverable | Status |
|---|---|---|
| 1 | IR normalization pipeline stripping language-specific artifacts | ✅ Complete |
| 2 | CFG/DFG fingerprinting engine with similarity scoring | ✅ Complete |
| 3 | CLI tool reporting clone pairs with scores and source mappings | ✅ Complete |
| 4 | Evaluation on a curated corpus of equivalent implementations across 3+ languages | ✅ Complete |
| 5 | False-positive/negative analysis documenting accuracy | ✅ Complete |
| + | Premium Web Dashboard with real-time graph visualization | ✅ Bonus |

---

## Supported Languages

| Language | Compiler Used | IR Generation Command |
|---|---|---|
| **C** | `clang` | `clang -S -emit-llvm -O0 file.c -o file.ll` |
| **C++** | `clang++` | `clang++ -S -emit-llvm -O0 file.cpp -o file.ll` |
| **Rust** | `rustc` | `rustc --emit=llvm-ir --edition 2021 file.rs` |
| **Fortran** | `flang-new` / `gfortran` | `flang-new -S -emit-llvm file.f90 -o file.ll` |

---

## Tech Stack

| Layer | Technology |
|---|---|
| **Analysis Engine** | C++17, LLVM 18 API |
| **Build System** | CMake 3.20+ |
| **Graph Generation** | Graphviz (`dot`) |
| **Web Backend** | Python 3.10+, Flask, Flask-CORS |
| **Web Frontend** | React 18, Vite, Vanilla CSS |
| **Analytics** | Pandas, Matplotlib, Seaborn |

---

## Project Structure

```
clone-detector/
│
├── src/                          # C++ source files
│   ├── main.cpp                  # Entry point, pipeline orchestration
│   ├── ir_parser/                # LLVM IR loading and function traversal
│   ├── normalizer/               # IR normalization pass
│   ├── fingerprint/              # Opcode frequency fingerprint extraction
│   ├── cfg_builder/              # CFG extraction and DOT generation
│   ├── dfg_builder/              # DFG extraction (use-def chains)
│   └── similarity/               # Cosine similarity scoring engine
│
├── include/                      # Corresponding header files
│
├── testcases/
│   └── benchmark/                # Curated corpus of equivalent implementations
│       ├── sorting/              # Bubble Sort, Merge Sort, Selection Sort
│       ├── searching/            # Binary Search, Linear Search
│       ├── math/                 # Factorial, Fibonacci
│       └── recursion/            # Recursive algorithms
│
├── web/
│   ├── backend/                  # Flask API server
│   │   └── app.py
│   └── frontend/                 # React web UI
│       └── src/
│
├── results/                      # Generated similarity reports and graphs
├── scripts/                      # Helper shell scripts
│
├── build.sh                      # ← Build the C++ engine
├── run.sh                        # ← Run the full evaluation pipeline
├── evaluate.py                   # Python evaluation runner
├── visualize.py                  # Heatmap and bar chart generation
│
├── README.md                     # This file
├── DESIGN.md                     # Architecture and design decisions
├── IMPLEMENTATION.md             # LLVM implementation deep-dive
└── EVALUATION.md                 # Accuracy metrics and analysis
```

---

## Getting Started

### Prerequisites

Tested on **Ubuntu 22.04 LTS** (or WSL2 on Windows 11).

```bash
# 1. LLVM toolchain (required)
sudo apt update
sudo apt install -y llvm-18 clang-18 cmake build-essential

# 2. Graphviz (required for CFG/DFG graphs)
sudo apt install -y graphviz

# 3. Fortran compiler (optional, for Fortran support)
sudo apt install -y flang

# 4. Python dependencies (required for analytics and web backend)
pip install flask flask-cors pandas matplotlib seaborn

# 5. Node.js + npm (required for web frontend)
sudo apt install -y nodejs npm
```

### Build

```bash
# Clone the repository
git clone https://github.com/yashwantrathi/clone-detector.git
cd clone-detector

# Run the provided build script
./build.sh
```

This will:
1. Create the `build/` directory
2. Run `cmake ..` to configure the LLVM-linked C++ project
3. Run `make` to compile the `clone_detector` binary

The compiled binary will be at `build/clone_detector`.

### Run Evaluation

Execute the full automated evaluation pipeline against the curated benchmark corpus:

```bash
./run.sh
```

This runs **5+ curated test cases** (C vs C++, C vs Fortran, C vs Rust, and false-positive tests) and generates:
- `results/results.csv` — pairwise similarity scores in tabular format
- `results/similarity_heatmap.png` — visual similarity heatmap
- `results/similarity_bar.png` — bar chart of clone scores
- `results/cfg_graphs/` — Control Flow Graph PNGs per function
- `results/dfg_graphs/` — Data Flow Graph PNGs per function

### Web Dashboard

Start the Flask backend and React frontend in separate terminals:

```bash
# Terminal 1 — Backend API
cd web/backend
source venv/bin/activate   # if using virtualenv
python3 app.py

# Terminal 2 — Frontend
cd web/frontend
npm install
npm run dev
```

Open `http://localhost:5173` in your browser. The landing page will guide you in.

---

## CLI Usage

```bash
# Compare two source files directly
./build/clone_detector path/to/file1.c path/to/file2.cpp

# Generate CFG graph for a specific function
./build/clone_detector path/to/file.c --cfg functionName

# Generate DFG graph for a specific function
./build/clone_detector path/to/file.c --dfg functionName

# Set custom similarity threshold (default: 70%)
./build/clone_detector file1.c file2.cpp --threshold 80

# Verbose output
./build/clone_detector file1.c file2.cpp --verbose
```

---

## Understanding the Output

```
[COMPARING] bubble_sort.c :: bubbleSort  vs  bubble_sort.cpp :: bubbleSort
  Opcode Similarity  : 91.3%
  CFG Nodes         : 7  vs  7
  CFG Edges         : 9  vs  9
  Cyclomatic Complexity : 3  vs  3
  Overall Score     : 94.5%
  → Result          : STRONG CLONE DETECTED
```

| Score Range | Classification | Meaning |
|---|---|---|
| ≥ 70% | **Strong Clone** | Semantically equivalent across languages |
| 40–69% | **Partial Similarity** | Same algorithmic family, different structure |
| < 40% | **Different** | No significant structural overlap |

---

## Documentation

| File | Contents |
|---|---|
| [DESIGN.md](DESIGN.md) | System architecture, design goals, alternatives considered |
| [IMPLEMENTATION.md](IMPLEMENTATION.md) | LLVM API usage, normalization passes, fingerprint algorithms |
| [EVALUATION.md](EVALUATION.md) | Benchmark corpus results, accuracy metrics, FP/FN analysis |

---

## Evaluation Summary

| Language Pair | Algorithm | Similarity | Result |
|---|---|---|---|
| C vs C++ | Bubble Sort | ~94% | ✅ Strong Clone |
| C vs Fortran | Bubble Sort | ~78% | ✅ Strong Clone |
| C vs C++ | Binary Search | ~100% | ✅ Strong Clone |
| C vs Rust | Bubble Sort | ~58% | ⚠️ Partial (FN case) |
| Bubble vs Merge Sort (C) | — | ~52% | ✅ Correctly Rejected |

See [EVALUATION.md](EVALUATION.md) for the full analysis.

---

## Known Limitations

- **Rust's safety IR:** Rust injects bounds-check panic blocks and trait-dispatch IR that significantly dilutes opcode fingerprints when compared against plain C/C++. A language-aware post-normalization pass for Rust is the highest-priority future work.
- **Optimization level sensitivity:** Running `clang -O2` vs `clang -O0` produces very different IR for the same source. All evaluations are performed at `-O0` for reproducibility.
- **No cross-function analysis:** Clone detection is scoped to individual function boundaries. Inlined functions or macro-expanded code may not be correctly attributed.

---

## Author

**Yashwanth Rathi**
Compiler Design Lab Project — Assignment 23
*Cross-Language Code Clone Detector via LLVM IR*
