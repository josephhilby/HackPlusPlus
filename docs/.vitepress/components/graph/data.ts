export const DEFAULT_GRAPH_DATA = {
  root: {
    type: "system",
    name: "Hack++",
    details: [
      { prefix: "", value: "16-Bit Fixed-Instruction Arch" },
      { prefix: "Subsystems:", value: ["Hardware", "Software"] },
    ],
  },
  children: [
    {
      type: "subsystem",
      name: "Hardware",
      details: [
        { prefix: "", value: "Harvard Model" },
        { prefix: "Modules:", value: ["Mem", "Inst", "CPU"] },
      ],
    },
    {
      type: "subsystem",
      name: "Software",
      details: [
        {
          prefix: "",
          value: "User-Kernel Split",
        },
        { prefix: "Domains:", value: ["User App", "OS"] },
      ],
    },
  ],
};

export const HARDWARE_GRAPH_DATA = {
  root: {
    type: "subsystem",
    name: "Hardware",
    details: [
      { prefix: "", value: "Harvard Model" },
      { prefix: "Modules:", value: ["Mem", "Inst", "CPU"] },
    ],
  },
  children: [
    {
      type: "module",
      name: "Memory",
      details: [
        {
          prefix: "",
          value: "Memory Mapping",
        },
        { prefix: "Components:", value: ["RAM16K", "RAM8K", "Reg."] },
      ],
    },
    {
      type: "module",
      name: "Instruction",
      details: [
        { prefix: "", value: "Privilege Levels" },
        { prefix: "Components:", value: ["ROM32K", "ROM16K"] },
      ],
    },
    {
      type: "module",
      name: "CPU",
      details: [
        { prefix: "", value: "Single-Cycle Deterministic" },
        { prefix: "Components:", value: ["PC", "ALU", "Register"] },
      ],
    },
  ],
};

export const SOFTWARE_GRAPH_DATA = {
  root: {
    type: "subsystem",
    name: "Software",
    details: [
      { prefix: "", value: "User-Kernel Split" },
      { prefix: "Domains:", value: ["User App", "OS"] },
    ],
  },
  children: [
    {
      type: "domain",
      name: "User App",
      details: [{ prefix: "", value: "Application Programs" }],
    },
    {
      type: "domain",
      name: "OS",
      details: [{ prefix: "", value: "Operating System" }],
    },
  ],
};
