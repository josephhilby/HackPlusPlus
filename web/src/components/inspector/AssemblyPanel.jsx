import './AssemblyPanel.css'

function normalizeInstructions(instructions) {
    const result = []
    let address = 0

    for (const line of instructions) {
        const trimmed = line.trim()

        if (!trimmed) continue

        if (/^\(.*\)$/.test(trimmed)) {
            result.push({
                type: 'label',
                label: trimmed.slice(1, -1),
            })
        } else {
            result.push({
                type: 'instruction',
                text: trimmed,
                address,
            })
            address++
        }
    }

    return result
}

export default function AssemblyPanel({
                                          title = 'Assembly Inspector',
                                          instructions = [],
                                          currentPc = null,
                                          onStep,
                                          onClose,
                                          canStep = false,
                                      }) {
    const rows = normalizeInstructions(instructions)

    return (
        <section className="assembly-panel">
            <div className="assembly-panel-header">
                <h2 className="assembly-panel-title">{title}</h2>

                <div className="assembly-panel-actions">
                    <button
                        className="assembly-panel-btn"
                        type="button"
                        onClick={onStep}
                        disabled={!canStep}
                    >
                        Step
                    </button>

                    <button
                        className="assembly-panel-btn assembly-panel-btn--close"
                        type="button"
                        onClick={onClose}
                    >
                        Close
                    </button>
                </div>
            </div>

            <div className="assembly-list">
                {rows.length === 0 ? (
                    <div className="assembly-empty">No program loaded.</div>
                ) : (
                    rows.map((row, i) => {
                        if (row.type === 'label') {
                            return (
                                <div key={`label-${i}`} className="assembly-label">
                                    {row.label}
                                </div>
                            )
                        }

                        const isActive = row.address === currentPc

                        return (
                            <div
                                key={`inst-${row.address}`}
                                className={`assembly-row ${isActive ? 'assembly-row--active' : ''}`}
                            >
                <span className="assembly-address">
                  <span className="assembly-address-prefix">0x</span>
                    {row.address.toString(16).toUpperCase().padStart(4, '0')}
                </span>

                                <code className="assembly-text">{row.text}</code>
                            </div>
                        )
                    })
                )}
            </div>
        </section>
    )
}