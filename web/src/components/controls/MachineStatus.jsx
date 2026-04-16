import './MachineStatus.css'

export default function MachineStatus({
                                          state,
                                          programName,
                                          pc,
                                          cycles,
                                      }) {
    return (
        <section className="control-card">
            <h2 className="control-card-title">Machine Status</h2>

            <div className="machine-status-grid">
                <div className="machine-status-row">
                    <span className="machine-status-label">State</span>
                    <span className="machine-status-value">{state}</span>
                </div>

                <div className="machine-status-row">
                    <span className="machine-status-label">Program</span>
                    <span className="machine-status-value">{programName}</span>
                </div>

                <div className="machine-status-row">
                    <span className="machine-status-label">PC</span>
                    <span className="machine-status-value">{pc}</span>
                </div>

                <div className="machine-status-row">
                    <span className="machine-status-label">Cycles</span>
                    <span className="machine-status-value">{cycles}</span>
                </div>
            </div>
        </section>
    )
}