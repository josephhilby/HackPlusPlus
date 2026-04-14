export default class MachineClient {
    constructor() {
        this.initialized = false
    }

    async init() {
        // Later:
        // - load wasm module
        // - initialize machine memory/state
        this.initialized = true
    }

    isReady() {
        return this.initialized
    }

    async getFramebuffer() {
        if (!this.initialized) {
            throw new Error('Machine is not initialized.')
        }

        // Placeholder framebuffer:
        // 512 * 256 bits = 131072 bits = 16384 bytes
        return new ArrayBuffer(16384)
    }

    async reset() {
        if (!this.initialized) {
            throw new Error('Machine is not initialized.')
        }
    }

    async step() {
        if (!this.initialized) {
            throw new Error('Machine is not initialized.')
        }
    }

    async runCycles(_count) {
        if (!this.initialized) {
            throw new Error('Machine is not initialized.')
        }
    }
}