<script setup lang="ts">
import "./gate.css";
import { ref } from "vue";
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
} from "./parts/gateGeometry";

import AdderBody from "./parts/AdderBody.vue";
import GateLabel from "./parts/GateLabel.vue";
import OrBody from "./parts/OrBody.vue";
import Wire from "./parts/Wire.vue";
import WireElbow from "./parts/WireElbow.vue";
import WireC from "./parts/WireC.vue";

type AdderRow = TruthTableRow & {
  a: number;
  b: number;
  c: number;
  carry: number;
  sum: number;
  rowClass: string;
};

const hovered = ref<AdderRow | null>(null);
const columns = ["a", "b", "c", "carry", "sum"];
const rows: AdderRow[] = [
  { a: 0, b: 0, c: 0, sum: 0, carry: 0, rowClass: "truth-false" },
  { a: 0, b: 0, c: 1, sum: 1, carry: 0, rowClass: "truth-true" },
  { a: 0, b: 1, c: 0, sum: 1, carry: 0, rowClass: "truth-true" },
  { a: 0, b: 1, c: 1, sum: 0, carry: 1, rowClass: "truth-true" },
  { a: 1, b: 0, c: 0, sum: 1, carry: 0, rowClass: "truth-true" },
  { a: 1, b: 0, c: 1, sum: 0, carry: 1, rowClass: "truth-true" },
  { a: 1, b: 1, c: 0, sum: 0, carry: 1, rowClass: "truth-true" },
  { a: 1, b: 1, c: 1, sum: 1, carry: 1, rowClass: "truth-true" },
];

const aOn = (row: AdderRow | null) => row?.a === 1;
const bOn = (row: AdderRow | null) => row?.b === 1;
const cOn = (row: AdderRow | null) => row?.c === 1;
const sumOn = (row: AdderRow | null) => row?.sum === 1;
const carryOn = (row: AdderRow | null) => row?.carry === 1;

// Intermediate signals for FA logic
const sum0 = (row: AdderRow | null) => (row ? (row.a ^ row.b) === 1 : false);
const carry0 = (row: AdderRow | null) => (row ? (row.a & row.b) === 1 : false);
const carry1 = (row: AdderRow | null) =>
  row ? ((row.a ^ row.b) & row.c) === 1 : false;
</script>

<template>
  <div class="gate-identity">
    <div class="gate-identity-label">Boolean Identity</div>
    <code class="gate-identity-formula"
      >a + b + c = (2 · carry) + (1 · sum)</code
    >
  </div>

  <div class="gate-demo">
    <TruthTable
      :columns="columns"
      :rows="rows"
      :hovered-row="hovered"
      @row-enter="hovered = $event as AdderRow"
      @row-leave="hovered = null"
    />

    <div class="gate-visual">
      <div class="gate-svg-wrap">
        <svg class="gate-svg" :viewBox="`0 0 ${VIEW_W} ${VIEW_H}`">
          <GateLabel :x="LABEL_IN_X" :y="LABEL_A_Y" text="A" />
          <GateLabel :x="LABEL_IN_X" :y="VIEW_GATE_OFFSET_Y + MID_Y" text="B" />
          <GateLabel :x="LABEL_IN_X" :y="LABEL_B_Y" text="C" />

          <GateLabel
            :x="LABEL_OUT_X"
            :y="LABEL_A_Y"
            text="Carry"
            class="gate-text-out"
          />
          <GateLabel
            :x="LABEL_OUT_X"
            :y="LABEL_B_Y"
            text="Sum"
            class="gate-text-out"
          />

          <!-- Internals: HA + HA + OR (Grouped and scaled for clarity) -->
          <g transform="translate(160 94)  scale(0.18)">
            <g transform="translate(30 90)">
              <!-- HA1 -->
              <g transform="translate(-150 -100)">
                <AdderBody
                  :is-full="false"
                  :a-on="aOn(hovered)"
                  :b-on="bOn(hovered)"
                  :sum-on="sum0(hovered)"
                  :carry-on="carry0(hovered)"
                />
              </g>

              <!-- HA2 -->
              <g transform="translate(100 -20)">
                <AdderBody
                  :is-full="false"
                  :a-on="sum0(hovered)"
                  :b-on="cOn(hovered)"
                  :sum-on="sumOn(hovered)"
                  :carry-on="carry1(hovered)"
                />
              </g>

              <!-- OR -->
              <g transform="translate(350 -95)">
                <OrBody
                  :a-on="carry0(hovered)"
                  :b-on="carry1(hovered)"
                  :out-on="carryOn(hovered)"
                  :out-x2="330"
                />
              </g>

              <!-- Input Wires -->
              <WireElbow
                :x1="-250"
                :y1="-211"
                :x2="-150"
                :y2="-80"
                :on="aOn(hovered)"
              />

              <WireElbow
                :x1="-250"
                :y1="0"
                :x2="-150"
                :y2="0"
                :on="bOn(hovered)"
              />

              <WireElbow
                :x1="-250"
                :y1="210"
                :x2="100"
                :y2="80"
                :on="cOn(hovered)"
              />

              <!-- HA1 to OR -->
              <WireElbow
                :x1="120"
                :y1="-70"
                :x2="330"
                :y2="-70"
                :on="carry0(hovered)"
              />

              <!-- Output Wires -->
              <WireElbow
                :x1="900"
                :y1="-210"
                :x2="680"
                :y2="-40"
                :on="carryOn(hovered)"
              />

              <WireElbow
                :x1="900"
                :y1="210"
                :x2="370"
                :y2="90"
                :on="sumOn(hovered)"
              />
            </g>
          </g>

          <!-- Outer Shell -->
          <g
            :transform="`translate(${VIEW_GATE_OFFSET_X} ${VIEW_GATE_OFFSET_Y})`"
          >
            <AdderBody
              :is-full="true"
              :a-on="aOn(hovered)"
              :b-on="bOn(hovered)"
              :c-on="cOn(hovered)"
              :sum-on="sumOn(hovered)"
              :carry-on="carryOn(hovered)"
            />
          </g>
        </svg>
      </div>

      <code class="gate-readout">
        FA({{ hovered?.a ?? "a" }}, {{ hovered?.b ?? "b" }},
        {{ hovered?.c ?? "c" }}) = { carry: {{ hovered?.carry ?? "-" }} , sum:
        {{ hovered?.sum ?? "-" }} }
      </code>
    </div>
  </div>
</template>
