import './ProgramLoader.css'

export default function ProgramLoader({
                                          programs,
                                          selectedProgramId,
                                          onProgramChange,
                                          onLoad,
                                          onView,
                                          canView,
                                      }) {
    return (
        <section className="control-card">
            <h2 className="control-card-title">Program</h2>

            <div className="program-loader">
                <div className="program-loader-row">
                    <select
                        id="program-select"
                        className="program-select"
                        value={selectedProgramId}
                        onChange={(event) => onProgramChange?.(event.target.value)}
                    >
                        {programs.map((program) => (
                            <option key={program.id} value={program.id}>
                                {program.id}
                            </option>
                        ))}
                    </select>

                    <div className="program-loader-actions">
                        <button
                            className="control-btn"
                            type="button"
                            onClick={onLoad}
                        >
                            Load
                        </button>

                        <button
                            className="control-btn"
                            type="button"
                            onClick={onView}
                            disabled={!canView}
                        >
                            View
                        </button>
                    </div>
                </div>
            </div>
        </section>
    )
}