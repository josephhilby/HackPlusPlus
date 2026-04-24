const WIDTH = 512
const HEIGHT = 256
const WORDS_PER_ROW = 32

export { WIDTH, HEIGHT }

export function renderFramebufferToCanvas(canvas, framebuffer) {
    if (!canvas) return

    const ctx = canvas.getContext('2d')
    if (!ctx) return

    if (!(framebuffer instanceof Uint16Array)) return

    const image = ctx.createImageData(WIDTH, HEIGHT)
    const data = image.data

    let wordIndex = 0

    for (let row = 0; row < HEIGHT; row++) {
        for (let w = 0; w < WORDS_PER_ROW; w++) {
            const word = framebuffer[wordIndex++] ?? 0

            for (let bit = 0; bit < 16; bit++) {
                const x = w * 16 + bit
                const y = row
                const i = (y * WIDTH + x) * 4

                const on = (word >> bit) & 1

                if (on) {
                    const glow = (Math.random() * 10) | 0

                    data[i + 0] = 90 + (glow >> 1)
                    data[i + 1] = 235 + glow
                    data[i + 2] = 140 + (glow >> 1)
                    data[i + 3] = 255
                } else {
                    data[i + 0] = 4
                    data[i + 1] = 16
                    data[i + 2] = 8
                    data[i + 3] = 255
                }
            }
        }
    }

    ctx.putImageData(image, 0, 0)
}