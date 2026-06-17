import { HEIGHT, WIDTH } from "../../types";

import "./Screen.css";

export default function Screen({ canvasRef }) {
  return (
    <div className="console-screen-shell">
      <div className="console-screen-bezel">
        <div className="console-crt">
          <canvas
            ref={canvasRef}
            className="console-screen-canvas"
            width={WIDTH}
            height={HEIGHT}
          />
          <div className="console-crt-overlay" aria-hidden="true"></div>
        </div>
      </div>
    </div>
  );
}
