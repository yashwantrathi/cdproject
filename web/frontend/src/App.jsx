import React, { useState, useEffect } from 'react'
import FileUpload from './components/FileUpload.jsx'
import TestcaseLibrary from './components/TestcaseLibrary.jsx'
import SimilarityMatrix from './components/SimilarityMatrix.jsx'
import CodePreview from './components/CodePreview.jsx'
import CloneDetails from './components/CloneDetails.jsx'
import GraphViewer from './components/GraphViewer.jsx'

const API_URL = 'http://localhost:5000/api'

/* ── Dark / Light mode toggle button ─────────────────────────── */
function ThemeToggle({ isDark, onToggle }) {
  return (
    <button
      onClick={onToggle}
      title={isDark ? 'Switch to Light Mode' : 'Switch to Dark Mode'}
      style={{
        display: 'flex', alignItems: 'center', gap: 6,
        padding: '7px 14px', borderRadius: 'var(--r-sm)',
        border: '1px solid var(--border-glass)',
        background: 'var(--bg-glass)', color: 'var(--text-secondary)',
        fontSize: '0.82rem', fontWeight: 600,
        fontFamily: 'Inter, sans-serif',
        cursor: 'pointer', transition: 'all 0.2s ease',
      }}
      onMouseOver={e => { e.currentTarget.style.borderColor = 'var(--border-hover)'; e.currentTarget.style.color = 'var(--text-primary)'; }}
      onMouseOut={e  => { e.currentTarget.style.borderColor = 'var(--border-glass)'; e.currentTarget.style.color = 'var(--text-secondary)'; }}
    >
      {isDark ? '☀️ Light' : '🌙 Dark'}
    </button>
  )
}

