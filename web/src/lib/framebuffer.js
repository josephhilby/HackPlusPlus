const WIDTH = 512
const HEIGHT = 256
const WORDS_PER_ROW = 32

export { WIDTH, HEIGHT }

export function renderFramebufferToCanvas(canvas, buffer) {
    if (!canvas) return

    const ctx = canvas.getContext('2d')
    if (!ctx) return

    const bytes = new Uint8Array(buffer)
    const image = ctx.createImageData(WIDTH, HEIGHT)
    const data = image.data

    let byteIndex = 0

    for (let row = 0; row < HEIGHT; row++) {
        for (let w = 0; w < WORDS_PER_ROW; w++) {
            const lo = bytes[byteIndex++]
            const hi = bytes[byteIndex++]
            const word = lo | (hi << 8)

            for (let bit = 0; bit < 16; bit++) {
                const x = w * 16 + bit
                const y = row
                const i = (y * WIDTH + x) * 4

                const on = (word >> bit) & 1
                const v = on ? 0 : 190 + ((Math.random() * 6) | 0)

                data[i + 0] = v
                data[i + 1] = v
                data[i + 2] = v
                data[i + 3] = 255
            }
        }
    }

    ctx.putImageData(image, 0, 0)
}