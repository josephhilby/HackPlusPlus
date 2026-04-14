import { useEffect, useRef, useState } from 'react'
import MachineClient from '../runtime/machineClient'

export default function useMachine({ onFramebuffer }) {
    const machineRef = useRef(null)

    const [status, setStatus] = useState('Initializing Hack++ runtime...')
    const [result, setResult] = useState('')

    useEffect(() => {
        let cancelled = false

        async function init() {
            try {
                const machine = new HackMachineClient()
                await machine.init()

                if (cancelled) return

                machineRef.current = machine
                setStatus('Hack++ runtime ready.')
            } catch (error) {
                if (cancelled) return
                setStatus('Runtime initialization failed.')
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

    return {
        status,
        result,
        refreshScreen,
    }
}