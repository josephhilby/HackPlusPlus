<script setup lang="ts">
import "./wide_gate.css";
import { ref, computed } from "vue";

const props = defineProps<{
  ways: 4 | 8;
}>();

const selSize = Math.log2(props.ways);

// State
const inWord = ref(new Array(16).fill(0));
const sel = ref(new Array(selSize).fill(0));

// Logic
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

const results = computed(() => {
  return Array.from({ length: props.ways }, (_, i) =>
    i === selVal.value ? [...inWord.value] : new Array(16).fill(0),
  );
});

const indices = computed(() => {
  const arr = [];
  for (let i = 15; i >= 0; i--) arr.push(i);
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
        <div v-for="i in 16" :key="i" class="bit header">
          {{ 16 - i }}
        </div>
      </div>
      <div
        class="decimal-value"
        style="visibility: hidden; border: none; background: transparent"
      >
        0
      </div>
    </div>

    <!-- Input Bit (Padded to bit 0) -->
    <div class="word-container">
      <div class="label">in</div>
      <div class="bits">
        <div
          v-for="idx in indices"
          :key="'in-b' + idx"
          class="bit clickable"
          :class="{ active: inWord[idx] }"
          @click="inWord[idx] ^= 1"
        >
          <span class="bit-val">{{ inWord[idx] }}</span>
        </div>
      </div>
      <div class="decimal-value">{{ toDecimal(inWord) }}</div>
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

    <!-- Outputs -->
    <div
      v-for="(word, wIdx) in results"
      :key="'out' + wIdx"
      class="word-container"
    >
      <div class="label">{{ labels[wIdx] }}</div>
      <div class="bits">
        <div
          v-for="idx in indices"
          :key="'out' + wIdx + 'b' + idx"
          class="bit result"
          :class="{ active: word[idx] }"
        >
          <span class="bit-val">{{ word[idx] }}</span>
        </div>
      </div>
      <div class="decimal-value">{{ toDecimal(word) }}</div>
    </div>
  </div>
</template>

<style scoped>
.bits {
  justify-content: flex-start;
}
.label {
  flex-shrink: 0;
}
.decimal-value {
  margin-left: auto;
}
</style>
