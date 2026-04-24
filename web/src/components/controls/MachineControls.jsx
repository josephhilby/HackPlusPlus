import './MachineControls.css'

export default function MachineControls({
                                            machineState,
                                            onRun,
                                            onStop,
                                            onStep,
                                            onReset,
                                            canStep,
                                            canReset,
                                            canToggleRun,
                                        }) {
    const isRunning = machineState === 'running'
    const primaryLabel = isRunning ? 'Stop' : 'Run'
    const primaryAction = isRunning ? onStop : onRun
    const primaryClass = isRunning
        ? 'control-btn control-btn--stop'
        : 'control-btn control-btn--run'

    return (
        <section className="control-card">
            <h2 className="control-card-title">Controls</h2>

            <div className="machine-controls-row machine-controls-row--compact">
                <button
                    className={primaryClass}
                    onClick={primaryAction}
                    disabled={!canToggleRun}
                >
                    {primaryLabel}
                </button>

                <button
                    className="control-btn control-btn--step"
                    onClick={onStep}
                    disabled={!canStep}
                >
                    Step
                </button>

                <button
                    className="control-btn control-btn--reset"
                    onClick={onReset}
                    disabled={!canReset}
                >
                    Reset
                </button>
            </div>
        </section>
    )
}