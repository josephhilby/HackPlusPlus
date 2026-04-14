import './Screen.css'

export default function Screen() {
    return (
        <div className="console-screen-shell">
            <div className="console-screen-bezel">
                <div className="console-crt">
                    <div className="console-screen-placeholder">
                        <span>Hack++ Display</span>
                    </div>
                    <div className="console-crt-overlay" aria-hidden="true"></div>
                </div>
            </div>
        </div>
    )
}