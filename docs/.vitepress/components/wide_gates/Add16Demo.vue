<script setup lang="ts">
import "./wide_gate.css";
import { ref, computed } from "vue";

const a = ref(new Array(16).fill(0));
const b = ref(new Array(16).fill(0));

// Ripple-Carry Logic
const addition = computed(() => {
  const sum = new Array(16).fill(0);
  const carries = new Array(16).fill(0); // carryIn for each bit
  let c = 0;
  for (let i = 0; i < 16; i++) {
    carries[i] = c;
    const total = a.value[i] + b.value[i] + c;
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
  return unsigned >= 32768 ? unsigned - 65536 : unsigned;
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
      <div class="label" style="opacity: 0.5">c</div>
      <div class="bits" style="margin-left: -24px">
        <!-- Carry Out from Bit 15 (sits to the left of the 16-bit block) -->
        <div
          class="bit carry-bit ignored-bit"
          :class="{ active: addition.carryOut }"
        >
          <span class="bit-val">{{ addition.carryOut || "" }}</span>
        </div>

        <!-- Carries into bits 15...1 -->
        <div
          v-for="idx in indices"
          :key="'c' + idx"
          class="bit carry-bit"
          :class="{ active: addition.carries[idx] && idx > 0 }"
        >
          <span class="bit-val">{{
            idx > 0 && addition.carries[idx] ? 1 : ""
          }}</span>
        </div>
      </div>
      <div class="decimal-value" style="visibility: hidden; border: none">
        0
      </div>
    </div>

    <!-- Input A -->
    <div class="word-container">
      <div class="label">A</div>
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

    <!-- Input B -->
    <div class="word-container">
      <div class="label">B</div>
      <div class="bits">
        <div
          v-for="idx in indices"
          :key="'b' + idx"
          class="bit clickable"
          :class="{ active: b[idx] }"
          @click="b[idx] ^= 1"
        >
          <span class="bit-val">{{ b[idx] }}</span>
        </div>
      </div>
      <div class="decimal-value">{{ toDecimal(b) }}</div>
    </div>

    <div class="divider"></div>

    <!-- Output Sum -->
    <div class="word-container">
      <div class="label">sum</div>
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
