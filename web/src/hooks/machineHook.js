import { useEffect, useRef, useState } from 'react'
import MachineClient from '../runtime/machineClient'

export default function useMachine({ onFramebuffer }) {
    const machineRef = useRef(null)

    const [runtimeStatus, setRuntimeStatus] = useState('Initializing Hack++ runtime...')
    const [result, setResult] = useState('')

    const [machineState, setMachineState] = useState({
        status: 'idle',
        pc: 0,
        flags: 0,
        cycles: 0,
        programId: null,
    })

    useEffect(() => {
        let cancelled = false

        async function init() {
            try {
                const machine = new MachineClient()
                await machine.init()

                if (cancelled) return

                machineRef.current = machine
                setRuntimeStatus('Hack++ runtime ready.')

                const initialState = await machine.getState()
                if (!cancelled) {
                    setMachineState((prev) => ({
                        ...prev,
                        ...initialState,
                    }))
                }
            } catch (error) {
                if (cancelled) return
                setRuntimeStatus('Runtime initialization failed.')
                setResult(error instanceof Error ? error.message : 'Unknown error.')
            }
        }

        init()

        return () => {
            cancelled = true
        }
    }, [])

    async function refreshScreen() {
        try {
            const machine = machineRef.current
            if (!machine || !machine.isReady()) {
                setResult('Machine not ready.')
                return
            }

            const buffer = await machine.getFramebuffer()
            onFramebuffer?.(buffer)
            setResult('Screen updated.')
        } catch (error) {
            setResult(error instanceof Error ? error.message : 'Unknown error.')
        }
    }

    async function load(program) {
        try {
            const machine = machineRef.current
            if (!machine || !machine.isReady()) {
                setResult('Machine not ready.')
                return
            }

            const nextState = await machine.load(program)
            setMachineState({
                ...nextState,
                programId: program.id,
            })
            await refreshScreen()
        } catch (error) {
            setResult(error instanceof Error ? error.message : 'Unknown error.')
        }
    }

    async function run() {
        try {
            const machine = machineRef.current
            if (!machine || !machine.isReady()) {
                setResult('Machine not ready.')
                return
            }

            const nextState = await machine.run()
            setMachineState((prev) => ({
                ...prev,
                ...nextState,
            }))
        } catch (error) {
            setResult(error instanceof Error ? error.message : 'Unknown error.')
        }
    }

    async function stop() {
        try {
            const machine = machineRef.current
            if (!machine || !machine.isReady()) {
                setResult('Machine not ready.')
                return
            }

            const nextState = await machine.stop()
            setMachineState((prev) => ({
                ...prev,
                ...nextState,
            }))
        } catch (error) {
            setResult(error instanceof Error ? error.message : 'Unknown error.')
        }
    }

    async function step() {
        try {
            const machine = machineRef.current
            if (!machine || !machine.isReady()) {
                setResult('Machine not ready.')
                return
            }

            const nextState = await machine.step()
            setMachineState((prev) => ({
                ...prev,
                ...nextState,
            }))
            await refreshScreen()
        } catch (error) {
            setResult(error instanceof Error ? error.message : 'Unknown error.')
        }
    }

    async function reset() {
        try {
            const machine = machineRef.current
            if (!machine || !machine.isReady()) {
                setResult('Machine not ready.')
                return
            }

            const nextState = await machine.reset()
            setMachineState((prev) => ({
                ...prev,
                ...nextState,
            }))
            await refreshScreen()
        } catch (error) {
            setResult(error instanceof Error ? error.message : 'Unknown error.')
        }
    }

    async function setKeyboard(value) {
        try {
            const machine = machineRef.current
            if (!machine || !machine.isReady()) {
                setResult('Machine not ready.')
                return
            }

            const nextState = await machine.setKeyboard(value)
            setMachineState((prev) => ({
                ...prev,
                ...nextState,
            }))
            await refreshScreen()
        } catch (error) {
            setResult(error instanceof Error ? error.message : 'Unknown error.')
        }
    }

    return {
        runtimeStatus,
        result,
        machineState,
        load,
        run,
        stop,
        step,
        reset,
        setKeyboard,
        refreshScreen,
    }
}