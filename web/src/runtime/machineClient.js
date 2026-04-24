import computerModuleUrl from './wasm/computer.js?url'

export default class MachineClient {
    constructor() {
        this.module = null
        this.instance = null
    }

    async init() {
        const createModule = (await import(/* @vite-ignore */ computerModuleUrl)).default

        this.module = await createModule()
        this.instance = this.module

        this.instance._init()
    }

    isReady() {
        return !!this.instance
    }

    async load(program) {
        const response = await fetch(program.bin)
        const text = await response.text()

        const romWords = text
            .split(/\r?\n/)
            .map((line) => line.trim())
            .filter(Boolean)
            .map((line) => parseInt(line, 2))

        const rom = Uint16Array.from(romWords)

        if (rom.length > 32768) {
            throw new Error('Program too large for ROM')
        }

        // Get pointer to machine-owned ROM
        const ptr = this.instance._get_rom_ptr()

        // Write directly into Wasm memory
        const heapU16 = new Uint16Array(
            this.instance.HEAPU8.buffer,
            ptr,
            rom.length
        )
        heapU16.set(rom)

        // Tell core how many words are valid
        this.instance._commit_rom(rom.length)

        return this.getState()
    }

    async run() {
        this.instance._run()
        return this.getState()
    }

    async stop() {
        this.instance._stop()
        return this.getState()
    }

    async step() {
        this.instance._step()
        return this.getState()
    }

    async reset() {
        this.instance._reset()
        return this.getState()
    }

    async setKeyboard(value) {
        this.instance._set_keyboard(value)
        return this.getState()
    }

    async getState() {
        const ptr = this.instance._get_state_ptr()
        const view = new DataView(this.instance.HEAPU8.buffer)

        const pc     = view.getUint16(ptr + 0, true)
        const flags  = view.getUint16(ptr + 2, true)
        const cycles = view.getUint32(ptr + 4, true)

        return {
            pc,
            flags,
            cycles,
        }
    }

    async getFramebuffer() {
        const ptr = this.instance._get_framebuffer_ptr()
        const length = 8192

        const view = new Uint16Array(this.instance.HEAPU8.buffer, ptr, length)
        return new Uint16Array(view)
    }
}