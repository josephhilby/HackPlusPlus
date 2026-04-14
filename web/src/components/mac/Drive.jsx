export default function Drive({ status }) {
    return (
        <div className="mac-floppy">
            <div className="slot" aria-hidden="true"></div>
            <div className="label">{status}</div>
        </div>
    )
}