import { WIDTH, HEIGHT, WORDS_PER_ROW } from "../types";

export function renderFramebufferToCanvas(
  canvas: HTMLCanvasElement | null,
  framebuffer: Uint16Array,
) {
  if (!canvas) return;

  const ctx = canvas.getContext("2d");
  if (!ctx) return;

  if (!(framebuffer instanceof Uint16Array)) return;

  const image = ctx.createImageData(WIDTH, HEIGHT);
  const data = image.data;

  let wordIndex = 0;

  for (let row = 0; row < HEIGHT; row++) {
    for (let w = 0; w < WORDS_PER_ROW; w++) {
      const word: number = framebuffer[wordIndex++] ?? 0;

      for (let bit = 0; bit < 16; bit++) {
        const x: number = w * 16 + bit;
        const y: number = row;
        const i: number = (y * WIDTH + x) * 4;

        const on: number = (word >> bit) & 1;

        if (on) {
          const glow: number = (Math.random() * 10) | 0;

          data[i + 0] = 90 + (glow >> 1);
          data[i + 1] = 235 + glow;
          data[i + 2] = 140 + (glow >> 1);
          data[i + 3] = 255;
        } else {
          data[i + 0] = 4;
          data[i + 1] = 16;
          data[i + 2] = 8;
          data[i + 3] = 255;
        }
      }
    }
  }

  ctx.putImageData(image, 0, 0);
}
