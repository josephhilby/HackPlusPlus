export const DEFAULT_GRAPH_DATA = {
  root: {
    type: "system",
    name: "Hack++",
    details: [
      { prefix: "", value: "16-Bit Fixed-Instruction Model" },
      { prefix: "Subsystems:", value: ["Hardware", "Software"] },
    ],
  },
  children: [
    {
      type: "subsystem",
      name: "Hardware",
      details: [
        { prefix: "", value: "Harvard Model" },
        { prefix: "Modules:", value: ["CPU", "RAM", "ROM"] },
      ],
    },
    {
      type: "subsystem",
      name: "Software",
      details: [
        {
          prefix: "",
          value: "User-Kernel Model",
        },
        { prefix: "Domains:", value: ["Application", "OS"] },
      ],
    },
  ],
};
