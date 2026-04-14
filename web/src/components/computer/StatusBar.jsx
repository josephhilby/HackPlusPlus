import './StatusBar.css'

export default function StatusBar({ status, subtitle }) {
    return (
        <div className="console-status-bar">
            <p className="console-status">{status}</p>
            <p className="console-subtitle">{subtitle}</p>
        </div>
    )
}