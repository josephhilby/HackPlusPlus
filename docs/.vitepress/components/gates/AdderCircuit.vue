<script setup lang="ts">
import "./gate.css";
import { ref, computed } from "vue";
import TruthTable, { type TruthTableRow } from "../table/TruthTable.vue";

import {
  VIEW_H,
  VIEW_W,
  VIEW_GATE_OFFSET_X,
  VIEW_GATE_OFFSET_Y,
  LABEL_IN_X,
  LABEL_A_Y,
  LABEL_B_Y,
  LABEL_OUT_X,
  MID_Y,
  ADDER_SUM_Y,
  ADDER_CARRY_Y,
} from "./parts/gateGeometry";

import AdderBody from "./parts/AdderBody.vue";
import GateLabel from "./parts/GateLabel.vue";

const props = withDefaults(
  defineProps<{
    type?: "half" | "full";
  }>(),
  {
    type: "full",
  },
);

type AdderRow = TruthTableRow & {
  a: number;
  b: number;
  c?: number;
  sum: number;
  carry: number;
  rowClass: string;
};

const hovered = ref<AdderRow | null>(null);

const isFull = computed(() => props.type === "full");

const columns = computed(() =>
  isFull.value ? ["a", "b", "c", "sum", "carry"] : ["a", "b", "sum", "carry"],
);

const rows = computed<AdderRow[]>(() => {
  if (isFull.value) {
    return [
      { a: 0, b: 0, c: 0, sum: 0, carry: 0, rowClass: "truth-false" },
      { a: 0, b: 0, c: 1, sum: 1, carry: 0, rowClass: "truth-true" },
      { a: 0, b: 1, c: 0, sum: 1, carry: 0, rowClass: "truth-true" },
      { a: 0, b: 1, c: 1, sum: 0, carry: 1, rowClass: "truth-true" },
      { a: 1, b: 0, c: 0, sum: 1, carry: 0, rowClass: "truth-true" },
      { a: 1, b: 0, c: 1, sum: 0, carry: 1, rowClass: "truth-true" },
      { a: 1, b: 1, c: 0, sum: 0, carry: 1, rowClass: "truth-true" },
      { a: 1, b: 1, c: 1, sum: 1, carry: 1, rowClass: "truth-true" },
    ];
  } else {
    return [
      { a: 0, b: 0, sum: 0, carry: 0, rowClass: "truth-false" },
      { a: 0, b: 1, sum: 1, carry: 0, rowClass: "truth-true" },
      { a: 1, b: 0, sum: 1, carry: 0, rowClass: "truth-true" },
      { a: 1, b: 1, sum: 0, carry: 1, rowClass: "truth-true" },
    ];
  }
});

function aOn(row: AdderRow | null) {
  return row?.a === 1;
}

function bOn(row: AdderRow | null) {
  return row?.b === 1;
}

function cInOn(row: AdderRow | null) {
  return isFull.value && row?.c === 1;
}

function sumOn(row: AdderRow | null) {
  return row?.sum === 1;
}

function carryOn(row: AdderRow | null) {
  return row?.carry === 1;
}
</script>

<template>
  <div class="gate-identity">
    <div class="gate-identity-label">Boolean Identity</div>
    <code class="gate-identity-formula">{{
      isFull ? "a + b + c = 2 · carry + sum" : "a + b = 2 · carry + sum"
    }}</code>
  </div>

  <div class="gate-demo">
    <TruthTable
      :key="props.type"
      :columns="columns"
      :rows="rows"
      :hovered-row="hovered"
      @row-enter="hovered = $event as AdderRow"
      @row-leave="hovered = null"
    />

    <div class="gate-visual">
      <div class="gate-svg-wrap">
        <svg
          class="gate-svg"
          :viewBox="`0 0 ${VIEW_W} ${VIEW_H}`"
          role="img"
          aria-label="Full Adder circuit diagram"
        >
          <!-- labels -->
          <GateLabel :x="LABEL_IN_X" :y="LABEL_A_Y" text="A" />
          <GateLabel :x="LABEL_IN_X" :y="LABEL_B_Y" text="B" />
          <GateLabel
            v-if="isFull"
            :x="LABEL_IN_X"
            :y="VIEW_GATE_OFFSET_Y + MID_Y"
            text="C"
          />

          <GateLabel
            :x="LABEL_OUT_X"
            :y="VIEW_GATE_OFFSET_Y + ADDER_SUM_Y"
            text="Sum"
            class="gate-text-out"
          />
          <GateLabel
            :x="LABEL_OUT_X"
            :y="VIEW_GATE_OFFSET_Y + ADDER_CARRY_Y"
            text="Carry"
            class="gate-text-out"
          />

          <!-- Adder Body -->
          <g
            :transform="`translate(${VIEW_GATE_OFFSET_X} ${VIEW_GATE_OFFSET_Y})`"
          >
            <AdderBody
              :is-full="isFull"
              :a-on="aOn(hovered)"
              :b-on="bOn(hovered)"
              :c-in-on="cInOn(hovered)"
              :sum-on="sumOn(hovered)"
              :carry-on="carryOn(hovered)"
            />
          </g>
        </svg>
      </div>

      <code class="gate-readout">
        <template v-if="isFull">
          FA({{ hovered?.a ?? "a" }}, {{ hovered?.b ?? "b" }},
          {{ hovered?.c ?? "c" }}) = { sum: {{ hovered?.sum ?? "-" }}, carry:
          {{ hovered?.carry ?? "-" }} }
        </template>
        <template v-else>
          HA({{ hovered?.a ?? "a" }}, {{ hovered?.b ?? "b" }}) = { sum:
          {{ hovered?.sum ?? "-" }}, carry: {{ hovered?.carry ?? "-" }} }
        </template>
      </code>
    </div>
  </div>
</template>
