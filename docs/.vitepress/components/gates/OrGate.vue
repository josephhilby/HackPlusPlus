<script setup lang="ts">
import './gate.css'
import { ref } from 'vue'
import TruthTable, { type TruthTableRow } from '../table/TruthTable.vue'

import GateLabel from './parts/GateLabel.vue'
import WireElbow from './parts/WireElbow.vue'
import OrBody from './parts/OrBody.vue'
import NandBody from './parts/NandBody.vue'
import NotBody from './parts/NotBody.vue'
import {
  LABEL_A_Y,
  LABEL_B_Y,
  LABEL_IN_X,
  LABEL_OUT_X,
  LABEL_OUT_Y,
  VIEW_GATE_OFFSET_X, VIEW_GATE_OFFSET_Y,
  VIEW_H,
  VIEW_W
} from "./parts/gateGeometry.js";

type OrRow = TruthTableRow & {
  a: number
  b: number
  out: number
  rowClass: string
}

const hovered = ref<OrRow | null>(null)

const columns = ['a', 'b', 'out']

const rows: OrRow[] = [
  { a: 0, b: 0, out: 0, rowClass: 'truth-false' },
  { a: 0, b: 1, out: 1, rowClass: 'truth-true' },
  { a: 1, b: 0, out: 1, rowClass: 'truth-true' },
  { a: 1, b: 1, out: 1, rowClass: 'truth-true' }
]

function aOn(row: OrRow | null) {
  return row?.a === 1
}

function bOn(row: OrRow | null) {
  return row?.b === 1
}

function outOn(row: OrRow | null) {
  return row?.out === 1
}

function notA(row: OrRow | null) {
  if (!row) return false
  return row.a === 0
}

function notB(row: OrRow | null) {
  if (!row) return false
  return row.b === 0
}
</script>

<template>
  <div class="gate-identity">
    <span class="identity-label">Identity:</span>
    <code class="identity-formula">OR(a, b) := ¬(¬a ∧ ¬b) = a ∨ b</code>
  </div>

  <div class="gate-demo">
    <TruthTable
        :columns="columns"
        :rows="rows"
        :hovered-row="hovered"
        @row-enter="hovered = $event as OrRow"
        @row-leave="hovered = null"
    />

    <div class="gate-visual">
      <div class="gate-svg-wrap">
        <svg
            class="gate-svg"
            :viewBox="`0 0 ${VIEW_W} ${VIEW_H}`"
            role="img"
            aria-label="OR gate implemented with two NOT gates feeding a NAND"
        >
          <!-- labels -->
          <GateLabel :x="LABEL_IN_X" :y="LABEL_A_Y" text="A" />
          <GateLabel :x="LABEL_IN_X" :y="LABEL_B_Y" text="B" />
          <GateLabel :x="LABEL_OUT_X" :y="LABEL_OUT_Y" text="Out" class="gate-text-out" />

          <!-- internal NOT, NOT, NAND implementation -->
          <g transform="translate(200 96) scale(0.22)">
            <!-- top NOT -->
            <g transform="translate(-150 -174)">
              <NotBody
                  :in-on="aOn(hovered)"
                  :out-on="notA(hovered)"
              />
            </g>

            <!-- bottom NOT -->
            <g transform="translate(-150 174)">
              <NotBody
                  :in-on="bOn(hovered)"
                  :out-on="notB(hovered)"
              />
            </g>

            <!-- final NAND -->
            <g transform="translate(150 0)">
              <!-- top NOT to NAND -->
              <WireElbow
                  :x1="-90"
                  :y1="-110"
                  :x2="0"
                  :y2="25"
                  :on="notA(hovered)"
              />

              <!-- bottom NOT to NAND -->
              <WireElbow
                  :x1="-90"
                  :y1="238"
                  :x2="0"
                  :y2="110"
                  :on="notB(hovered)"
              />

              <NandBody
                  :a-on="notA(hovered)"
                  :b-on="notB(hovered)"
                  :out-on="outOn(hovered)"
                  :out-x2="400"
              />
            </g>
          </g>

          <!-- outer OR shell -->
          <g :transform="`translate(${VIEW_GATE_OFFSET_X} ${VIEW_GATE_OFFSET_Y})`">
            <OrBody
                :a-on="aOn(hovered)"
                :b-on="bOn(hovered)"
                :out-on="outOn(hovered)"
                :out-x2="300"
            />
          </g>
        </svg>
      </div>

      <code class="gate-readout">
        OR({{ hovered?.a ?? "-" }}, {{ hovered?.b ?? "-" }}) = {{ hovered?.out ?? "-" }}
      </code>
    </div>
  </div>
</template>