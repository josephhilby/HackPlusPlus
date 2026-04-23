export function formatMachineState(machineState) {
    if (!machineState) return 'IDLE'

    const { pc = 0, flags = 0 } = machineState

    const isRunning = (flags & 0x0001) !== 0
    const isLoaded = (flags & 0x0002) !== 0
    const isError = (flags & 0x0004) !== 0

    if (isError) return 'ERROR'
    if (isRunning) return 'RUNNING'
    if (!isLoaded) return 'IDLE'
    if (pc === 0) return 'READY'
    return 'STOPPED'
}