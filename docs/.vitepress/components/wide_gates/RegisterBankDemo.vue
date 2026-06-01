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
      <div class="reg" style="width: 140px;">
        <span class="reg-id">IN</span>
        <div class="hex-input">
          <code
            v-for="(digit, i) in dInHexDigits"
            :key="i"
            class="reg-val hex-digit"
            @click="setNibble(3 - i)"
          >
            <span v-if="i === 0" class="prefix">0x</span>{{ digit }}
          </code>
        </div>
      </div>

      <div class="reg" style="width: 140px;">
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
