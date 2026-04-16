export function formatMachineState(state) {
    switch (state) {
        case 'loaded':
            return 'READY'
        case 'running':
            return 'RUNNING'
        case 'stopped':
            return 'STOPPED'
        default:
            return 'IDLE'
    }
}