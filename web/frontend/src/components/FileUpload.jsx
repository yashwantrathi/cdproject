import React, { useState, useRef } from 'react'
import axios from 'axios'

const API = 'http://localhost:5000'
const SUPPORTED_EXT = ['.c', '.cpp', '.cxx', '.cc', '.rs', '.f90', '.f']

function getLangFromFile(filename) {
  if (filename.endsWith('.c')) return 'c'
  if (filename.endsWith('.cpp') || filename.endsWith('.cxx') || filename.endsWith('.cc')) return 'cpp'
  if (filename.endsWith('.rs')) return 'rust'
  if (filename.endsWith('.f90') || filename.endsWith('.f')) return 'fortran'
  return 'unknown'
}

const LANG_LABELS = { c: 'C', cpp: 'C++', rust: 'Rust', fortran: 'Fortran', unknown: '?' }

export default function FileUpload({ onResults, onError, loading, setLoading }) {
  const [files, setFiles] = useState([])
  const [dragover, setDragover] = useState(false)
  const inputRef = useRef(null)

  const addFiles = (fileList) => {
    const newFiles = Array.from(fileList).filter(f => {
      const ext = '.' + f.name.split('.').pop().toLowerCase()
      return SUPPORTED_EXT.includes(ext)
    })
    setFiles(prev => [...prev, ...newFiles].slice(0, 4))
  }

  const removeFile = (index) => setFiles(prev => prev.filter((_, i) => i !== index))

  const handleDrop = (e) => {
    e.preventDefault()
    setDragover(false)
    addFiles(e.dataTransfer.files)
  }

  const handleCompare = async () => {
    if (files.length < 2) { onError('Please upload at least 2 files.'); return }
    setLoading(true)
    const formData = new FormData()
    files.forEach(f => formData.append('files', f))
    try {
      const res = await axios.post(`${API}/api/upload`, formData, {
        headers: { 'Content-Type': 'multipart/form-data' }
      })
      onResults(res.data)
    } catch (err) {
      onError(err.response?.data?.error || 'Upload failed. Make sure the backend is running.')
    } finally {
      setLoading(false)
    }
  }

  return (
    <div className="glass-card">
      <div className="card-header">
        <div className="card-icon purple">📤</div>
        <div>
          <div className="card-title">Upload Source Files</div>
          <div className="card-subtitle">Drag & drop or click to upload — supports C, C++, Rust, Fortran</div>
        </div>
      </div>

      <div
        id="upload-dropzone"
        className={`upload-zone ${dragover ? 'dragover' : ''}`}
        onClick={() => inputRef.current?.click()}
        onDragOver={(e) => { e.preventDefault(); setDragover(true) }}
        onDragLeave={() => setDragover(false)}
        onDrop={handleDrop}
      >
        <div className="upload-icon-wrap">
          {dragover ? '📂' : '📁'}
        </div>
        <div className="upload-title">
          {dragover ? 'Drop files here!' : 'Click or drag files to upload'}
        </div>
        <div className="upload-subtitle">
          Supports .c · .cpp · .rs · .f90 &nbsp;—&nbsp; Max 4 files
        </div>
        <input
          ref={inputRef}
          type="file"
          multiple
          accept=".c,.cpp,.cxx,.cc,.rs,.f90,.f"
          style={{ display: 'none' }}
          onChange={(e) => addFiles(e.target.files)}
        />
      </div>

      {files.length > 0 && (
        <>
          <div className="file-chips">
            {files.map((f, i) => {
              const lang = getLangFromFile(f.name)
              return (
                <div key={i} className="file-chip">
                  <span className={`lang-tag ${lang}`}>{LANG_LABELS[lang]}</span>
                  <span style={{ fontFamily: 'JetBrains Mono, monospace', fontSize: '0.8rem' }}>{f.name}</span>
                  <button className="file-chip-remove" onClick={() => removeFile(i)}>×</button>
                </div>
              )
            })}
          </div>

          <div style={{ display: 'flex', gap: 10, justifyContent: 'flex-end', marginTop: 20 }}>
            <button className="btn btn-outline btn-sm" onClick={() => setFiles([])}>
              Clear All
            </button>
            <button
              id="btn-upload-compare"
              className="btn btn-primary"
              onClick={handleCompare}
              disabled={files.length < 2 || loading}
            >
              {loading
                ? <><div className="spinner spinner-sm" /> Analyzing…</>
                : `🔬 Compare ${files.length} File${files.length > 1 ? 's' : ''}`
              }
            </button>
          </div>
        </>
      )}
    </div>
  )
}