/* ── Landing Page ──────────────────────────────────────────── */
function LandingPage({ onEnter, isDark, onToggleTheme }) {
  const [scrolled, setScrolled] = useState(false)

  useEffect(() => {
    const handler = () => setScrolled(window.scrollY > 20)
    window.addEventListener('scroll', handler)
    return () => window.removeEventListener('scroll', handler)
  }, [])

  return (
    <div className="landing-page">
      {/* Nav */}
      <nav className={`landing-nav ${scrolled ? 'scrolled' : ''}`}>
        <div className="nav-inner">
          <div className="nav-logo">
            <div className="nav-logo-mark">⚡</div>
            <span className="nav-logo-text">IR<span>Clone</span></span>
          </div>
          <div className="nav-actions">
            <ThemeToggle isDark={isDark} onToggle={onToggleTheme} />
            <button className="btn btn-primary btn-sm" onClick={onEnter}>
              Launch App →
            </button>
          </div>
        </div>
      </nav>

      {/* Hero */}
      <section className="hero">
        <div className="hero-bg-grid" />
        <div className="hero-content">
          <div className="hero-badge">
            <div className="hero-badge-dot" />
            Powered by LLVM IR — Cross-Language Analysis
          </div>
          <h1 className="hero-title">
            Detect Code Clones<br />
            <span className="grad">Across Any Language</span>
          </h1>
          <p className="hero-description">
            A production-grade clone detection engine that compiles C, C++, Rust, and Fortran
            to LLVM IR, extracts CFG/DFG fingerprints, and finds semantically equivalent
            functions across language boundaries.
          </p>
          <div className="hero-cta-group">
            <button className="btn btn-hero" onClick={onEnter}>
              🚀 Start Analyzing
            </button>
            <button className="btn btn-hero-outline" onClick={onEnter}>
              📚 Explore Testcases
            </button>
          </div>
          <div className="hero-stats">
            <div className="hero-stat">
              <div className="hero-stat-value">4</div>
              <div className="hero-stat-label">Languages</div>
            </div>
            <div className="hero-divider" />
            <div className="hero-stat">
              <div className="hero-stat-value">CFG</div>
              <div className="hero-stat-label">Flow Graphs</div>
            </div>
            <div className="hero-divider" />
            <div className="hero-stat">
              <div className="hero-stat-value">DFG</div>
              <div className="hero-stat-label">Data Graphs</div>
            </div>
            <div className="hero-divider" />
            <div className="hero-stat">
              <div className="hero-stat-value">IR</div>
              <div className="hero-stat-label">Normalised</div>
            </div>
          </div>
        </div>
      </section>

      {/* Features */}
      <section className="features-section">
        <div className="section-label">✦ Capabilities</div>
        <h2 className="section-title">Everything You Need for<br />Cross-Language Analysis</h2>
        <p className="section-desc">
          From IR normalization to CFG fingerprinting — a complete pipeline for detecting
          semantically equivalent code regardless of the source language.
        </p>
        <div className="features-grid">
          {[
            { icon: '🔄', color: 'cyan',    title: 'IR Normalization',         desc: 'Strips language-specific artifacts and compiler boilerplate from LLVM IR, creating a clean, comparable representation.' },
            { icon: '🗺️', color: 'purple',  title: 'CFG Fingerprinting',       desc: 'Builds Control Flow Graphs from LLVM BasicBlocks, computing cyclomatic complexity and structural similarity scores.' },
            { icon: '🌐', color: 'emerald', title: 'DFG Analysis',             desc: 'Extracts Data Flow Graphs showing instruction-level dependencies, enabling deep semantic understanding.' },
            { icon: '⚡', color: 'amber',   title: 'Multi-File Comparison',    desc: 'Compare up to 4 files simultaneously. Results visualized as a pairwise similarity matrix with color-coded scores.' },
            { icon: '📊', color: 'blue',    title: 'Similarity Scoring',       desc: 'Opcode-frequency based cosine similarity with tunable thresholds. Categorizes clones as Strong, Partial, or Different.' },
            { icon: '🖼️', color: 'rose',    title: 'Visual Graph Export',      desc: 'Generate and download publication-quality PNG graphs of CFG/DFG structures with a single click.' },
          ].map((f, i) => (
            <div className="feature-card" key={i} style={{ animationDelay: `${i * 0.08}s` }}>
              <div className={`feature-icon-wrap ${f.color}`}>{f.icon}</div>
              <div className="feature-title">{f.title}</div>
              <div className="feature-desc">{f.desc}</div>
            </div>
          ))}
        </div>
      </section>

      {/* Language support */}
      <section className="lang-section">
        <div className="section-label">✦ Language Support</div>
        <h2 className="section-title">Works Across All Major Systems Languages</h2>
        <div className="lang-pills">
          <div className="lang-pill c">     <span>●</span> C          </div>
          <div className="lang-pill cpp">   <span>●</span> C++        </div>
          <div className="lang-pill rust">  <span>●</span> Rust       </div>
          <div className="lang-pill fortran"><span>●</span> Fortran   </div>
        </div>
      </section>

      {/* CTA */}
      <section className="cta-section">
        <div className="cta-card">
          <div className="section-label" style={{ justifyContent: 'center' }}>✦ Get Started</div>
          <h2 className="section-title" style={{ marginTop: 12 }}>Ready to Detect Clones?</h2>
          <p className="section-desc" style={{ margin: '16px auto 40px', textAlign: 'center' }}>
            Upload your source files or explore the curated benchmark library — results in seconds.
          </p>
          <button className="btn btn-hero" onClick={onEnter}>
            Launch IRClone →
          </button>
        </div>
      </section>

      <footer className="landing-footer">
        IRClone — Cross-Language LLVM Clone Detector &nbsp;·&nbsp; Compiler Design Lab Project &nbsp;·&nbsp; 2026
      </footer>
    </div>
  )
}

