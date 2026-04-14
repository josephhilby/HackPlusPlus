import './StatusBar.css'

export default function StatusBar({ status, subtitle }) {
    return (
        <div className="console-status-bar">
            <div className="console-slot" aria-hidden="true"></div>

            <div className="console-status-copy">
                <p className="console-status">{status}</p>
                <p className="console-subtitle">{subtitle}</p>
            </div>
        </div>
    )
}