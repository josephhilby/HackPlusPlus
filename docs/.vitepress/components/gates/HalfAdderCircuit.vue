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
} from "./parts/gateGeometry";

import AdderBody from "./parts/AdderBody.vue";
import GateLabel from "./parts/GateLabel.vue";
import XorBody from "./parts/XorBody.vue";
import AndBody from "./parts/AndBody.vue";
import WireElbow from "./parts/WireElbow.vue";
import WireC from "./parts/WireC.vue";
import Wire from "./parts/Wire.vue";

type AdderRow = TruthTableRow & {
  a: number;
  b: number;
  carry: number;
  sum: number;
  rowClass: string;
};

const hovered = ref<AdderRow | null>(null);
const columns = ["a", "b", "carry", "sum"];
const rows: AdderRow[] = [
  { a: 0, b: 0, carry: 0, sum: 0, rowClass: "truth-false" },
  { a: 0, b: 1, carry: 0, sum: 1, rowClass: "truth-true" },
  { a: 1, b: 0, carry: 0, sum: 1, rowClass: "truth-true" },
  { a: 1, b: 1, carry: 1, sum: 0, rowClass: "truth-true" },
];

const aOn = (row: AdderRow | null) => row?.a === 1;
const bOn = (row: AdderRow | null) => row?.b === 1;
const sumOn = (row: AdderRow | null) => row?.sum === 1;
const carryOn = (row: AdderRow | null) => row?.carry === 1;
</script>

<template>
  <div class="gate-identity">
    <div class="gate-identity-label">Boolean Identity</div>
    <code class="gate-identity-formula">a + b = 2 · carry + 1 · sum</code>
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
        <svg
          class="gate-svg"
          :viewBox="`0 0 ${VIEW_W} ${VIEW_H}`"
          role="img"
          aria-label="Half Adder circuit diagram"
        >
          <!-- labels -->
          <GateLabel :x="LABEL_IN_X" :y="LABEL_A_Y" text="A" />
          <GateLabel :x="LABEL_IN_X" :y="LABEL_B_Y" text="B" />

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

          <!-- Internals: XOR and AND (Grouped and scaled for clarity) -->
          <g transform="translate(160 94)  scale(0.25)">
            <!-- XOR for Sum path -->
            <g transform="translate(60 -115)">
              <AndBody
                :a-on="aOn(hovered)"
                :b-on="bOn(hovered)"
                :out-on="carryOn(hovered)"
                :out-x2="330"
              />
            </g>

            <!-- AND for Carry path -->
            <g transform="translate(60 115)">
              <XorBody
                :a-on="aOn(hovered)"
                :b-on="bOn(hovered)"
                :out-on="sumOn(hovered)"
                :out-x2="330"
              />
            </g>

            <!-- Input A Distribution (to XOR A and AND A) -->
            <Wire :x1="-250" :y1="-87" :x2="-10" :y2="-87" :on="aOn(hovered)" />
            <WireC :cx="-10" :y1="-89" :y2="141" :dx="60" :on="aOn(hovered)" />

            <!-- Input B Distribution (to XOR B and AND B) -->
            <Wire :x1="-250" :y1="216" :x2="30" :y2="216" :on="bOn(hovered)" />
            <WireC :cx="30" :y1="-13" :y2="217" :dx="60" :on="bOn(hovered)" />

            <!-- top AND to A -->
            <WireElbow
              :x1="600"
              :y1="-88"
              :x2="400"
              :y2="-50"
              :on="carryOn(hovered)"
            />

            <!-- bottom AND to B -->
            <WireElbow
              :x1="600"
              :y1="215"
              :x2="400"
              :y2="180"
              :on="sumOn(hovered)"
            />
          </g>

          <!-- Outer Shell -->
          <g
            :transform="`translate(${VIEW_GATE_OFFSET_X} ${VIEW_GATE_OFFSET_Y})`"
          >
            <AdderBody
              :is-full="false"
              :a-on="aOn(hovered)"
              :b-on="bOn(hovered)"
              :sum-on="sumOn(hovered)"
              :carry-on="carryOn(hovered)"
            />
          </g>
        </svg>
      </div>

      <code class="gate-readout">
        HA({{ hovered?.a ?? "a" }}, {{ hovered?.b ?? "b" }}) = { carry:
        {{ hovered?.carry ?? "-" }} , sum: {{ hovered?.sum ?? "-" }} }
      </code>
    </div>
  </div>
</template>
