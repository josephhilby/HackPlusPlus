import './Frame.css'
import Screen from './Screen'
import StatusBar from './StatusBar'
import Badge from './Badge.jsx'

export default function Frame({status}) {
    return (
        <div className="console-frame">
            <div className="console-top">
                <Screen />
            </div>

            <div className="console-bottom">
                <Badge />
                <StatusBar status={status} />
            </div>
        </div>
    )
}