const RAM_WORDS = 24577
const ROM_WORDS = 32768

const SCREEN_BASE = 16384
const SCREEN_WORDS = 8192
const SCREEN_END = SCREEN_BASE + SCREEN_WORDS - 1

const KEYBOARD_ADDR = 24576

const SCREEN_WIDTH = 512
const SCREEN_HEIGHT = 256
const WORDS_PER_ROW = SCREEN_WIDTH / 16

const FRAMEBUFFER_BYTES = SCREEN_WORDS * 2

/*
Hack++ Core ABI
    init()
    load(program)
    run()
    stop()
    step()
    reset()

    set_keyboard(uint16_t value)

    get_state()
    get_framebuffer_ptr()
 */

export default class MockMachine {
    constructor() {
        this.ready = false
        this.program = null

        // RAM space through keyboard MMIO
        this.ram = new Uint16Array(RAM_WORDS)
        this.rom = new Uint16Array(ROM_WORDS)

        this.state = {
            status: 'idle',
            pc: 0,
            cycles: 0,
            programId: null,
            keyboard: 0,
            romSize: 0,
        }
    }

    async init() {
        this.ready = true
    }

    isReady() {
        return this.ready
    }

    load(program) {
        this.#assertReady()

        this.program = program
        this.rom = program.rom ?? new Uint16Array()

        this.state = {
            status: 'loaded',
            pc: 0,
            cycles: 0,
            programId: program?.id ?? null,
            keyboard: 0,
            romSize: this.rom.length,
        }

        this.ram.fill(0)
    }

    run() {
        this.#assertReady()
        this.#assertProgramLoaded()

        this.state.status = 'running'
    }

    stop() {
        this.#assertReady()
        this.#assertProgramLoaded()

        this.state.status = 'stopped'
    }

    step() {
        this.#assertReady()
        this.#assertProgramLoaded()

        if (this.state.status === 'running') {
            throw new Error('Cannot step while running.')
        }

        const instructionCount = this.state.romSize ?? this.rom?.length ?? 0

        if (instructionCount === 0) {
            return this.getState()
        }

        this.state.pc = Math.min(this.state.pc + 1, instructionCount - 1)
        this.state.cycles += 1
        this.state.status = 'stopped'

        this.#updateMockFramebuffer()

        return this.getState()
    }

    reset() {
        this.#assertReady()
        this.#assertProgramLoaded()

        this.ram.fill(0)

        this.state.pc = 0
        this.state.cycles = 0
        this.state.status = 'loaded'
        this.state.keyboard = 0
    }

    setKeyboard(value) {
        this.#assertReady()

        const normalized = value & 0xffff
        this.ram[KEYBOARD_ADDR] = normalized
        this.state.keyboard = normalized
    }

    getState() {
        return {
            ...this.state,
            mmio: {
                screenBase: SCREEN_BASE,
                screenWords: SCREEN_WORDS,
                screenEnd: SCREEN_END,
                keyboardAddr: KEYBOARD_ADDR,
            },
        }
    }

    getFramebuffer() {
        const screenWords = this.ram.slice(SCREEN_BASE, SCREEN_BASE + SCREEN_WORDS)
        return screenWords.buffer.slice(0)
    }

    #assertReady() {
        if (!this.ready) {
            throw new Error('Machine not initialized.')
        }
    }

    #assertProgramLoaded() {
        if (!this.program) {
            throw new Error('No program loaded.')
        }
    }

    #updateMockFramebuffer() {
        // Clear only screen MMIO region, not the whole RAM.
        this.ram.fill(0, SCREEN_BASE, SCREEN_BASE + SCREEN_WORDS)

        // Fake pattern tied to PC/cycles so the screen visibly changes.
        const activeRow = this.state.pc % SCREEN_HEIGHT
        const rowBase = SCREEN_BASE + activeRow * WORDS_PER_ROW

        for (let word = 0; word < WORDS_PER_ROW; word++) {
            const pattern = (word + this.state.cycles) % 2 === 0 ? 0xffff : 0x0000
            this.ram[rowBase + word] = pattern
        }

        // Optional visual effect: if keyboard MMIO is nonzero, light a second row.
        if (this.state.keyboard !== 0) {
            const keyboardRow = (activeRow + 2) % SCREEN_HEIGHT
            const keyboardRowBase = SCREEN_BASE + keyboardRow * WORDS_PER_ROW

            for (let word = 0; word < WORDS_PER_ROW; word++) {
                this.ram[keyboardRowBase + word] = 0xffff
            }
        }
    }
}