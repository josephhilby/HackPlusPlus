import { useEffect, useRef, useState } from 'react'
import './App.css'

import Frame from './components/computer/Frame'
import ProgramLoader from './components/controls/ProgramLoader'
import MachineControls from './components/controls/MachineControls'
import MachineStatus from './components/controls/MachineStatus'
import AssemblyPanel from './components/inspector/AssemblyPanel'

import useMachine from './hooks/machineHook'

import { renderFramebufferToCanvas } from './lib/framebuffer'
import { formatMachineState } from './lib/machineState'
import { asmLoader } from './lib/asmLoader'

import { programCatalog } from './config/programCatalog.js'

export default function App() {
    const canvasRef = useRef(null)
    const [selectedProgramId, setSelectedProgramId] = useState(programCatalog[0]?.id ?? '')
    const [assemblyLines, setAssemblyLines] = useState([])
    const [isAssemblyOpen, setIsAssemblyOpen] = useState(false)
    const [isLoadingProgram, setIsLoadingProgram] = useState(false)

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

    async function handleLoadProgram() {
        if (!selectedProgram) return

        try {
            setIsLoadingProgram(true)
            await load(selectedProgram)
        } finally {
            setIsLoadingProgram(false)
        }
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

    function handleOpenAssemblyModal() {
        setIsAssemblyOpen(true)
    }

    function handleCloseAssemblyModal() {
        setIsAssemblyOpen(false)
    }

    const machineStatus = formatMachineState(machineState)
    const hasLoadedProgram = !!machineState.programId
    const isRunning = (machineState.flags & 0x0001) !== 0
    const isLoaded = (machineState.flags & 0x0002) !== 0

    const canStep = hasLoadedProgram && !isRunning && !isLoadingProgram
    const canReset = hasLoadedProgram && isLoaded && !isLoadingProgram

    return (
        <main className="app-shell">
            <div className="console-stack">
                <Frame
                    canvasRef={canvasRef}
                    status={isLoadingProgram ? 'Loading program...' : (runtimeStatus || 'Ready')}
                />

                <div className="control-dock">
                    <div className="control-dock-main">
                        <ProgramLoader
                            programs={programCatalog}
                            selectedProgramId={selectedProgramId}
                            onProgramChange={setSelectedProgramId}
                            onLoad={handleLoadProgram}
                            onView={handleOpenAssemblyModal}
                            canView={hasLoadedProgram && !isLoadingProgram}
                            isLoading={isLoadingProgram}
                        />

                        <MachineControls
                            machineState={machineStatus}
                            onRun={handleRun}
                            onStop={handleStop}
                            onStep={handleStep}
                            onReset={handleReset}
                            canToggleRun={hasLoadedProgram && !isLoadingProgram}
                            canStep={canStep}
                            canReset={canReset}
                        />
                    </div>

                    <div className="control-dock-side">
                        <MachineStatus
                            state={isLoadingProgram ? 'LOADING' : formatMachineState(machineState.status)}
                            programName={loadedProgram ? loadedProgram.id : 'None'}
                            pc={machineState.pc}
                            cycles={machineState.cycles}
                        />
                    </div>
                </div>
            </div>

            {isAssemblyOpen && (
                <div
                    className="modal-overlay"
                    onClick={handleCloseAssemblyModal}
                >
                    <div
                        className="modal-dialog"
                        onClick={(event) => event.stopPropagation()}
                    >
                        <AssemblyPanel
                            title="Assembly"
                            instructions={assemblyLines}
                            currentPc={loadedProgram ? machineState.pc : null}
                            onStep={handleStep}
                            onClose={handleCloseAssemblyModal}
                            canStep={canStep}
                        />
                    </div>
                </div>
            )}
        </main>
    )
}