export default function MachineControls({
                                            onRun,
                                            onStop,
                                            onReset,
                                            canRun,
                                            canStop,
                                            canReset,
                                        }) {
    return (
        <section className="control-card">
            <h2 className="control-card-title">Controls</h2>

            <div className="machine-controls-row">
                <button
                    className="control-btn control-btn--run"
                    onClick={onRun}
                    disabled={!canRun}
                >
                    Run
                </button>

                <button
                    className="control-btn control-btn--stop"
                    onClick={onStop}
                    disabled={!canStop}
                >
                    Stop
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