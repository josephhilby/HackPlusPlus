import { useRef } from 'react'
import './App.css'

import MacFrame from './components/mac/Frame'
import useMachine from './hooks/machineHook'
import { renderFramebufferToCanvas } from './lib/framebuffer'

export default function App() {
  const canvasRef = useRef(null)

  const { status, result, refreshScreen } = useMachine({
    onFramebuffer: (buffer) => {
      renderFramebufferToCanvas(canvasRef.current, buffer)
    },
  })

  return (
      <main className="stage">
        <MacFrame
            canvasRef={canvasRef}
            status={status}
            result={result}
            onRefresh={refreshScreen}
        />

        <p className="footer-hint">Project info here.</p>
      </main>
  )
}