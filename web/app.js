(function () {
    const status = document.getElementById("status");
    const result = document.getElementById("result");
    const button = document.getElementById("btn-test");
    const screen = document.getElementById("screen");
    const ctx = screen.getContext("2d");

    // Paint a simple test pattern so you can verify the screen and scaling.
    function testWebsocket() {
        const image = ctx.createImageData(screen.width, screen.height);
        const data = image.data;

        for (let y = 0; y < screen.height; y++) {
            for (let x = 0; x < screen.width; x++) {
                const i = (y * screen.width + x) * 4;

                // Diagonal + border pattern
                const on = (x === y) || (x === 0) || (y === 0) || (x === screen.width - 1) || (y === screen.height - 1);
                const v = on ? 255 : 0;

                data[i + 0] = v;   // R
                data[i + 1] = v;   // G
                data[i + 2] = v;   // B
                data[i + 3] = 255; // A
            }
        }

        ctx.putImageData(image, 0, 0);
    }

    button.addEventListener("click", () => {
        testWebsocket();
        result.textContent = "Test Complete.";
    });

    status.textContent = "Ready. Select a program to load.";
})();