(function () {
    const status = document.getElementById("status");
    const result = document.getElementById("result");
    const button = document.getElementById("btn-test");
    const screen = document.getElementById("screen");
    const ctx = screen.getContext("2d");

    const WIDTH = 512;
    const HEIGHT = 256;
    const WORDS_PER_ROW = 32; // 512 / 16

    let ws = null;

    function renderFramebuffer(buffer) {
        const bytes = new Uint8Array(buffer);
        const image = ctx.createImageData(WIDTH, HEIGHT);
        const data = image.data;

        let byteIndex = 0;

        for (let row = 0; row < HEIGHT; row++) {
            for (let w = 0; w < WORDS_PER_ROW; w++) {
                // little-endian 16-bit word
                const lo = bytes[byteIndex++];
                const hi = bytes[byteIndex++];
                const word = lo | (hi << 8);

                for (let bit = 0; bit < 16; bit++) {
                    const x = w * 16 + bit;
                    const y = row;
                    const i = (y * WIDTH + x) * 4;

                    // if flipped, use (15 - bit)
                    const on = (word >> bit) & 1;
                    // 1 = black, 0 = white add some bias for crt effect
                    const v = on ? 0 : (190 + ((Math.random() * 6) | 0));

                    data[i + 0] = v;    // R
                    data[i + 1] = v;    // G
                    data[i + 2] = v;    // B
                    data[i + 3] = 255;
                }
            }
        }

        ctx.putImageData(image, 0, 0);
    }

    function connectWS() {
        const url = `ws://${location.host}/ws`;
        ws = new WebSocket(url);
        ws.binaryType = "arraybuffer";

        ws.onopen = () => {
            status.textContent = "WebSocket connected.";
            result.textContent = "";
        };

        ws.onmessage = (ev) => {
            if (typeof ev.data === "string") {
                result.textContent = ev.data;
            } else {
                renderFramebuffer(ev.data);
                result.textContent = "Screen updated.";
            }
        };

        ws.onclose = () => {
            status.textContent = "WebSocket closed.";
        };
    }

    button.addEventListener("click", () => {
        if (!ws || ws.readyState !== WebSocket.OPEN) {
            connectWS();
        } else {
            ws.send("get"); // ask server to resend framebuffer
        }
    });

    status.textContent = "Ready. Click to connect.";
})();
