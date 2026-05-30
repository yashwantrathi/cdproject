import React from 'react'

function getLangFromFile(filename) {
  if (!filename) return 'unknown'
  const f = filename.split('/').pop()
  if (f.endsWith('.c')) return 'c'
  if (f.endsWith('.cpp') || f.endsWith('.cxx') || f.endsWith('.cc')) return 'cpp'
  if (f.endsWith('.rs')) return 'rust'
  if (f.endsWith('.f90') || f.endsWith('.f')) return 'fortran'
  return 'unknown'
}

const LANG_LABELS = { c: 'C', cpp: 'C++', rust: 'Rust', fortran: 'Fortran', unknown: '?' }

function getShortName(filename) {
  if (!filename) return ''
  const base = filename.split('/').pop().split('\\').pop()
  const parts = base.split('.')
  if (parts.length === 1) return base.length > 16 ? base.slice(0, 14) + '…' : base
  const ext = parts.pop()
  const name = parts.join('.')
  return name.length > 14 ? name.slice(0, 12) + '….' + ext : base
}

export default function SimilarityMatrix({ comparisons, files }) {
  if (!comparisons || !files || files.length < 2) return null

  const n = files.length
  const matrix = Array.from({ length: n }, () => Array(n).fill(null))

  for (let i = 0; i < n; i++) matrix[i][i] = 100.0

  if (Array.isArray(comparisons)) {
    for (const cmp of comparisons) {
      const i = files.indexOf(cmp.file1 || cmp.files?.[0])
      const j = files.indexOf(cmp.file2 || cmp.files?.[1])
      const sim = cmp.similarity ?? cmp.score ?? 0
      if (i >= 0 && j >= 0) {
        matrix[i][j] = Math.max(matrix[i][j] || 0, sim)
        matrix[j][i] = Math.max(matrix[j][i] || 0, sim)
      }
    }
  }

  const getCellClass = (val) => {
    if (val === null)  return 'cell-empty'
    if (val >= 99)     return 'cell-self'
    if (val >= 70)     return 'cell-high'
    if (val >= 40)     return 'cell-medium'
    return 'cell-low'
  }

  return (
    <div className="glass-card" style={{ marginBottom: 24, overflowX: 'auto' }}>
      <div className="card-header">
        <div className="card-icon emerald">📊</div>
        <div>
          <div className="card-title">Similarity Matrix</div>
          <div className="card-subtitle">Pairwise function-level clone similarity scores (%)</div>
        </div>
      </div>

      <div className="matrix-wrapper">
        <table className="matrix-table">
          <thead>
            <tr>
              <th />
              {files.map((f, i) => (
                <th key={i}>
                  <span className={`lang-tag ${getLangFromFile(f)}`} style={{ fontSize: '0.62rem' }}>
                    {getShortName(f)}
                  </span>
                </th>
              ))}
            </tr>
          </thead>
          <tbody>
            {files.map((rowFile, i) => (
              <tr key={i}>
                <td style={{ padding: '8px 12px', textAlign: 'right' }}>
                  <span className={`lang-tag ${getLangFromFile(rowFile)}`} style={{ fontSize: '0.62rem' }}>
                    {getShortName(rowFile)}
                  </span>
                </td>
                {files.map((_, j) => {
                  const val = matrix[i][j]
                  return (
                    <td key={j} className={getCellClass(val)}>
                      {val !== null ? `${val.toFixed(1)}%` : '—'}
                    </td>
                  )
                })}
              </tr>
            ))}
          </tbody>
        </table>
      </div>

      <div className="matrix-legend">
        {[
          { cls: 'cell-self',   dot: 'rgba(6,182,212,0.4)',   label: 'Self (100%)' },
          { cls: 'cell-high',   dot: 'rgba(16,185,129,0.4)',  label: 'Strong Clone (≥70%)' },
          { cls: 'cell-medium', dot: 'rgba(245,158,11,0.4)',  label: 'Partial (40–69%)' },
          { cls: 'cell-low',    dot: 'rgba(244,63,94,0.35)',  label: 'Different (<40%)' },
        ].map(({ dot, label }) => (
          <div className="legend-item" key={label}>
            <div className="legend-dot" style={{ background: dot }} />
            {label}
          </div>
        ))}
      </div>
    </div>
  )
}
