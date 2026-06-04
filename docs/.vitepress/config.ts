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
  scopeName: ebnfGrammar.scopeName ?? "source.ebnf",
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
            { text: "Boolean Logic Gates", link: "/boolean.md" },
            { text: "Combinational Circuits", link: "/combinational.md" },
            { text: "Sequential Circuits", link: "/sequential.md" },
            { text: "Functional Modules", link: "/module.md" },
            { text: "Computer Subsystems", link: "/subsystem.md" },
            { text: "Assembly Language", link: "/assembly.md" },
            { text: "Bytecode Language", link: "/bytecode.md" },
            { text: "Jack Language", link: "/jack.md" },
            { text: "Operating System", link: "/os.md" },
          ],
        },
      ],
    },
  }),
);
