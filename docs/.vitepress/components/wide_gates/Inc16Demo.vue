<script setup lang="ts">
import "./wide_gate.css";
import { ref, computed } from "vue";

const a = ref(new Array(16).fill(0));

// Incrementor Logic
const addition = computed(() => {
  const sum = new Array(16).fill(0);
  const carries = new Array(16).fill(0);
  let c = 1; // Start with a carry-in of 1 (the increment)

  for (let i = 0; i < 16; i++) {
    carries[i] = c;
    const total = a.value[i] + c;
    sum[i] = total % 2;
    c = total >= 2 ? 1 : 0;
  }
  return { sum, carries, carryOut: c };
});

const toDecimal = (arr: number[]) => {
  let unsigned = 0;
  for (let i = 0; i < 16; i++) {
    if (arr[i]) unsigned += 1 << i;
  }
  return unsigned;
};

const indices = computed(() => {
  const arr = [];
  for (let i = 15; i >= 0; i--) arr.push(i);
  return arr;
});
</script>

<template>
  <div class="wide-gate-demo">
    <!-- Header indices -->
    <div class="word-container">
      <div class="label">bit</div>
      <div class="bits">
        <div v-for="idx in indices" :key="'idx' + idx" class="bit header">
          {{ idx }}
        </div>
      </div>
      <div
        class="decimal-value"
        style="visibility: hidden; border: none; background: transparent"
      >
        0
      </div>
    </div>

    <!-- Carry Chain -->
    <div class="word-container carry-row">
      <div class="label" style="opacity: 0.5">+1</div>
      <div class="bits" style="margin-left: -24px">
        <div
          class="bit carry-bit ignored-bit"
          :class="{ active: addition.carryOut }"
        >
          <span class="bit-val">{{ addition.carryOut || "" }}</span>
        </div>
        <div
          v-for="idx in indices"
          :key="'c' + idx"
          class="bit carry-bit"
          :class="{ active: addition.carries[idx] }"
        >
          <span class="bit-val">{{ addition.carries[idx] ? 1 : "" }}</span>
        </div>
      </div>
      <div class="decimal-value" style="visibility: hidden; border: none">
        0
      </div>
    </div>

    <!-- Input A -->
    <div class="word-container">
      <div class="label">in</div>
      <div class="bits">
        <div
          v-for="idx in indices"
          :key="'a' + idx"
          class="bit clickable"
          :class="{ active: a[idx] }"
          @click="a[idx] ^= 1"
        >
          <span class="bit-val">{{ a[idx] }}</span>
        </div>
      </div>
      <div class="decimal-value">{{ toDecimal(a) }}</div>
    </div>

    <div class="divider"></div>

    <!-- Output Sum -->
    <div class="word-container">
      <div class="label">out</div>
      <div class="bits">
        <div
          v-for="idx in indices"
          :key="'sum' + idx"
          class="bit result"
          :class="{ active: addition.sum[idx] }"
        >
          <span class="bit-val">{{ addition.sum[idx] }}</span>
        </div>
      </div>
      <div class="decimal-value">{{ toDecimal(addition.sum) }}</div>
    </div>
  </div>
</template>

<style scoped>
.carry-row {
  margin-top: -8px;
  margin-bottom: -4px;
  z-index: 10;
}
.carry-bit {
  border: none;
  font-size: 0.7rem;
  color: var(--vp-c-text-3);
  background: transparent;
  pointer-events: none;
  position: relative;
}
.carry-bit.active {
  color: var(--vp-c-warning-1);
  font-weight: bold;
}
.ignored-bit.active {
  color: var(--vp-c-danger-1);
}
</style>
