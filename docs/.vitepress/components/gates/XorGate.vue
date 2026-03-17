<script setup lang="ts">
import './gate.css'
import { ref } from 'vue'
import TruthTable, { type TruthTableRow } from '../table/TruthTable.vue'

import GateLabel from './parts/GateLabel.vue'
import WireElbow from './parts/WireElbow.vue'

import AndBody from './parts/AndBody.vue'
import OrBody from './parts/OrBody.vue'
import NandBody from './parts/NandBody.vue'
import XorBody from './parts/XorBody.vue'
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
import Wire from "./parts/Wire.vue";

type XorRow = TruthTableRow & {
  a: number
  b: number
  out: number
  rowClass: string
}

const hovered = ref<XorRow | null>(null)

const columns = ['a', 'b', 'out']

const rows: XorRow[] = [
  { a: 0, b: 0, out: 0, rowClass: 'truth-false' },
  { a: 0, b: 1, out: 1, rowClass: 'truth-true' },
  { a: 1, b: 0, out: 1, rowClass: 'truth-true' },
  { a: 1, b: 1, out: 0, rowClass: 'truth-false' }
]

function aOn(row: XorRow | null) {
  return row?.a === 1
}

function bOn(row: XorRow | null) {
  return row?.b === 1
}

function outOn(row: XorRow | null) {
  return row?.out === 1
}

function orIntermediate(row: XorRow | null) {
  if (!row) return false
  return row.a === 1 || row.b === 1
}

function nandIntermediate(row: XorRow | null) {
  if (!row) return false
  return !(row.a === 1 && row.b === 1)
}
</script>

<template>
  <div class="gate-identity">
    <span class="identity-label">Identity:</span>
    <code class="identity-formula">XOR(a, b) := (a ∨ b) ∧ ¬(a ∧ b)</code>
  </div>

  <div class="gate-demo">
    <TruthTable
        :columns="columns"
        :rows="rows"
        :hovered-row="hovered"
        @row-enter="hovered = $event as XorRow"
        @row-leave="hovered = null"
    />

    <div class="gate-visual">
      <div class="gate-svg-wrap">
        <svg
            class="gate-svg"
            :viewBox="`0 0 ${VIEW_W} ${VIEW_H}`"
            role="img"
            aria-label="XOR gate implemented as OR feeding AND with NAND"
        >
          <!-- labels -->
          <GateLabel :x="LABEL_IN_X" :y="LABEL_A_Y" text="A" />
          <GateLabel :x="LABEL_IN_X" :y="LABEL_B_Y" text="B" />
          <GateLabel :x="LABEL_OUT_X" :y="LABEL_OUT_Y" text="Out" class="gate-text-out" />

          <!-- internal OR, NAND, AND implementation -->
          <g transform="translate(210 96) scale(0.22)">
            <!-- input wires -->
            <WireC
                :cx="-100"
                :y1="-149"
                :y2="200"
                :dx="100"
                :on="aOn(hovered)"
            />
            <Wire
                :x1="-200"
                :y1="-110"
                :x2="-100"
                :y2="-110"
                :on="aOn(hovered)"
            />
            <WireC
                :cx="-50"
                :y1="-72"
                :y2="277"
                :dx="100"
                :on="bOn(hovered)"
            />
            <Wire
                :x1="-200"
                :y1="237"
                :x2="-50"
                :y2="237"
                :on="bOn(hovered)"
            />


            <!-- top OR -->
            <g transform="translate(0 -174)">
              <OrBody
                  :a-on="aOn(hovered)"
                  :b-on="bOn(hovered)"
                  :out-on="orIntermediate(hovered)"
              />
            </g>

            <!-- bottom NAND -->
            <g transform="translate(0 174)">
              <NandBody
                  :a-on="aOn(hovered)"
                  :b-on="bOn(hovered)"
                  :out-on="nandIntermediate(hovered)"
              />
            </g>

            <!-- final AND -->
            <g transform="translate(280 0)">
              <!-- top OR to AND -->
              <WireElbow
                  :x1="-10"
                  :y1="-110"
                  :x2="0"
                  :y2="25"
                  :on="orIntermediate(hovered)"
              />

              <!-- bottom NAND to AND -->
              <WireElbow
                  :x1="-10"
                  :y1="238"
                  :x2="0"
                  :y2="110"
                  :on="nandIntermediate(hovered)"
              />

              <AndBody
                  :a-on="orIntermediate(hovered)"
                  :b-on="nandIntermediate(hovered)"
                  :out-on="outOn(hovered)"
                  :out-x2="300"
              />
            </g>

          </g>

          <!-- OUTER XOR BODY (drawn last) -->
          <g :transform="`translate(${VIEW_GATE_OFFSET_X} ${VIEW_GATE_OFFSET_Y})`">
            <XorBody
                :a-on="aOn(hovered)"
                :b-on="bOn(hovered)"
                :out-on="outOn(hovered)"
                :out-x2="300"
            />
          </g>

        </svg>
      </div>

      <code class="gate-readout">
        XOR({{ hovered?.a ?? "-" }}, {{ hovered?.b ?? "-" }}) = {{ hovered?.out ?? "-" }}
      </code>
    </div>
  </div>
</template>