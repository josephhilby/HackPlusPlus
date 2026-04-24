export async function asmLoader(program) {
    if (!program?.asm) {
        throw new Error('Program is missing an asm path.')
    }

    const response = await fetch(program.asm)

    if (!response.ok) {
        throw new Error(`Failed to load ASM source: ${program.asm}`)
    }

    const text = await response.text()

    if (/<!doctype html>/i.test(text) || /<html/i.test(text)) {
        throw new Error(`ASM path resolved to HTML instead of source: ${program.asm}`)
    }

    return text
        .split(/\r?\n/)
        .map((line) => line.replace(/\r$/, ''))
}