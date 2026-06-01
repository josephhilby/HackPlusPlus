<script setup lang="ts">
import "./wide_gate.css";
import { ref, computed } from "vue";

const dIn = ref(0x0000);
const load = ref(0);
const regOut = ref<number | null>(null);

const toHex = (val: number | null) =>
  val === null
    ? "----"
    : "0x" + val.toString(16).toUpperCase().padStart(4, "0");

const step = () => {
  if (load.value === 1) {
    regOut.value = dIn.value;
  }
};

const setNibble = (nibbleIdx: number) => {
  const shift = nibbleIdx * 4;
  let val = (dIn.value >> shift) & 0xf;
  val = (val + 1) % 16;
  const mask = ~(0xf << shift);
  dIn.value = (dIn.value & mask) | (val << shift);
};

const dInHexDigits = computed(() => {
  return dIn.value.toString(16).toUpperCase().padStart(4, "0").split("");
});
</script>

<template>
  <div class="wide-gate-demo register-container" style="max-width: 500px">
    <!-- Column 1: Controls -->
    <div class="pc-col controls">
      <div class="col-label">Manual</div>
      <button class="pc-btn cycle" @click="step">
        <span class="btn-text">Cycle</span>
      </button>
      <button
        class="pc-btn load-toggle"
        :class="{ active: load }"
        @click="load ^= 1"
      >
        LOAD
      </button>
    </div>

    <!-- Column 2: Input at time t -->
    <div class="pc-col inputs">
      <div class="col-label">State (t)</div>
      <div class="reg" style="width: 140px; padding: 4px 6px">
        <span class="reg-id">IN</span>
        <div class="hex-input">
          <code
            v-for="(digit, i) in dInHexDigits"
            :key="i"
            class="reg-val hex-digit"
            style="cursor: pointer"
            @click="setNibble(3 - i)"
          >
            <span v-if="i === 0" class="prefix">0x</span>{{ digit }}
          </code>
        </div>
      </div>
    </div>

    <!-- Column 3: State at time t+1 -->
    <div class="pc-col registers">
      <div class="col-label">State (t+1)</div>
      <div class="pc-unit-box">
        <div class="flags-row">
          <div class="flag" :class="{ active: load }">load</div>
        </div>
        <div class="reg">
          <span class="reg-id">OUT</span>
          <code class="reg-val">{{ toHex(regOut) }}</code>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.register-container {
  display: grid;
  grid-template-columns: 100px 140px 160px;
  gap: 1.5rem;
  align-items: stretch;
  min-height: 120px;
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
  padding: 8px 6px;
  border-radius: 4px;
  font-size: 0.8rem;
  font-weight: bold;
  cursor: pointer;
  border: 1px solid var(--vp-c-divider);
  background: var(--vp-c-bg);
  transition: all 0.2s;
  display: flex;
  justify-content: center;
  align-items: center;
}

.pc-btn:hover {
  border-color: var(--vp-c-brand);
  color: var(--vp-c-brand);
}

.pc-btn.load-toggle.active {
  background: var(--vp-c-brand-soft);
  border-color: var(--vp-c-brand);
  color: var(--vp-c-brand);
}

.pc-unit-box {
  border: 1px solid var(--vp-c-brand-soft);
  background: color-mix(in srgb, var(--vp-c-brand-soft) 5%, transparent);
  border-radius: 6px;
  padding: 8px;
  display: flex;
  flex-direction: column;
  gap: 8px;
  flex: 1;
  justify-content: center;
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

.hex-input {
  display: flex;
  font-family: var(--vp-font-family-mono);
}

.hex-digit {
  cursor: pointer;
  user-select: none;
  padding: 0 1px;
  border-radius: 2px;
  transition: all 0.1s;
}

.hex-digit:hover {
  background: var(--vp-c-brand-soft);
  color: var(--vp-c-brand);
}

.prefix {
  opacity: 0.5;
}

.reg-id {
  font-size: 0.7rem;
  font-weight: bold;
  color: var(--vp-c-text-2);
}

.reg-val {
  font-size: 1.1rem;
  color: var(--vp-c-brand-1);
}

@media (max-width: 640px) {
  .register-container {
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
