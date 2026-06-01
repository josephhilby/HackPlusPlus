<script setup lang="ts">
import "./wide_gate.css";
import { ref } from "vue";

const dIn = ref(0);
const dffOut = ref<string>("-");

const step = () => {
  dffOut.value = dIn.value.toString();
};
</script>

<template>
  <div class="wide-gate-demo dff-container">
    <!-- Column 1: Controls -->
    <div class="pc-col controls">
      <div class="col-label">Manual</div>
      <button class="pc-btn cycle" @click="step">
        <div class="cycle-formula">
          <span>(t)</span>
          <span class="arrow">&rarr;</span>
          <span>(t+1)</span>
        </div>
        <span class="btn-text">Cycle</span>
      </button>
    </div>

    <!-- Column 2: Input at time t -->
    <div class="pc-col inputs">
      <div class="col-label">State (t)</div>
      <div class="reg clickable" @click="dIn ^= 1">
        <span class="reg-id">IN</span>
        <code class="reg-val">{{ dIn }}</code>
      </div>
    </div>

    <!-- Column 3: State at time t+1 -->
    <div class="pc-col registers">
      <div class="col-label">State (t+1)</div>
      <div class="reg">
        <span class="reg-id">OUT</span>
        <code class="reg-val">{{ dffOut }}</code>
      </div>
    </div>
  </div>
</template>
