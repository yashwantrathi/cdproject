import React, { useState } from 'react'

export default function GraphViewer({ isOpen, onClose, imageUrl, title, loading, error, dotText, metrics }) {
  const [viewMode, setViewMode] = useState('visual') // 'visual' | 'text'

  if (!isOpen) return null

  // Extract metrics block
  let metricsText = metrics || ""
  if (metricsText.includes("====== DFG ======")) {
    metricsText = "====== DFG ======" + metricsText.split("====== DFG ======")[1]
  } else if (metricsText.includes("====== CFG ======")) {
    metricsText = "====== CFG ======" + metricsText.split("====== CFG ======")[1]
  }
  metricsText = metricsText.replace(/GRAPH_GENERATED:.*(\r?\n|$)/g, "")

  const isCFG = title.startsWith('CFG')

  return (
    <div className="modal-backdrop" onClick={onClose}>
      <div className="graph-modal-box" onClick={e => e.stopPropagation()}>
        
        {/* Header */}
        <div className="modal-top">
          <div className={`modal-type-badge ${isCFG ? 'cfg' : 'dfg'}`}>
            {isCFG ? 'Control Flow' : 'Data Flow'}
          </div>
          <div className="modal-title-text">{title}</div>
          
          {imageUrl && !loading && !error && (
            <div className="view-toggle-group">
              <button 
                className={`view-toggle-btn ${viewMode === 'visual' ? 'active' : ''}`}
                onClick={() => setViewMode('visual')}
              >
                🖼️ Visual
              </button>
              <button 
                className={`view-toggle-btn ${viewMode === 'text' ? 'active' : ''}`}
                onClick={() => setViewMode('text')}
              >
                📝 Text/Metrics
              </button>
            </div>
          )}
          
          <button className="modal-close-btn" onClick={onClose}>✕</button>
        </div>

        {/* Body */}
        <div className="modal-body">
          {loading && (
            <div className="modal-loading">
              <div className="spinner" />
              <div style={{ fontWeight: 500 }}>Generating graph using LLVM...</div>
            </div>
          )}
          
          {error && (
            <div className="modal-error">
              <span style={{ fontSize: '1.25rem' }}>⚠️</span>
              <span style={{ fontWeight: 500 }}>{error}</span>
            </div>
          )}

          {imageUrl && !loading && !error && viewMode === 'visual' && (
            <div className="modal-graph-view">
              <img src={imageUrl} alt={title} className="graph-img" />
            </div>
          )}

          {imageUrl && !loading && !error && viewMode === 'text' && (
            <div className="modal-text-view">
              <div className="text-view-section">
                <div className={`text-view-label ${isCFG ? 'cyan' : 'purple'}`}>✦ Metrics Output</div>
                <pre className="text-view-pre">
                  {metricsText.trim()}
                </pre>
              </div>
              
              <div className="text-view-section">
                <div className={`text-view-label ${isCFG ? 'cyan' : 'purple'}`}>✦ Raw Graphviz DOT Data</div>
                <pre className="text-view-pre">
                  {dotText || "No DOT file found."}
                </pre>
              </div>
            </div>
          )}
        </div>

        {/* Footer */}
        <div className="modal-footer">
          {imageUrl && !loading && !error && (
            <a href={imageUrl} download={`${title.replace(/\s+/g, '_')}.png`} className="btn btn-primary btn-sm">
              ⬇️ Download High-Res Image
            </a>
          )}
          <button className="btn btn-outline btn-sm" onClick={onClose}>Close</button>
        </div>
      </div>
    </div>
  )
}
