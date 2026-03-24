import { defineConfig } from 'vitepress'
import { withMermaid } from 'vitepress-plugin-mermaid'
import type MarkdownIt from 'markdown-it'
import fs from 'node:fs'
import path from 'node:path'
import { fileURLToPath } from 'node:url'
import { createHighlighter } from 'shiki'

const __dirname = path.dirname(fileURLToPath(import.meta.url))

const ebnfGrammar = JSON.parse(
    fs.readFileSync(
        path.resolve(__dirname, './syntax/ebnf.tmLanguage.json'),
        'utf8'
    )
)

const ebnfLang = {
    name: 'ebnf',
    aliases: ['ebnf'],
    scopeName: ebnfGrammar.scopeName ?? 'source.bnf',
    ...ebnfGrammar
}

const ebnfHighlighter = await createHighlighter({
    themes: ['github-light', 'github-dark'],
    langs: [ebnfLang]
})

export default withMermaid(defineConfig({
    title: 'Hack++',
    description: 'A First Principles Computer, Assembler, VM, and Operating System',

    markdown: {
        lineNumbers: false,
        languageAlias: {
            hdl: 'javascript'
        },


        config(md: MarkdownIt) {

            const defaultFence =
                md.renderer.rules.fence ??
                ((tokens, idx, options, env, self) =>
                    self.renderToken(tokens, idx, options))

            md.renderer.rules.fence = (tokens, idx, options, env, self) => {
                const token = tokens[idx]
                const info = (token.info || '').trim()
                const lang = info.split(/\s+/)[0]

                if (lang !== 'ebnf') {
                    return defaultFence(tokens, idx, options, env, self)
                }

                const html = ebnfHighlighter.codeToHtml(token.content, {
                    lang: 'ebnf',
                    themes: {
                        light: 'github-light',
                        dark: 'github-dark'
                    }
                })

                return [
                    '<div class="language-ebnf vp-adaptive-theme">',
                    '<button title="Copy Code" class="copy"></button>',
                    '<span class="lang">ebnf</span>',
                    html,
                    '</div>'
                ].join('')
            }
        }
    },

    themeConfig: {
        outline: {
            level: [2, 3]
        },

        nav: [
            { text: 'Docs', link: '/' },
            { text: 'GitHub', link: 'https://github.com/josephhilby/HackPlusPlus' }
        ],

        sidebar: [
            {
                text: 'Hack++ Docs',
                items: [
                    { text: 'Home', link: '/' },
                    { text: 'Index', link: '/00_root.md' },
                    { text: 'Boolean Logic Gates', link: '/01_primitives.md' },
                    { text: 'Routing & Arithmetic Circuits', link: '/02_routing_arithmetic.md' },
                    { text: 'Sequential Chips', link: '/05_sequential.md' },
                    { text: 'Memory Subsystem', link: '/06_memory.md' },
                    { text: 'ALU & CPU Subsystem', link: '/07_processor.md' },
                    // { text: 'Computer', link: '/00_root.md' },
                    // { text: 'Instruction Set Architecture', link: '/00_root.md' },
                    // { text: 'Virtual Machine', link: '/00_root.md' },
                    // { text: 'High-Level Language', link: '/00_root.md' },
                ]
            }
        ]
    }
}))