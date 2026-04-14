export default function Controls({ result, onRefresh }) {
    return (
        <div className="mac-controls">
            <button className="btn" onClick={onRefresh}>
                Refresh Screen
            </button>
            <span className="hint">{result}</span>
        </div>
    )
}