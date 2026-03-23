<script setup lang="ts">
import './gate.css'
import { ref } from 'vue'
import TruthTable, { type TruthTableRow } from '../table/TruthTable.vue'

import GateLabel from './parts/GateLabel.vue'
import WireElbow from './parts/WireElbow.vue'
import NotBody from './parts/NotBody.vue'
import MuxBody from './parts/MuxBody.vue'
import {
  LABEL_IN_X,
  LABEL_OUT_X,
  LABEL_A_Y,
  LABEL_B_Y,
  LABEL_OUT_Y,
  VIEW_H,
  VIEW_W,
  VIEW_GATE_OFFSET_X,
  VIEW_GATE_OFFSET_Y
} from './parts/gateGeometry.js'
import Wire from "./parts/Wire.vue";

type MuxRow = TruthTableRow & {
  a: number
  b: number
  sel: number
  out: number
  rowClass: string
}

const hovered = ref<MuxRow | null>(null)

const columns = ['a', 'b', 'sel', 'out']

const rows: MuxRow[] = [
  { a: 0, b: 0, sel: 0, out: 0, rowClass: 'truth-false' },
  { a: 0, b: 1, sel: 0, out: 0, rowClass: 'truth-false' },
  { a: 1, b: 0, sel: 0, out: 1, rowClass: 'truth-true' },
  { a: 1, b: 1, sel: 0, out: 1, rowClass: 'truth-true' },
  { a: 0, b: 0, sel: 1, out: 0, rowClass: 'truth-false' },
  { a: 0, b: 1, sel: 1, out: 1, rowClass: 'truth-true' },
  { a: 1, b: 0, sel: 1, out: 0, rowClass: 'truth-false' },
  { a: 1, b: 1, sel: 1, out: 1, rowClass: 'truth-true' }
]

function aOn(row: MuxRow | null) {
  return row?.a === 1
}

function bOn(row: MuxRow | null) {
  return row?.b === 1
}

function selOn(row: MuxRow | null) {
  return row?.sel === 1
}

function outOn(row: MuxRow | null) {
  return row?.out === 1
}

function notSel(row: MuxRow | null) {
  if (!row) return false
  return row.sel === 0
}

function topAndOut(row: MuxRow | null) {
  if (!row) return false
  return row.a === 1 && row.sel === 0
}

function bottomAndOut(row: MuxRow | null) {
  if (!row) return false
  return row.b === 1 && row.sel === 1
}

</script>

<template>
  <div class="gate-identity">
    <div class="gate-identity-label">Boolean Identity</div>
    <code class="gate-identity-formula">(a ∧ ¬sel) ∨ (b ∧ sel)</code>
  </div>

  <div class="gate-demo">
    <TruthTable
        :columns="columns"
        :rows="rows"
        :hovered-row="hovered"
        @row-enter="hovered = $event as MuxRow"
        @row-leave="hovered = null"
    />

    <div class="gate-visual">
      <div class="gate-svg-wrap">
        <svg
            class="gate-svg"
            :viewBox="`0 0 ${VIEW_W} ${VIEW_H}`"
            role="img"
            aria-label="2-to-1 multiplexer implemented with one NOT gate and three NAND gates"
        >
          <!-- labels -->
          <GateLabel :x="LABEL_IN_X" :y="LABEL_A_Y" text="A" />
          <GateLabel :x="LABEL_IN_X" :y="LABEL_B_Y" text="B" />
          <GateLabel :x="LABEL_OUT_X" :y="LABEL_OUT_Y" text="Out" class="gate-text-out" />
          <GateLabel :x="210" :y="-10" text="Sel" />

          <!-- internal NAND implementation -->
          <g transform="translate(190 67)  scale(0.18)">
            <!-- top = (a ∧ ¬sel) -->
            <g transform="translate(30 55)">
              <AndBody
                  :a-on="aOn(hovered)"
                  :b-on="notSel(hovered)"
                  :out-on="topAndOut(hovered)"
                  :out-x2="250"
              />
            </g>

            <!-- ¬(sel) -->
            <g transform="translate(-90 110) scale(0.75)">
              <NotBody
                  :in-on="selOn(hovered)"
                  :out-on="notSel(hovered)"
                  :out-x2="200"
              />
            </g>

            <!-- bottom = (b ∧ sel) -->
            <g transform="translate(30 295)">
              <AndBody
                  :a-on="bOn(hovered)"
                  :b-on="selOn(hovered)"
                  :out-on="bottomAndOut(hovered)"
                  :out-x2="250"
              />
            </g>

            <!-- final OR of gated branches -->
            <g transform="translate(300 175)">
              <OrBody
                  :a-on="topAndOut(hovered)"
                  :b-on="bottomAndOut(hovered)"
                  :out-on="outOn(hovered)"
                  :out-x2="300"
              />
            </g>

            <!-- A into top AND -->
            <WireElbow
                :x1="-210"
                :y1="26"
                :x2="30"
                :y2="80"
                :on="aOn(hovered)"
            />

            <!-- B into bottom AND -->
            <WireElbow
                :x1="-210"
                :y1="450"
                :x2="-30"
                :y2="330"
                :on="bOn(hovered)"
            />
            <Wire :x1="-30" :y1="320" :x2="20" :y2="320" :on="bOn(hovered)" />

            <!-- Sel into Feeder -->
            <WireElbow
                :x1="-100"
                :y1="-100"
                :x2="185"
                :y2="-200"
                :on="selOn(hovered)"
            />

            <!-- Feeder into top/btm AND -->
            <WireElbow
                :x1="30"
                :y1="397"
                :x2="-100"
                :y2="-100"
                :on="selOn(hovered)"
            />

            <!-- top AND into final OR -->
            <WireElbow
                :x1="290"
                :y1="120"
                :x2="300"
                :y2="203"
                :on="topAndOut(hovered)"
            />

            <!-- bottom AND into final OR -->
            <WireElbow
                :x1="300"
                :y1="277"
                :x2="290"
                :y2="360"
                :on="bottomAndOut(hovered)"
            />
          </g>

          <!-- outer mux symbol -->
          <g :transform="`translate(${VIEW_GATE_OFFSET_X} ${VIEW_GATE_OFFSET_Y})`">
            <MuxBody
                mode="mux"
                :top-on="aOn(hovered)"
                :bottom-on="bOn(hovered)"
                :mid-on="outOn(hovered)"
                :select-on="selOn(hovered)"
                :out-x2="280"
            />
          </g>
        </svg>
      </div>

      <code class="gate-readout">
        MUX({{ hovered?.a ?? "a" }}, {{ hovered?.b ?? "b" }}, {{ hovered?.sel ?? "sel" }}) = {{ hovered?.out ?? "-" }}
      </code>
    </div>
  </div>
</template>