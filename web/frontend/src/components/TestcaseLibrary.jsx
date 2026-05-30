import React, { useState, useEffect } from 'react'
import axios from 'axios'

const API = 'http://localhost:5000'

const CATEGORIES = [
  { id: 'sorting',   label: 'Sorting',   emoji: '🔄', desc: 'Comparison-based sorting algorithms' },
  { id: 'searching', label: 'Searching', emoji: '🔍', desc: 'Search algorithms on arrays' },
  { id: 'math',      label: 'Math',      emoji: '🧮', desc: 'Mathematical computations' },
  { id: 'recursion', label: 'Recursion', emoji: '🔁', desc: 'Recursive algorithms' },
]

function getLangFromFile(filename) {
  if (filename.endsWith('.c')) return 'c'
  if (filename.endsWith('.cpp') || filename.endsWith('.cxx') || filename.endsWith('.cc')) return 'cpp'
  if (filename.endsWith('.rs')) return 'rust'
  if (filename.endsWith('.f90') || filename.endsWith('.f')) return 'fortran'
  return 'unknown'
}

const LANG_LABELS = { c: 'C', cpp: 'C++', rust: 'Rust', fortran: 'Fortran', unknown: '?' }

function getAlgorithmName(filename) {
  const base = filename.split('/').pop().replace(/\.[^.]+$/, '')
  return base.split('_').map(w => w.charAt(0).toUpperCase() + w.slice(1)).join(' ')
}

function getCategoryFromPath(filepath) {
  for (const cat of CATEGORIES) {
    if (filepath.includes(`benchmark/${cat.id}/`) || filepath.includes(`${cat.id}/`)) return cat.id
  }
  return null
}

