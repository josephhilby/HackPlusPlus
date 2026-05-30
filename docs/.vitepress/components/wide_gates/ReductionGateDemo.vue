<script setup lang="ts">
import "./wide_gate.css";
import { ref, computed } from "vue";

const a = ref(new Array(8).fill(0));
const out = computed(() => (a.value.some((v) => v === 1) ? 1 : 0));
</script>

<template>
  <div class="wide-gate-demo">
    <div class="word-container">
      <div class="label">in</div>
      <div class="bits">
        <div
          v-for="i in 8"
          :key="'a' + (8 - i)"
          class="bit clickable"
          :class="{ active: a[8 - i] }"
          @click="a[8 - i] ^= 1"
        >
          <span class="bit-val">{{ a[8 - i] }}</span>
        </div>
      </div>
      <div class="decimal-value" style="visibility: hidden; border: none">
        0
      </div>
    </div>

    <div class="divider"></div>

    <div class="word-container">
      <div class="label">out</div>
      <div class="bits">
        <!-- Spacers to align the single output bit to the right (Bit 0 position) -->
        <div
          v-for="i in 7"
          :key="'sp' + i"
          class="bit header"
          style="border: none"
        ></div>
        <div class="bit result" :class="{ active: out }">
          <span class="bit-val">{{ out }}</span>
        </div>
      </div>
      <div class="decimal-value" style="visibility: hidden; border: none">
        0
      </div>
    </div>
  </div>
</template>

<style scoped>
/* Alignment fixes to ensure labels and bits stack correctly */
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
