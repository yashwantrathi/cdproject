import React from 'react'

export default function CodePreview({ files }) {
  if (!files || files.length === 0) return null

  return (
    <div className="code-preview-section">
      <div className="section-label" style={{ marginBottom: 8 }}>✦ Source Inspector</div>
      <h3 style={{ fontSize: '1.25rem', fontWeight: 600, color: 'var(--text-primary)' }}>Raw Source Code</h3>
      
      <div className="code-preview-grid">
        {files.map((file, idx) => (
          <div key={idx} className="code-preview-card" style={{ animationDelay: `${idx * 0.1}s` }}>
            <div className="code-preview-header">
              <div className="code-dots">
                <div className="code-dot red" />
                <div className="code-dot yellow" />
                <div className="code-dot green" />
              </div>
              <div className="code-preview-filename">{file.filename}</div>
              <span className={`lang-tag ${file.lang}`}>{file.lang.toUpperCase()}</span>
            </div>
            <div className="code-preview-body">
              <pre>{file.content}</pre>
            </div>
          </div>
        ))}
      </div>
    </div>
  )
}
