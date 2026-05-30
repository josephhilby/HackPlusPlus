<script setup lang="ts">
import "./wide_gate.css";
import { ref, computed } from "vue";

const props = defineProps({
  gate: {
    type: String,
    required: true, // 'Not16', 'And16', 'Or16'
  },
});

const inputSize = 16;
const isBinary = props.gate === "And16" || props.gate === "Or16";

// State stored as LSB at index 0, MSB at index size-1
const a = ref(new Array(inputSize).fill(0));
const b = ref(new Array(inputSize).fill(0));

// Computation logic
const out = computed(() => {
  if (props.gate === "Not16") return a.value.map((v) => v ^ 1);
  if (props.gate === "And16") return a.value.map((v, i) => v & b.value[i]);
  if (props.gate === "Or16") return a.value.map((v, i) => v | b.value[i]);
  return [];
});

// Generate indices from MSB to LSB for display
const indices = computed(() => {
  const arr = [];
  for (let i = inputSize - 1; i >= 0; i--) arr.push(i);
  return arr;
});

// Binary array to Signed 16-bit Integer
const toDecimal = (arr: number[]) => {
  let unsigned = 0;
  for (let i = 0; i < 16; i++) {
    if (arr[i]) unsigned += Math.pow(2, i);
  }
  // Return as signed 16-bit
  return unsigned >= 32768 ? unsigned - 65536 : unsigned;
};
</script>

<template>
  <div class="wide-gate-demo">
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

    <div v-if="isBinary" class="word-container">
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

    <div class="word-container">
      <div class="label">out</div>
      <div class="bits">
        <div
          v-for="idx in indices"
          :key="'out' + idx"
          class="bit result"
          :class="{ active: out[idx] }"
        >
          <span class="bit-val">{{ out[idx] }}</span>
        </div>
      </div>
      <div class="decimal-value">{{ toDecimal(out) }}</div>
    </div>
  </div>
</template>
