import './Badge.css'

export default function Badge() {
    return (
        <div className="console-badge">
            <div className="console-badge-mark">
                <div className="console-bars">
                    <i></i><i></i><i></i><i></i><i></i><i></i>
                </div>
            </div>

            <div className="console-badge-copy">
                <span className="console-badge-title">HACK++</span>
                <span className="console-badge-model">16-bit</span>
            </div>
        </div>
    )
}