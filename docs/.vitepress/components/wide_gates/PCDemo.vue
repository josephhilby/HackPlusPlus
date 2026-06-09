<script setup lang="ts">
import "./wide_gate.css";
import { ref, computed } from "vue";

const pc = ref(0);
const a = ref(0);
const d = ref(0);
const returnAddr = ref(0); // Hardware Link Register (LR)
const kern = ref(1); // Hardware context flag boots securely into Kernel (1)
const pendingReset = ref(false);

// Internal boot tracking (simulating the hardware R13 state)
const isColdBoot = ref(true);

// Isolated ROM Domain 1: Kernel Space
const kernelProgram = [
  { addr: 0, code: "CMP BOOT, 0" }, // 0x0000: Sets D to random garbage if cold boot, or 0 if trap
  { addr: 1, code: "JNZ 4" }, // 0x0001: Jumps to 4 if D != 0 (cold boot)
  { addr: 2, code: "ADD A, 1" }, // 0x0002: TRAP HANDLER: Increment register A to service request
  { addr: 3, code: "MGK" }, // 0x0003: Service complete. Trap return back to User Space!
  { addr: 4, code: "CLR BOOT" }, // 0x0004: COLD INIT: Flushes all data registers and drops flag
  { addr: 5, code: "MGK" }, // 0x0005: Launch initial execution environment out to User Space
];

// Isolated ROM Domain 0: User Space
const userProgram = [
  { addr: 0, code: "MOVE A, 1" }, // 0x0000: Set baseline
  { addr: 1, code: "MGK" }, // 0x0001: Deliberate Hardware Trap to Kernel Vector 0
  { addr: 2, code: "JMP 1" }, // 0x0002: Returned from Kernel loop! Jump back to the trap
];

const activeProgram = computed(() =>
  kern.value === 1 ? kernelProgram : userProgram,
);

const currentInstr = computed(
  () =>
    activeProgram.value.find((i) => i.addr === pc.value) || {
      addr: pc.value,
      code: "HALT",
    },
);

const shouldJump = computed(() => {
  if (kern.value === 1 && currentInstr.value.code === "JNZ 4") {
    return d.value !== 0; // Evaluates based on register D's status
  }
  if (kern.value === 0 && currentInstr.value.code === "JMP 1") {
    return true; // Standard infinite branch sequence
  }
  return false;
});

const flags = computed(() => {
  const reset = pendingReset.value;
  const isMgk = currentInstr.value.code === "MGK";
  const isJmp =
    currentInstr.value.code.startsWith("JMP") ||
    currentInstr.value.code.startsWith("JNZ");

  const load = !reset && isJmp && shouldJump.value;
  const jmp = !reset && isMgk;
  const inc = !reset && !load && !jmp;

  return { reset, load, jmp, inc, kern: kern.value };
});

const toHex = (val: number) =>
  "0x" + val.toString(16).toUpperCase().padStart(4, "0");

const step = () => {
  if (flags.value.reset) {
    kern.value = 1;
    pc.value = 0;
    a.value = 0;
    d.value = 0;
    returnAddr.value = 0;
    isColdBoot.value = true;
    pendingReset.value = false;
    return;
  }

  // Handle hardware tracking for CMP BOOT instruction mutations *before* moving PC
  if (currentInstr.value.code === "CMP BOOT, 0") {
    d.value = isColdBoot.value ? 0xb007 : 0x0000;
  }

  if (flags.value.jmp) {
    if (kern.value === 0) {
      // User -> Kernel Hardware System Call Intercept
      returnAddr.value = (pc.value + 1) & 0xffff;
      kern.value = 1;
      pc.value = 0; // Vector straight back to instruction zero
    } else {
      // Kernel Mode execution handling magic branches
      if (pc.value === 5) {
        // Initial Drop: Cold Boot drop to User Space 0x0000
        kern.value = 0;
        pc.value = 0;
      } else {
        // Standard Trap Return: Wake up at saved User link register address
        kern.value = 0;
        pc.value = returnAddr.value;
      }
    }
    return;
  }

  // Standard execution state updates
  const instr = currentInstr.value.code;
  if (instr.startsWith("MOVE A")) {
    a.value = parseInt(instr.split(",")[1].trim());
  } else if (instr === "ADD A, 1") {
    a.value = (a.value + 1) & 0xffff;
  } else if (instr === "CLR BOOT") {
    isColdBoot.value = false;
    a.value = 0; // Clear register fields to simulate clean environment initialization
    d.value = 0;
  }

  // Address Multiplexer Routing
  if (flags.value.load) {
    pc.value = currentInstr.value.code === "JNZ 4" ? 4 : 1;
  } else {
    pc.value = pc.value + 1;
  }
};

const triggerReset = () => {
  pendingReset.value = true;
};
</script>

