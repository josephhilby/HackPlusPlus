import { useRef, useState } from 'react'
import './App.css'

import Frame from './components/computer/Frame'
import ProgramLoader from './components/controls/ProgramLoader'
import MachineControls from './components/controls/MachineControls'
import useMachine from './hooks/machineHook'
import { renderFramebufferToCanvas } from './lib/framebuffer'

const mockPrograms = [
    { id: 'fill', name: 'Fill.asm' },
    { id: 'mult', name: 'Mult.asm' },
    { id: 'rect', name: 'Rect.asm' },
]

export default function App() {
    const canvasRef = useRef(null)
    const [selectedProgramId, setSelectedProgramId] = useState('fill')
    const [loadedProgramId, setLoadedProgramId] = useState(null)
    const [machineState, setMachineState] = useState('idle')

    const { status } = useMachine({
        onFramebuffer: (buffer) => {
            renderFramebufferToCanvas(canvasRef.current, buffer)
        },
    })

    const loadedProgram =
        mockPrograms.find((program) => program.id === loadedProgramId) ?? null

    function handleLoadProgram() {
        setLoadedProgramId(selectedProgramId)
        setMachineState('loaded')
        console.log('Load program:', selectedProgramId)
    }

    function handleRun() {
        if (!loadedProgramId) return
        setMachineState('running')
        console.log('Run program:', loadedProgramId)
    }

    function handleStop() {
        if (!loadedProgramId) return
        setMachineState('stopped')
        console.log('Stop program:', loadedProgramId)
    }

    function handleReset() {
        if (!loadedProgramId) return
        setMachineState('loaded')
        console.log('Reset program:', loadedProgramId)
    }

    return (
        <main className="app-shell">
            <div className="console-stack">
                <Frame
                    status={status || 'Ready'}
                    subtitle={
                        loadedProgram
                            ? `${loadedProgram.name} • ${machineState}`
                            : 'No program loaded.'
                    }
                />

                <MachineControls
                    onRun={handleRun}
                    onStop={handleStop}
                    onReset={handleReset}
                    canRun={!!loadedProgramId && machineState !== 'running'}
                    canStop={!!loadedProgramId && machineState === 'running'}
                    canReset={!!loadedProgramId && machineState === 'stopped'}
                />

                <ProgramLoader
                    programs={mockPrograms}
                    selectedProgramId={selectedProgramId}
                    onProgramChange={setSelectedProgramId}
                    onLoad={handleLoadProgram}
                />
            </div>
        </main>
    )
}