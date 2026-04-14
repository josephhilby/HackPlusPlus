import { useEffect, useRef, useState } from 'react'
import './App.css'

const WIDTH = 512
const HEIGHT = 256
const WORDS_PER_ROW = 32 // 512 / 16

export default function App() {
  const canvasRef = useRef(null)
  const wsRef = useRef(null)

  const [status, setStatus] = useState('Ready. Click to connect.')
  const [result, setResult] = useState('')

  function renderFramebuffer(buffer) {
    const canvas = canvasRef.current
    if (!canvas) return

    const ctx = canvas.getContext('2d')
    if (!ctx) return

    const bytes = new Uint8Array(buffer)
    const image = ctx.createImageData(WIDTH, HEIGHT)
    const data = image.data

    let byteIndex = 0

    for (let row = 0; row < HEIGHT; row++) {
      for (let w = 0; w < WORDS_PER_ROW; w++) {
        // little-endian 16-bit word
        const lo = bytes[byteIndex++]
        const hi = bytes[byteIndex++]
        const word = lo | (hi << 8)

        for (let bit = 0; bit < 16; bit++) {
          const x = w * 16 + bit
          const y = row
          const i = (y * WIDTH + x) * 4

          // if flipped, use (15 - bit)
          const on = (word >> bit) & 1

          // 1 = black, 0 = white add some bias for crt effect
          const v = on ? 0 : 190 + ((Math.random() * 6) | 0)

          data[i + 0] = v
          data[i + 1] = v
          data[i + 2] = v
          data[i + 3] = 255
        }
      }
    }

    ctx.putImageData(image, 0, 0)
  }

  function connectWS() {
    const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:'
    const url = `${protocol}//${window.location.host}/ws`

    const ws = new WebSocket(url)
    ws.binaryType = 'arraybuffer'
    wsRef.current = ws

    ws.onopen = () => {
      setStatus('WebSocket connected.')
      setResult('')
    }

    ws.onmessage = (ev) => {
      if (typeof ev.data === 'string') {
        setResult(ev.data)
      } else {
        renderFramebuffer(ev.data)
        setResult('Screen updated.')
      }
    }

    ws.onclose = () => {
      setStatus('WebSocket closed.')
    }

    ws.onerror = () => {
      setStatus('WebSocket error.')
    }
  }

  function handleTestClick() {
    const ws = wsRef.current

    if (!ws || ws.readyState !== WebSocket.OPEN) {
      connectWS()
    } else {
      ws.send('get')
    }
  }

  useEffect(() => {
    return () => {
      if (wsRef.current) {
        wsRef.current.close()
      }
    }
  }, [])

  return (
      <main className="stage">
        <div
            className="mac128k"
            role="application"
            aria-label="Hack WebEmu Macintosh-style frame"
        >
          <div className="mac-top">
            <div className="mac-screen">
              <div className="mac-crt">
                <canvas
                    ref={canvasRef}
                    id="screen"
                    width={WIDTH}
                    height={HEIGHT}
                />
                <div className="crt-overlay" aria-hidden="true"></div>
              </div>
            </div>
          </div>

          <div className="mac-bottom">
            <div className="mac-emboss">
              <div className="mac-logo">
                <div className="bars">
                  <i></i><i></i><i></i><i></i><i></i><i></i>
                </div>
                <div className="mac-model">
                  HACK++ <br /> 128K
                </div>
              </div>
            </div>

            <div className="mac-floppy">
              <div className="slot" aria-hidden="true"></div>
              <div className="label">{status}</div>
            </div>

            <div className="mac-controls">
              <button className="btn" onClick={handleTestClick}>
                WebSocket Test
              </button>
              <span className="hint">{result}</span>
            </div>
          </div>
        </div>

        <p className="footer-hint">Project info here.</p>
      </main>
  )
}