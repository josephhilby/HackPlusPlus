import { useRef } from 'react'
import './App.css'

import Frame from './components/computer/Frame'
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
        <main className="app-shell">
            <Frame
                status="Running"
                subtitle="Fill.asm • PC: 23"
            />
        </main>
    )
}