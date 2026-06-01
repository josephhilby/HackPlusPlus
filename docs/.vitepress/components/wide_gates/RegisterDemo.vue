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
      <div class="reg" style="width: 140px">
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
