import Screen from './Screen'
import Badge from './Badge'
import Drive from './Drive'
import Controls from './Controls'

export default function Frame({
        canvasRef,
        status,
        result,
        onRefresh,
    }) {
    return (
        <div
            className="mac128k"
            role="application"
            aria-label="Hack++ Macintosh-style frame"
        >
            <Screen ref={canvasRef} />

            <div className="mac-bottom">
                <Badge />
                <Drive status={status} />
                <Controls result={result} onRefresh={onRefresh} />
            </div>
        </div>
    )
}