import './StatusBar.css'

export default function StatusBar({status}) {
    return (
        <div className="console-status-bar">
            <p className="console-status">{status}</p>
        </div>
    )
}