<script setup lang="ts">
import "./wide_gate.css";
import { ref, computed } from "vue";

const pc = ref(0);
const a = ref(0);
const d = ref(0);
const pendingReset = ref(false);

const program = [
  {
    addr: 0,
    code: "LOAD A, 0",
    action: () => {
      a.value = 0;
    },
  },
  {
    addr: 1,
    code: "LOAD A, 1",
    action: () => {
      a.value = 1;
    },
  },
  {
    addr: 2,
    code: "MOVE D, A",
    action: () => {
      d.value = a.value;
    },
  },
  {
    addr: 3,
    code: "LOAD A, 2",
    action: () => {
      a.value = 2;
    },
  },
  {
    addr: 4,
    code: "ADD A, D",
    action: () => {
      d.value = (d.value + a.value) & 0xffff;
    },
  },
  {
    addr: 5,
    code: "JNZ 1",
    action: () => {
      /* jump logic handled in sequencer */
    },
  },
];

const currentInstr = computed(
  () =>
    program.find((i) => i.addr === pc.value) || {
      addr: pc.value,
      code: "HALT",
    },
);

// PC Logic Sequencer (Flags for the CURRENT cycle determining NEXT state)
const flags = computed(() => {
  const reset = pendingReset.value;
  const load = !reset && currentInstr.value.code === "JNZ 1" && d.value !== 0;
  const inc = !reset && !load;
  return { reset, load, inc };
});

const toHex = (val: number) =>
  "0x" + val.toString(16).toUpperCase().padStart(4, "0");

const step = () => {
  if (flags.value.reset) {
    pc.value = 0;
    pendingReset.value = false;
  } else if (flags.value.load) {
    pc.value = 1; // JNZ 1
  } else {
    // Execute the data side of the instruction before moving the PC
    currentInstr.value.action?.();
    pc.value = pc.value + 1;
  }
};

const triggerReset = () => {
  pendingReset.value = true;
};
</script>

<template>
  <div class="wide-gate-demo pc-container">
    <!-- Part 1: User Controls -->
    <div class="pc-col controls">
      <div class="col-label">User Input</div>
      <button class="pc-btn cycle" @click="step">Cycle</button>
      <button
        class="pc-btn reset"
        :class="{ pending: pendingReset }"
        @click="triggerReset"
      >
        Reset
      </button>
    </div>

    <!-- Part 2: Execution & Flags -->
    <div class="pc-col execution">
      <div class="col-label">Execution</div>
      <div class="instruction-box">
        <span class="addr">{{ toHex(currentInstr.addr) }}</span>
        <code class="code">{{ currentInstr.code }}</code>
      </div>
    </div>

    <!-- Part 3: Registers -->
    <div class="pc-col registers">
      <div class="col-label">Registers</div>
      <div class="pc-unit-box">
        <div class="flags-row">
          <div class="flag" :class="{ active: flags.reset }">reset</div>
          <div class="flag" :class="{ active: flags.load }">load</div>
          <div class="flag" :class="{ active: flags.inc }">inc</div>
        </div>
        <div class="reg">
          <span class="reg-id">PC</span>
          <code class="reg-val">{{ toHex(pc) }}</code>
        </div>
      </div>
      <div class="reg">
        <span class="reg-id">A</span>
        <code class="reg-val">{{ toHex(a) }}</code>
      </div>
      <div class="reg">
        <span class="reg-id">D</span>
        <code class="reg-val">{{ toHex(d) }}</code>
      </div>
    </div>

    <!-- Part 4: ROM -->
    <div class="pc-col rom">
      <div class="col-label">Instruction Registers</div>
      <div class="rom-container">
        <div
          v-for="instr in program"
          :key="'rom-' + instr.addr"
          class="rom-row"
          :class="{ active: pc === instr.addr }"
        >
          <span class="rom-addr">{{ toHex(instr.addr) }}</span>
          <code class="rom-code">{{ instr.code }}</code>
        </div>
      </div>
    </div>
  </div>
</template>
