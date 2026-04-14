import { forwardRef } from 'react'
import { WIDTH, HEIGHT } from '../../lib/framebuffer'

const Screen = forwardRef(function MacScreen(_, ref) {
    return (
        <div className="mac-top">
            <div className="mac-screen">
                <div className="mac-crt">
                    <canvas ref={ref} width={WIDTH} height={HEIGHT} />
                    <div className="crt-overlay" aria-hidden="true"></div>
                </div>
            </div>
        </div>
    )
})

export default Screen