<template>
  <div class="wide-gate-demo pc-vertical-layout">
    <div class="pc-row header-controls">
      <div class="action-buttons">
        <button class="pc-btn cycle" @click="step">Cycle</button>
        <button
          class="pc-btn reset"
          :class="{ pending: pendingReset }"
          @click="triggerReset"
        >
          Reset
        </button>
      </div>
      <div class="current-op">
        Executing: <code>{{ currentInstr.code }}</code>
      </div>
    </div>

    <div class="pc-row control-matrix">
      <div class="matrix-flag" :class="{ active: flags.reset }">reset</div>
      <div class="matrix-flag" :class="{ active: flags.load }">load</div>
      <div class="matrix-flag" :class="{ active: flags.jmp }">jmp</div>
      <div class="matrix-flag" :class="{ active: flags.inc }">inc</div>
      <div class="matrix-flag context" :class="{ active: flags.kern }">
        kern
      </div>
    </div>

    <div class="pc-row register-file">
      <div class="mini-reg">
        <span class="lbl">PC</span><code>{{ toHex(pc) }}</code>
      </div>
      <div class="mini-reg">
        <span class="lbl">LR</span><code>{{ toHex(returnAddr) }}</code>
      </div>
      <div class="mini-reg">
        <span class="lbl">A</span><code>{{ toHex(a) }}</code>
      </div>
      <div class="mini-reg">
        <span class="lbl">D</span><code>{{ toHex(d) }}</code>
      </div>
    </div>

    <div class="pc-row rom-wrapper">
      <div class="rom-bank" :class="{ dimmed: kern === 0 }">
        <div class="bank-label">Kernel Instruction Bank (ROM 1)</div>
        <div
          v-for="i in kernelProgram"
          :key="'k-' + i.addr"
          class="rom-line"
          :class="{ active: kern === 1 && pc === i.addr }"
        >
          <span class="addr">{{ toHex(i.addr) }}</span
          ><code class="code">{{ i.code }}</code>
        </div>
      </div>

      <div class="rom-bank" :class="{ dimmed: kern === 1 }">
        <div class="bank-label">User Instruction Bank (ROM 0)</div>
        <div
          v-for="i in userProgram"
          :key="'u-' + i.addr"
          class="rom-line"
          :class="{ active: kern === 0 && pc === i.addr }"
        >
          <span class="addr">{{ toHex(i.addr) }}</span
          ><code class="code">{{ i.code }}</code>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.pc-vertical-layout {
  display: flex;
  flex-direction: column;
  gap: 0.75rem;
  max-width: 100%;
}
.pc-row {
  display: flex;
  gap: 0.5rem;
  width: 100%;
}

/* Row 1: Flex Alignment & Isolated Groupings */
.header-controls {
  justify-content: space-between;
  align-items: center;
}
.action-buttons {
  display: flex;
  gap: 0.25rem; /* Matches the flag gap spacing perfectly */
  flex: 1;
}
.current-op {
  font-size: 0.85rem;
  color: var(--vp-c-text-2);
}

/* Row 2: 5-Column Logic Matrix */
.control-matrix {
  display: grid;
  grid-template-columns: repeat(5, 1fr);
  gap: 0.25rem;
  text-align: center;
}

/* Standardized Dimensional Sizing across Buttons and Flags */
.pc-btn {
  font-size: 0.7rem;
  padding: 0.15rem 0;
  border-radius: 4px;
  background: var(--vp-c-bg-alt);
  border: 1px solid var(--vp-c-divider);
  color: var(--vp-c-brand-1);
  text-transform: uppercase;
  font-weight: bold;
  cursor: pointer;
  line-height: normal;
  height: 24px; /* Fixed height match */
  width: 55px; /* Clean proportional button footprint */
  display: flex;
  align-items: center;
  justify-content: center;
}
.pc-btn:hover {
  background: var(--vp-c-bg-dim);
}
.pc-btn.reset.pending {
  background: #ef4444;
  color: white;
  border-color: #b91c1c;
}

.matrix-flag {
  font-size: 0.7rem;
  padding: 0.15rem 0;
  border-radius: 4px;
  background: var(--vp-c-bg-alt);
  border: 1px solid var(--vp-c-divider);
  color: var(--vp-c-text-3);
  text-transform: uppercase;
  font-weight: bold;
  height: 24px; /* Fixed height match */
  display: flex;
  align-items: center;
  justify-content: center;
}
.matrix-flag.active {
  background: var(--vp-c-brand-3);
  color: var(--vp-c-brand-1);
  border-color: var(--vp-c-brand-2);
}
.matrix-flag.context.active {
  background: #f59e0b;
  color: white;
  border-color: #d97706;
}

/* Register File Layout */
.register-file {
  display: grid;
  grid-template-columns: repeat(4, 1fr);
  text-align: center;
}
.mini-reg {
  border: 1px solid var(--vp-c-divider);
  padding: 0.25rem;
  border-radius: 4px;
  background: var(--vp-c-bg-alt);
}
.mini-reg .lbl {
  font-size: 0.75rem;
  display: block;
  color: var(--vp-c-text-2);
  font-weight: bold;
}

/* ROM Subsystems Layout */
.rom-wrapper {
  flex-direction: column;
  gap: 0.5rem;
}
.rom-bank {
  border: 1px solid var(--vp-c-divider);
  padding: 0.5rem;
  border-radius: 6px;
  transition: opacity 0.2s ease;
}
.rom-bank.dimmed {
  opacity: 0.35;
  background: var(--vp-c-bg-dim);
}
.bank-label {
  font-size: 0.75rem;
  font-weight: bold;
  margin-bottom: 0.25rem;
  color: var(--vp-c-brand);
}
.rom-line {
  display: flex;
  gap: 1rem;
  padding: 0.1rem 0.4rem;
  border-radius: 3px;
  font-size: 0.85rem;
}
.rom-line.active {
  background: var(--vp-c-brand-1);
  color: white;
}
</style>
