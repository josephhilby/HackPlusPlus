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
      <div class="col-label">Register Bank (ROM)</div>
      <div class="rom-container">
        <div
          v-for="instr in program"
          :key="'rom-' + instr.addr"
          class="rom-row"
          :class="{ active: pc === instr.addr }"
        >
          <span class="rom-addr">{{ toHex(instr.addr).slice(2) }}</span>
          <code class="rom-code">{{ instr.code }}</code>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.pc-container {
  display: grid;
  grid-template-columns: 100px 1fr 140px 160px;
  gap: 1.5rem;
  align-items: stretch;
  min-height: 140px;
}

.pc-col {
  display: flex;
  flex-direction: column;
  gap: 0.5rem;
}

.col-label {
  font-size: 0.65rem;
  text-transform: uppercase;
  font-weight: bold;
  color: var(--vp-c-text-3);
  letter-spacing: 0.05em;
}

.pc-btn {
  padding: 6px;
  border-radius: 4px;
  font-size: 0.8rem;
  font-weight: bold;
  cursor: pointer;
  border: 1px solid var(--vp-c-divider);
  background: var(--vp-c-bg);
  transition: all 0.2s;
}
.pc-btn.cycle:hover {
  border-color: var(--vp-c-brand);
  color: var(--vp-c-brand);
}
.pc-btn.reset.pending {
  background: var(--vp-c-danger-soft);
  border-color: var(--vp-c-danger-1);
  color: var(--vp-c-danger-1);
}

.instruction-box {
  flex: 1;
  background: var(--vp-c-bg);
  border: 1px solid var(--vp-c-divider);
  border-radius: 6px;
  position: relative;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  padding: 10px;
}
.instruction-box .addr {
  font-size: 0.65rem;
  color: var(--vp-c-text-3);
}
.instruction-box .code {
  font-weight: bold;
  color: var(--vp-c-brand-1);
  font-size: 1.1rem;
}

/* Decorative "CPU" bus lines */
.instruction-box::before,
.instruction-box::after {
  content: "";
  position: absolute;
  top: 50%;
  width: 15px;
  height: 1px;
  background: var(--vp-c-divider);
  box-shadow:
    0 -8px 0 var(--vp-c-divider),
    0 8px 0 var(--vp-c-divider);
}
.instruction-box::before {
  left: -15px;
}
.instruction-box::after {
  right: -15px;
}

.flags-row {
  display: flex;
  gap: 4px;
  justify-content: center;
}
.flag {
  font-size: 0.6rem;
  padding: 2px 6px;
  border-radius: 10px;
  background: var(--vp-c-bg-alt);
  color: var(--vp-c-text-3);
  text-transform: uppercase;
  border: 1px solid transparent;
}
.flag.active {
  background: var(--vp-c-brand-soft);
  color: var(--vp-c-brand);
  border-color: var(--vp-c-brand);
}

.reg {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 4px 8px;
  background: var(--vp-c-bg);
  border: 1px solid var(--vp-c-divider);
  border-radius: 4px;
}
.reg-id {
  font-size: 0.7rem;
  font-weight: bold;
  color: var(--vp-c-text-2);
}
.reg-val {
  font-size: 0.85rem;
  color: var(--vp-c-brand-1);
}

.rom-container {
  flex: 1;
  background: var(--vp-c-bg);
  border: 1px solid var(--vp-c-divider);
  border-radius: 6px;
  overflow: hidden;
  display: flex;
  flex-direction: column;
}

.pc-unit-box {
  border: 1px solid var(--vp-c-brand-soft);
  background: color-mix(in srgb, var(--vp-c-brand-soft) 5%, transparent);
  border-radius: 6px;
  padding: 6px;
  display: flex;
  flex-direction: column;
  gap: 6px;
}

.rom-row {
  display: flex;
  gap: 8px;
  padding: 2px 8px;
  font-size: 0.75rem;
  border-bottom: 1px solid var(--vp-c-divider);
  opacity: 0.6;
}

.rom-row:last-child {
  border-bottom: none;
}

.rom-row.active {
  opacity: 1;
  background: var(--vp-c-brand-soft);
  color: var(--vp-c-brand);
  font-weight: bold;
}

.rom-addr {
  color: var(--vp-c-text-3);
  font-family: var(--vp-font-family-mono);
}

@media (max-width: 640px) {
  .pc-container {
    grid-template-columns: 1fr;
  }
  .controls {
    flex-direction: row;
  }
  .controls button {
    flex: 1;
  }
}
</style>
