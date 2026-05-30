<script setup lang="ts">
import "./wide_gate.css";
import { ref, computed } from "vue";

const props = defineProps<{
  ways: 2 | 4 | 8;
}>();

const inputSize = 16;
const selSize = Math.log2(props.ways);

// State for inputs (A, B, C, D...)
const inputs = ref(
  Array.from({ length: props.ways }, () => new Array(inputSize).fill(0)),
);

// State for selector bits
const sel = ref(new Array(selSize).fill(0));

// Helpers
const bitsToUnsigned = (arr: number[]) => {
  let val = 0;
  for (let i = 0; i < arr.length; i++) if (arr[i]) val += 1 << i;
  return val;
};

const toDecimal = (arr: number[]) => {
  let unsigned = bitsToUnsigned(arr);
  return unsigned >= 32768 ? unsigned - 65536 : unsigned;
};

const selVal = computed(() => bitsToUnsigned(sel.value));

const out = computed(() => inputs.value[selVal.value] || new Array(16).fill(0));

const indices = computed(() => {
  const arr = [];
  for (let i = inputSize - 1; i >= 0; i--) arr.push(i);
  return arr;
});

const labels = ["A", "B", "C", "D", "E", "F", "G", "H"];
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

    <!-- Input Words -->
    <div
      v-for="(word, wIdx) in inputs"
      :key="'word' + wIdx"
      class="word-container"
    >
      <div class="label">{{ labels[wIdx] }}</div>
      <div class="bits">
        <div
          v-for="idx in indices"
          :key="'w' + wIdx + 'b' + idx"
          class="bit clickable"
          :class="{ active: word[idx] }"
          @click="word[idx] ^= 1"
        >
          <span class="bit-val">{{ word[idx] }}</span>
        </div>
      </div>
      <div class="decimal-value">{{ toDecimal(word) }}</div>
    </div>

    <!-- Selector -->
    <div class="word-container">
      <div class="label">sel</div>
      <div class="bits">
        <template v-for="i in 16" :key="'sel-pad' + i">
          <div
            v-if="17 - i > selSize"
            class="bit header"
            style="border: none"
          ></div>
          <div
            v-else
            class="bit clickable"
            :class="{ active: sel[16 - i] }"
            @click="sel[16 - i] ^= 1"
          >
            <span class="bit-val">{{ sel[16 - i] }}</span>
          </div>
        </template>
      </div>
      <div class="decimal-value" style="color: var(--vp-c-text-2)">
        {{ selVal }}
      </div>
    </div>

    <div class="divider"></div>

    <!-- Output -->
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

<style scoped>
.bits {
  justify-content: flex-start;
}
.decimal-value {
  margin-left: auto;
}
</style>
