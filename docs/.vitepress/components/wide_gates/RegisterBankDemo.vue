<script setup lang="ts">
import "./wide_gate.css";
import { ref, computed } from "vue";

const dIn = ref(0x0000);
const load = ref(0);
const address = ref(0);
const registers = ref(new Array(8).fill(0x0000));

const toHex = (val: number) =>
  "0x" + val.toString(16).toUpperCase().padStart(4, "0");

const step = () => {
  registers.value = registers.value.map((oldVal, i) =>
    load.value === 1 && address.value === i ? dIn.value : oldVal,
  );
};

const setNibble = (nibbleIdx: number) => {
  const shift = nibbleIdx * 4;
  let val = (dIn.value >> shift) & 0xf;
  val = (val + 1) % 16;
  const mask = ~(0xf << shift);
  dIn.value = (dIn.value & mask) | (val << shift);
};

const toggleAddrBit = (bitIdx: number) => {
  address.value ^= 1 << bitIdx;
};

const dInHexDigits = computed(() => {
  return dIn.value.toString(16).toUpperCase().padStart(4, "0").split("");
});

const addrBits = computed(() => {
  return address.value.toString(2).padStart(3, "0").split("").map(Number);
});
</script>

<template>
  <div class="wide-gate-demo bank-container">
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

    <!-- Column 2: Input/Address at time t -->
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

      <div class="reg" style="width: 140px; padding: 4px 6px">
        <span class="reg-id">ADDR</span>
        <div class="addr-input">
          <code
            v-for="(bit, i) in addrBits"
            :key="i"
            class="reg-val addr-bit"
            @click="toggleAddrBit(2 - i)"
          >
            {{ bit }}
          </code>
        </div>
      </div>
    </div>

    <!-- Column 3: Register Bank at time t+1 -->
    <div class="pc-col registers">
      <div class="col-label">Bank (t+1)</div>
      <div class="pc-unit-box">
        <div
          v-for="(val, i) in registers"
          :key="i"
          class="reg bank-row"
          :class="{ targeted: address === i }"
        >
          <span class="reg-id">OUT{{ i }}</span>
          <code class="reg-val small">{{ toHex(val) }}</code>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.bank-container {
  display: grid;
  grid-template-columns: 100px 150px 180px;
  gap: 1.5rem;
  align-items: stretch;
  min-height: 280px;
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
  flex-direction: column;
  align-items: center;
  line-height: 1.2;
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

.cycle-formula {
  display: flex;
  align-items: center;
  gap: 4px;
  font-size: 0.65rem;
  font-family: var(--vp-font-family-mono);
  color: var(--vp-c-brand-1);
  margin-bottom: 2px;
}
.cycle-formula .arrow {
  font-weight: bold;
  opacity: 0.8;
}

.pc-unit-box {
  border: 1px solid var(--vp-c-brand-soft);
  background: color-mix(in srgb, var(--vp-c-brand-soft) 5%, transparent);
  border-radius: 6px;
  padding: 6px;
  display: flex;
  flex-direction: column;
  gap: 4px;
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

.bank-row {
  border-color: transparent;
  background: transparent;
  padding: 2px 6px;
  opacity: 0.6;
  transition: all 0.2s;
}

.bank-row.targeted {
  opacity: 1;
  background: var(--vp-c-bg);
  border-color: var(--vp-c-brand-soft);
  box-shadow: var(--vp-shadow-1);
}

.hex-input {
  display: flex;
  font-family: var(--vp-font-family-mono);
}

.addr-input {
  display: flex;
  font-family: var(--vp-font-family-mono);
  gap: 2px;
}
.hex-digit,
.addr-bit {
  cursor: pointer;
  user-select: none;
  transition: all 0.1s;
}
.hex-digit:hover,
.addr-bit:hover {
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
.reg-val.small {
  font-size: 0.85rem;
}

@media (max-width: 640px) {
  .bank-container {
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
