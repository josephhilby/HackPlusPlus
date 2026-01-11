(function () {
    const status = document.getElementById("status");
    const result = document.getElementById("result");
    const btn = document.getElementById("btn-test");
    const canvas = document.getElementById("screen");
    const ctx = canvas.getContext("2d");

    // Paint a simple test pattern so you can verify the canvas and scaling.
    function drawTestPattern() {
        const img = ctx.createImageData(canvas.width, canvas.height);
        const data = img.data;

        for (let y = 0; y < canvas.height; y++) {
            for (let x = 0; x < canvas.width; x++) {
                const i = (y * canvas.width + x) * 4;

                // Diagonal + border pattern
                const on = (x === y) || (x === 0) || (y === 0) || (x === canvas.width - 1) || (y === canvas.height - 1);
                const v = on ? 255 : 0;

                data[i + 0] = v;   // R
                data[i + 1] = v;   // G
                data[i + 2] = v;   // B
                data[i + 3] = 255; // A
            }
        }

        ctx.putImageData(img, 0, 0);
    }

    btn.addEventListener("click", () => {
        drawTestPattern();
        result.textContent = "Canvas test pattern drawn.";
    });

    status.textContent = "Ready. Select a program to load.";
})();