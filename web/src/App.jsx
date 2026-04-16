import { useEffect, useRef, useState } from 'react'
import './App.css'

import Frame from './components/computer/Frame'
import ProgramLoader from './components/controls/ProgramLoader'
import MachineControls from './components/controls/MachineControls'
import MachineStatus from './components/controls/MachineStatus'
import AssemblyPanel from './components/inspector/AssemblyPanel'

import useMachine from './hooks/machineHook'
import { renderFramebufferToCanvas } from './lib/framebuffer'
import { asmLoader } from './lib/asmLoader'

import { programCatalog } from './config/programCatalog.js'
import { formatMachineState } from './lib/machineState'

export default function App() {
    const canvasRef = useRef(null)
    const [selectedProgramId, setSelectedProgramId] = useState(programCatalog[0]?.id ?? '')
    const [assemblyLines, setAssemblyLines] = useState([])

    const {
        runtimeStatus,
        machineState,
        load,
        run,
        stop,
        step,
        reset,
    } = useMachine({
        onFramebuffer: (buffer) => {
            renderFramebufferToCanvas(canvasRef.current, buffer)
        },
    })

    const selectedProgram =
        programCatalog.find((program) => program.id === selectedProgramId) ?? null

    const loadedProgram =
        programCatalog.find((program) => program.id === machineState.programId) ?? null

    useEffect(() => {
        let cancelled = false

        async function syncAssemblySource() {
            if (!loadedProgram) {
                setAssemblyLines([])
                return
            }

            try {
                const lines = await asmLoader(loadedProgram)
                if (!cancelled) {
                    setAssemblyLines(lines)
                }
            } catch (error) {
                if (!cancelled) {
                    setAssemblyLines([`// Failed to load ASM source: ${error.message}`])
                }
            }
        }

        syncAssemblySource()

        return () => {
            cancelled = true
        }
    }, [loadedProgram])

    function handleLoadProgram() {
        if (!selectedProgram) return
        load(selectedProgram)
    }

    function handleRun() {
        run()
    }

    function handleStop() {
        stop()
    }

    function handleStep() {
        step()
    }

    function handleReset() {
        reset()
    }

    const hasLoadedProgram = !!machineState.programId
    const isRunning = machineState.status === 'running'
    const canStep = hasLoadedProgram && !isRunning
    const canReset =
        hasLoadedProgram &&
        (machineState.status === 'loaded' || machineState.status === 'stopped')

    return (
        <main className="app-shell">
            <div className="console-stack">
                <Frame
                    canvasRef={canvasRef}
                    status={runtimeStatus || 'Ready'}
                />

                <div className="control-dock">
                    <div className="control-dock-main">
                        <ProgramLoader
                            programs={programCatalog}
                            selectedProgramId={selectedProgramId}
                            onProgramChange={setSelectedProgramId}
                            onLoad={handleLoadProgram}
                        />

                        <MachineControls
                            machineState={machineState.status}
                            onRun={handleRun}
                            onStop={handleStop}
                            onStep={handleStep}
                            onReset={handleReset}
                            canToggleRun={hasLoadedProgram}
                            canStep={canStep}
                            canReset={canReset}
                        />
                    </div>

                    <div className="control-dock-side">
                        <MachineStatus
                            state={formatMachineState(machineState.status)}
                            programName={loadedProgram ? loadedProgram.id : 'None'}
                            pc={machineState.pc}
                            cycles={machineState.cycles}
                        />
                    </div>
                </div>

                <AssemblyPanel
                    title="Assembly"
                    instructions={assemblyLines}
                    currentPc={loadedProgram ? machineState.pc : null}
                />
            </div>
        </main>
    )
}