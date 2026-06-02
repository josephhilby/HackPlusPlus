<script setup lang="ts">
import "./wide_gate.css";
import { ref, computed } from "vue";

const props = withDefaults(
  defineProps<{
    type?: "half" | "full";
  }>(),
  {
    type: "full",
  },
);

const a = ref(0);
const b = ref(0);
const cin = ref(0);

const result = computed(() => {
  const carryInVal = props.type === "full" ? cin.value : 0;
  const total = a.value + b.value + carryInVal;
  return {
    sum: total % 2,
    carry: total >= 2 ? 1 : 0,
  };
});
</script>

<template>
  <div class="wide-gate-demo" style="max-width: 300px">
    <!-- Header -->
    <div class="word-container">
      <div class="label">bit</div>
      <div class="bits">
        <div v-if="type === 'full'" class="bit header">...</div>
        <div class="bit header">{{ type === "full" ? "2ⁿ⁺¹" : "2¹" }}</div>
        <div class="bit header">{{ type === "full" ? "2ⁿ" : "2⁰" }}</div>
        <div v-if="type === 'full'" class="bit header">...</div>
      </div>
    </div>

    <!-- Carry -->
    <div v-if="type === 'full'" class="word-container carry-row">
      <div class="label" style="opacity: 0.5">c</div>
      <div class="bits">
        <div class="bit header">...</div>
        <div
          class="bit carry-bit"
          :class="{ active: result.carry }"
          style="cursor: default; pointer-events: none"
        >
          <span class="bit-val">{{ result.carry }}</span>
        </div>
        <div
          class="bit clickable carry-bit"
          :class="{ active: cin }"
          @click="cin ^= 1"
        >
          <span class="bit-val">{{ cin }}</span>
        </div>
        <div class="bit header">...</div>
      </div>
    </div>

    <!-- Input A -->
    <div class="word-container">
      <div class="label">A</div>
      <div class="bits">
        <div v-if="type === 'full'" class="bit header">...</div>
        <div class="bit header">{{ type === "full" ? "0" : "" }}</div>
        <div class="bit clickable" :class="{ active: a }" @click="a ^= 1">
          <span class="bit-val">{{ a }}</span>
        </div>
        <div v-if="type === 'full'" class="bit header">...</div>
      </div>
    </div>

    <!-- Input B -->
    <div class="word-container">
      <div class="label">B</div>
      <div class="bits">
        <div v-if="type === 'full'" class="bit header">...</div>
        <div class="bit header">{{ type === "full" ? "0" : "" }}</div>
        <div class="bit clickable" :class="{ active: b }" @click="b ^= 1">
          <span class="bit-val">{{ b }}</span>
        </div>
        <div v-if="type === 'full'" class="bit header">...</div>
      </div>
    </div>

    <div class="divider"></div>

    <!-- Result -->
    <div class="word-container">
      <div class="label">{{ type === "full" ? "sum" : "out" }}</div>
      <div class="bits">
        <template v-if="type === 'full'">
          <div class="bit header">...</div>
          <div class="bit result" :class="{ active: result.carry }">
            <span class="bit-val">{{ result.carry }}</span>
          </div>
        </template>
        <template v-else>
          <div
            class="bit carry-bit"
            :class="{ active: result.carry }"
            style="cursor: default; pointer-events: none"
          >
            <span class="bit-val">{{ result.carry }}</span>
          </div>
        </template>
        <div class="bit result" :class="{ active: result.sum }">
          <span class="bit-val">{{ result.sum }}</span>
        </div>
        <div v-if="type === 'full'" class="bit header">...</div>
      </div>
    </div>
  </div>
</template>
