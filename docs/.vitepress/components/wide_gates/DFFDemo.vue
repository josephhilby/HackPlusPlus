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
  <div class="wide-gate-demo dff-container" style="max-width: 500px">
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

<style scoped>
.dff-container {
  display: grid;
  grid-template-columns: 100px 140px 160px;
  gap: 1.5rem;
  align-items: stretch;
  min-height: 120px;
}

.pc-col {
  display: flex;
  flex-direction: column;
  gap: 0.5rem;
}

.col-label {
  font-size: 0.65rem;
  text-transform: uppercase;
  font-weight: bold;
  color: var(--vp-c-text-3);
  letter-spacing: 0.05em;
}

.pc-btn {
  padding: 6px;
  border-radius: 4px;
  font-size: 0.8rem;
  font-weight: bold;
  cursor: pointer;
  border: 1px solid var(--vp-c-divider);
  background: var(--vp-c-bg);
  transition: all 0.2s;
  display: flex;
  flex-direction: column;
  align-items: center;
  line-height: 1.2;
  padding: 8px 6px;
}

.cycle-formula {
  display: flex;
  align-items: center;
  gap: 4px;
  font-size: 0.65rem;
  font-family: var(--vp-font-family-mono);
  color: var(--vp-c-brand-1);
  margin-bottom: 2px;
}

.cycle-formula .arrow {
  font-weight: bold;
  opacity: 0.8;
}
.pc-btn.cycle:hover {
  border-color: var(--vp-c-brand);
  color: var(--vp-c-brand);
}

.reg {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 4px 8px;
  background: var(--vp-c-bg);
  border: 1px solid var(--vp-c-divider);
  border-radius: 4px;
}
.reg.clickable {
  cursor: pointer;
  user-select: none;
}
.reg.clickable:hover {
  border-color: var(--vp-c-brand);
}
.reg-id {
  font-size: 0.7rem;
  font-weight: bold;
  color: var(--vp-c-text-2);
}
.reg-val {
  font-size: 1.1rem;
  color: var(--vp-c-brand-1);
}

@media (max-width: 640px) {
  .dff-container {
    grid-template-columns: 1fr;
  }
  .controls {
    flex-direction: row;
  }
  .controls button {
    flex: 1;
  }
}
</style>
