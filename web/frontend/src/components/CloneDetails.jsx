import React from 'react'

export default function CloneDetails({ comparisons, onGenerateGraph }) {
  if (!comparisons || comparisons.length === 0) return null

  const significantClones = comparisons.filter(c => c.similarity >= 40.0)
  
  if (significantClones.length === 0) return null

  return (
    <div className="glass-card" style={{ marginTop: 24 }}>
      <div className="card-header">
        <div className="card-icon blue">🔍</div>
        <div>
          <div className="card-title">Clone Details & Analysis</div>
          <div className="card-subtitle">Generate Control Flow and Data Flow graphs for analyzed functions</div>
        </div>
      </div>

      <div className="clone-list">
        {significantClones.map((cmp, idx) => (
          <div key={idx} className="clone-row" style={{ animationDelay: `${idx * 0.05}s` }}>
            
            <div className="clone-info">
              <div className="clone-files">
                <span className="clone-file-func">{cmp.file1} <span className="clone-func-name">::{cmp.func1}</span></span>
                <span className="vs-pill">VS</span>
                <span className="clone-file-func">{cmp.file2} <span className="clone-func-name">::{cmp.func2}</span></span>
              </div>
              
              <div className="clone-meta">
                <span className={`sim-pill ${cmp.similarity >= 70 ? 'high' : 'medium'}`}>
                  {cmp.similarity.toFixed(1)}% Match
                </span>
                <span className="result-tag">— {cmp.result}</span>
              </div>
            </div>
            
            <div className="clone-actions">
              <div className="action-row">
                <span className="action-row-label">File 1:</span>
                <button className="graph-btn cfg" onClick={() => onGenerateGraph(cmp.file1, cmp.func1, 'cfg')}>
                  <span style={{ fontSize: '0.85rem' }}>🗺️</span> CFG
                </button>
                <button className="graph-btn dfg" onClick={() => onGenerateGraph(cmp.file1, cmp.func1, 'dfg')}>
                  <span style={{ fontSize: '0.85rem' }}>🌐</span> DFG
                </button>
              </div>
              <div className="action-row">
                <span className="action-row-label">File 2:</span>
                <button className="graph-btn cfg" onClick={() => onGenerateGraph(cmp.file2, cmp.func2, 'cfg')}>
                  <span style={{ fontSize: '0.85rem' }}>🗺️</span> CFG
                </button>
                <button className="graph-btn dfg" onClick={() => onGenerateGraph(cmp.file2, cmp.func2, 'dfg')}>
                  <span style={{ fontSize: '0.85rem' }}>🌐</span> DFG
                </button>
              </div>
            </div>
            
          </div>
        ))}
      </div>
    </div>
  )
}
