import './Frame.css'
import Screen from './Screen'
import Badge from './Badge'
import StatusBar from './StatusBar'

export default function Frame({ canvasRef, status }) {
    return (
        <div className="console-frame">
            <div className="console-top">
                <Screen canvasRef={canvasRef} />
            </div>

            <div className="console-bottom">
                <Badge />
                <StatusBar status={status} />
            </div>
        </div>
    )
}