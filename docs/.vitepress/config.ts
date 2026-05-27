import { defineConfig } from "vitepress";
import { withMermaid } from "vitepress-plugin-mermaid";
import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

const __dirname = path.dirname(fileURLToPath(import.meta.url));

const readGrammar = (fileName: string) =>
  JSON.parse(
    fs.readFileSync(path.resolve(__dirname, `./syntax/${fileName}`), "utf8"),
  );

const ebnfGrammar = readGrammar("ebnf.tmLanguage.json");
const hdlGrammar = readGrammar("hdl.tmLanguage.json");

const ebnfLang = {
  id: "ebnf",
  scopeName: ebnfGrammar.scopeName ?? "source.bnf",
  ...ebnfGrammar,
};

const hdlLang = {
  id: "hdl",
  scopeName: hdlGrammar.scopeName ?? "source.hdl",
  ...hdlGrammar,
};

export default withMermaid(
  defineConfig({
    title: "Hack++",
    description:
      "A First Principles Computer, Assembler, VM, and Operating System",

    markdown: {
      lineNumbers: false,

      theme: {
        light: "github-light",
        dark: "github-dark",
      },

      languages: [ebnfLang, hdlLang],
    },

    themeConfig: {
      outline: { level: [2, 3] },
      nav: [
        { text: "Docs", link: "/" },
        { text: "GitHub", link: "https://github.com/josephhilby/HackPlusPlus" },
      ],
      sidebar: [
        {
          text: "Hack++ Docs",
          items: [
            { text: "Home", link: "/" },
            { text: "Introduction", link: "/introduction.md" },
            { text: "Boolean Logic Gates", link: "/primitives.md" },
            { text: "Combinational Circuits", link: "/combinationals.md" },
            { text: "Sequential Circuits", link: "/sequentials.md" },
            { text: "ALU Module", link: "/logic.md" },
            { text: "CPU Module", link: "/processor.md" },
            { text: "Memory Subsystem", link: "/06_memory.md" },
          ],
        },
      ],
    },
  }),
);
