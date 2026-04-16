import MockMachine from './mockMachine'

export default class MachineClient {
    constructor() {
        this.machine = new MockMachine()
    }

    async init() {
        await this.machine.init()
    }

    isReady() {
        return this.machine.isReady()
    }

    async load(program) {
        const response = await fetch(program.bin)
        const text = await response.text()

        const romWords = text
            .split(/\r?\n/)
            .map((line) => line.trim())
            .filter(Boolean)
            .map((line) => parseInt(line, 2))

        await this.machine.load({
            id: program.id,
            rom: Uint16Array.from(romWords),
        })

        return this.machine.getState()
    }

    async run() {
        this.machine.run()
        return this.machine.getState()
    }

    async stop() {
        this.machine.stop()
        return this.machine.getState()
    }

    async step() {
        this.machine.step()
        return this.machine.getState()
    }

    async reset() {
        this.machine.reset()
        return this.machine.getState()
    }

    async setKeyboard(value) {
        this.machine.setKeyboard(value)
        return this.machine.getState()
    }

    async getState() {
        return this.machine.getState()
    }

    async getFramebuffer() {
        return this.machine.getFramebuffer()
    }
}