export default function TestcaseLibrary({ onResults, onError, onPreview, loading, setLoading }) {
  const [testcases, setTestcases]   = useState([])
  const [activeCategory, setActiveCategory] = useState('sorting')
  const [selected, setSelected]     = useState([])
  const [algorithms, setAlgorithms] = useState({})
  const [fetchError, setFetchError] = useState(null)

  useEffect(() => {
    axios.get(`${API}/api/testcases`)
      .then(res => {
        const files = res.data.testcases || []
        setTestcases(files)
        setFetchError(null)
        const grouped = {}
        for (const f of files) {
          const cat = getCategoryFromPath(f)
          if (!cat) continue
          const algoName = getAlgorithmName(f)
          const key = `${cat}::${algoName}`
          if (!grouped[key]) grouped[key] = { name: algoName, category: cat, files: [] }
          grouped[key].files.push(f)
        }
        setAlgorithms(grouped)
      })
      .catch(() => setFetchError('Could not connect to backend. Make sure the Flask server is running on port 5000.'))
  }, [])

  const currentAlgos = Object.values(algorithms)
    .filter(a => a.category === activeCategory)
    .sort((a, b) => a.name.localeCompare(b.name))

  const categoryCounts = {}
  for (const cat of CATEGORIES) {
    categoryCounts[cat.id] = Object.values(algorithms).filter(a => a.category === cat.id).length
  }

  const toggleFile = (filepath) => {
    setSelected(prev => {
      if (prev.includes(filepath)) return prev.filter(f => f !== filepath)
      if (prev.length >= 4) return prev
      return [...prev, filepath]
    })
  }

  const selectAllForAlgo = (algo) => setSelected(algo.files.slice(0, 4))

  const handleCompare = async () => {
    if (selected.length < 2) { onError('Please select at least 2 files to compare.'); return }
    setLoading(true)
    onPreview([])
    try {
      const previewPromises = selected.map(f =>
        axios.get(`${API}/api/testcase_content`, { params: { file: f } })
          .then(res => ({ filename: f.split('/').pop(), path: f, content: res.data.content, lang: getLangFromFile(f) }))
          .catch(() => ({ filename: f.split('/').pop(), path: f, content: '// Could not load file', lang: getLangFromFile(f) }))
      )
      const previews = await Promise.all(previewPromises)
      onPreview(previews)
      const res = await axios.post(`${API}/api/compare_testcases`, { files: selected })
      onResults(res.data)
    } catch (err) {
      onError(err.response?.data?.error || 'Comparison failed. Check that the backend is running.')
    } finally {
      setLoading(false)
    }
  }

  if (fetchError) {
    return (
      <div className="glass-card">
        <div className="error-banner" style={{ marginTop: 0 }}>
          <div className="error-icon-wrap">🔌</div>
          <div>
            <div style={{ fontWeight: 600, marginBottom: 4 }}>Backend Disconnected</div>
            <div style={{ fontSize: '0.82rem' }}>{fetchError}</div>
          </div>
        </div>
      </div>
    )
  }

  return (
    <div>
      <div className="library-layout">
        {/* Sidebar */}
        <div className="sidebar-card">
          <div style={{ marginBottom: 4 }}>
            <div className="card-title" style={{ fontSize: '0.82rem', fontWeight: 700, textTransform: 'uppercase', letterSpacing: '0.08em', color: 'var(--text-muted)' }}>
              Categories
            </div>
          </div>
          <div className="cat-list">
            {CATEGORIES.map(cat => (
              <button
                key={cat.id}
                id={`cat-${cat.id}`}
                className={`cat-btn ${activeCategory === cat.id ? 'active' : ''}`}
                onClick={() => setActiveCategory(cat.id)}
              >
                <span className="cat-btn-emoji">{cat.emoji}</span>
                <span className="cat-btn-label">{cat.label}</span>
                <span className="cat-count">{categoryCounts[cat.id] || 0}</span>
              </button>
            ))}
          </div>

          <div style={{ marginTop: 24, paddingTop: 20, borderTop: '1px solid var(--border-subtle)' }}>
            <div style={{ fontSize: '0.72rem', fontWeight: 600, textTransform: 'uppercase', letterSpacing: '0.08em', color: 'var(--text-muted)', marginBottom: 10 }}>
              How to use
            </div>
            {['Select 2–4 files', 'Mix languages freely', 'Click Compare'].map((tip, i) => (
              <div key={i} style={{ display: 'flex', alignItems: 'center', gap: 8, marginBottom: 8, fontSize: '0.78rem', color: 'var(--text-secondary)' }}>
                <span style={{ width: 20, height: 20, borderRadius: '50%', background: 'var(--cyan-subtle)', border: '1px solid rgba(6,182,212,0.2)', display: 'flex', alignItems: 'center', justifyContent: 'center', fontSize: '0.65rem', fontWeight: 700, color: 'var(--cyan)', flexShrink: 0 }}>{i + 1}</span>
                {tip}
              </div>
            ))}
          </div>
        </div>

        {/* Main */}
        <div>
          <div style={{ marginBottom: 20, display: 'flex', alignItems: 'center', justifyContent: 'space-between', flexWrap: 'wrap', gap: 12 }}>
            <div>
              <h2 style={{ fontSize: '1.15rem', fontWeight: 700, color: 'var(--text-primary)', display: 'flex', alignItems: 'center', gap: 8 }}>
                <span>{CATEGORIES.find(c => c.id === activeCategory)?.emoji}</span>
                {CATEGORIES.find(c => c.id === activeCategory)?.label} Algorithms
              </h2>
              <p style={{ fontSize: '0.78rem', color: 'var(--text-muted)', marginTop: 3 }}>
                {CATEGORIES.find(c => c.id === activeCategory)?.desc}
              </p>
            </div>
            {selected.length > 0 && (
              <div style={{ fontSize: '0.78rem', color: 'var(--cyan)', fontWeight: 600, background: 'var(--cyan-subtle)', padding: '5px 12px', borderRadius: '999px', border: '1px solid rgba(6,182,212,0.2)' }}>
                {selected.length} / 4 selected
              </div>
            )}
          </div>

          {currentAlgos.length === 0 ? (
            <div className="glass-card">
              <div className="empty-state">
                <div className="empty-icon">📭</div>
                <div className="empty-title">No algorithms found</div>
                <div className="empty-desc">This category is empty or the backend is not connected</div>
              </div>
            </div>
          ) : (
            <div className="algo-grid">
              {currentAlgos.map((algo, idx) => (
                <div className="algo-card" key={algo.name} style={{ animationDelay: `${idx * 0.05}s` }}>
                  <div className="algo-header">
                    <div className="algo-name">{algo.name}</div>
                    <button
                      className="quick-select-btn"
                      onClick={() => selectAllForAlgo(algo)}
                    >
                      ⚡ All
                    </button>
                  </div>

                  <div className="algo-langs-row">
                    {algo.files.map(f => (
                      <span key={f} className={`lang-tag ${getLangFromFile(f)}`}>
                        {LANG_LABELS[getLangFromFile(f)]}
                      </span>
                    ))}
                  </div>

                  <div style={{ display: 'flex', flexDirection: 'column', gap: 3 }}>
                    {algo.files.sort().map(f => {
                      const lang = getLangFromFile(f)
                      const isSelected = selected.includes(f)
                      return (
                        <div
                          key={f}
                          className={`file-item ${isSelected ? 'selected' : ''}`}
                          onClick={() => toggleFile(f)}
                        >
                          <div className="file-checkbox">
                            {isSelected && <span className="file-check-icon">✓</span>}
                          </div>
                          <span className={`lang-tag ${lang}`} style={{ fontSize: '0.65rem', padding: '2px 8px' }}>
                            {LANG_LABELS[lang]}
                          </span>
                          <span className="file-name-text">{f.split('/').pop()}</span>
                        </div>
                      )
                    })}
                  </div>
                </div>
              ))}
            </div>
          )}
        </div>
      </div>

      {/* Selection bar */}
      {selected.length > 0 && (
        <div className="selection-bar">
          <div className="sel-info">
            <div className="sel-count">✦ {selected.length} of 4 selected</div>
            <div className="sel-files">
              {selected.map(f => (
                <span key={f} className="sel-file-tag">
                  <span className={`lang-tag ${getLangFromFile(f)}`} style={{ fontSize: '0.62rem', padding: '1px 6px' }}>
                    {LANG_LABELS[getLangFromFile(f)]}
                  </span>
                  {f.split('/').pop()}
                  <button className="sel-file-remove" onClick={e => { e.stopPropagation(); toggleFile(f) }}>×</button>
                </span>
              ))}
            </div>
          </div>
          <div className="sel-divider" />
          <div className="sel-actions">
            <button className="btn btn-outline btn-sm" onClick={() => setSelected([])}>Clear</button>
            <button
              id="btn-compare"
              className="btn btn-primary"
              onClick={handleCompare}
              disabled={selected.length < 2 || loading}
            >
              {loading
                ? <><div className="spinner spinner-sm" /> Analyzing…</>
                : `🔬 Compare ${selected.length} Files`
              }
            </button>
          </div>
        </div>
      )}
    </div>
  )
}