/* ── App Shell ─────────────────────────────────────────────── */
export default function App() {
  const [view, setView] = useState('landing') // 'landing' | 'app'
  const [activeTab, setActiveTab] = useState('testcases')
  const [results, setResults] = useState(null)
  const [previewFiles, setPreviewFiles] = useState([])
  const [loading, setLoading] = useState(false)
  const [error, setError] = useState(null)
  const [isDark, setIsDark] = useState(true)

  const toggleTheme = () => {
    setIsDark(prev => {
      const next = !prev
      document.documentElement.classList.toggle('light-mode', !next)
      return next
    })
  }

  const [graphState, setGraphState] = useState({
    isOpen: false, loading: false, error: null,
    imageUrl: null, dotText: null, metrics: null, title: ''
  })

  const handleResults = (data) => { setResults(data); setError(null) }
  const handleError   = (msg)  => { setError(msg); setResults(null) }

  const handleGenerateGraph = async (file, funcName, type) => {
    setGraphState({ isOpen: true, loading: true, error: null, imageUrl: null, dotText: null, metrics: null, title: `${type.toUpperCase()}: ${funcName}` })
    try {
      const response = await fetch(`${API_URL}/generate_graph`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ file, function: funcName, type })
      })
      const data = await response.json()
      if (!response.ok) throw new Error(data.error || `Failed to generate ${type.toUpperCase()}`)
      setGraphState(prev => ({
        ...prev, loading: false,
        imageUrl: `data:image/png;base64,${data.image}`,
        dotText: data.dot_text,
        metrics: data.metrics
      }))
    } catch (err) {
      setGraphState(prev => ({ ...prev, loading: false, error: err.message }))
    }
  }

  const closeGraphModal = () => setGraphState(prev => ({ ...prev, isOpen: false }))

  if (view === 'landing') {
    return <LandingPage onEnter={() => setView('app')} isDark={isDark} onToggleTheme={toggleTheme} />
  }

  return (
    <div className="app-shell">
      {/* Top Nav */}
      <nav className="app-nav">
        <div className="nav-bar">
          <div className="nav-brand" onClick={() => setView('landing')}>
            <div className="brand-icon">⚡</div>
            <div>
              <div className="brand-name">IR<span>Clone</span></div>
            </div>
            <div className="brand-tag">Beta</div>
          </div>

          <div className="nav-tabs">
            <button
              id="tab-testcases"
              className={`nav-tab ${activeTab === 'testcases' ? 'active' : ''}`}
              onClick={() => setActiveTab('testcases')}
            >
              <span className="nav-tab-icon">📚</span>
              Testcase Library
            </button>
            <button
              id="tab-upload"
              className={`nav-tab ${activeTab === 'upload' ? 'active' : ''}`}
              onClick={() => setActiveTab('upload')}
            >
              <span className="nav-tab-icon">📤</span>
              Upload Files
            </button>
          </div>

          <div className="nav-right">
            <ThemeToggle isDark={isDark} onToggle={toggleTheme} />
            <button className="nav-back-btn" onClick={() => setView('landing')}>
              ← Home
            </button>
          </div>
        </div>
      </nav>

      <main className="app-main">
        {/* Tab Content */}
        {activeTab === 'testcases' && (
          <TestcaseLibrary
            onResults={handleResults}
            onError={handleError}
            onPreview={setPreviewFiles}
            loading={loading}
            setLoading={setLoading}
          />
        )}

        {activeTab === 'upload' && (
          <FileUpload
            onResults={handleResults}
            onError={handleError}
            loading={loading}
            setLoading={setLoading}
          />
        )}

        {/* Error */}
        {error && (
          <div className="error-banner">
            <div className="error-icon-wrap">⚠️</div>
            <span>{error}</span>
          </div>
        )}

        {/* Loading */}
        {loading && (
          <div className="loading-screen" style={{ marginTop: 48 }}>
            <div className="spinner" />
            <div className="loading-title">Analyzing Code</div>
            <div className="loading-steps">
              <div className="loading-step active">⚙️ Compiling to LLVM IR…</div>
              <div className="loading-step active">🔬 Extracting fingerprints…</div>
              <div className="loading-step active">📊 Computing similarity scores…</div>
            </div>
          </div>
        )}

        {/* Code Preview */}
        {previewFiles.length > 0 && !loading && (
          <CodePreview files={previewFiles} />
        )}

        {/* Results */}
        {results && !loading && (
          <div className="results-section">
            <div className="results-header">
              <div>
                <h2 className="results-title">Similarity Analysis Results</h2>
                <p className="results-subtitle">
                  {results.files_processed?.length} files compared · {results.comparisons?.length} function pairs analyzed
                </p>
              </div>
              <button
                className="btn btn-outline btn-sm"
                onClick={() => { setResults(null); setPreviewFiles([]) }}
              >
                ✕ Clear Results
              </button>
            </div>
            <SimilarityMatrix
              comparisons={results.comparisons}
              files={results.files_processed}
            />
            <CloneDetails
              comparisons={results.comparisons}
              onGenerateGraph={handleGenerateGraph}
            />
          </div>
        )}
      </main>

      {/* Graph Viewer */}
      <GraphViewer
        isOpen={graphState.isOpen}
        onClose={closeGraphModal}
        imageUrl={graphState.imageUrl}
        title={graphState.title}
        loading={graphState.loading}
        error={graphState.error}
        dotText={graphState.dotText}
        metrics={graphState.metrics}
      />
    </div>
  )
}
