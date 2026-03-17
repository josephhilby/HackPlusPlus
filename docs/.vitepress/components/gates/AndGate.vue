<script setup lang="ts">
import './gate.css'
import { ref } from 'vue'
import TruthTable, { type TruthTableRow } from '../table/TruthTable.vue'

import {
  VIEW_H,
  VIEW_W,
  VIEW_GATE_OFFSET_X,
  VIEW_GATE_OFFSET_Y,
  LABEL_IN_X,
  LABEL_A_Y,
  LABEL_B_Y,
  LABEL_OUT_X,
  LABEL_OUT_Y
} from './parts/gateGeometry'

import AndBody from './parts/AndBody.vue'
import GateLabel from './parts/GateLabel.vue'
import WireElbow from './parts/WireElbow.vue'
import NandBody from './parts/NandBody.vue'
import NotBody from './parts/NotBody.vue'

type AndRow = TruthTableRow & {
  a: number
  b: number
  out: number
  rowClass: string
}

const hovered = ref<AndRow | null>(null)

const columns = ['a', 'b', 'out']

const rows: AndRow[] = [
  { a: 0, b: 0, out: 0, rowClass: 'truth-false' },
  { a: 0, b: 1, out: 0, rowClass: 'truth-false' },
  { a: 1, b: 0, out: 0, rowClass: 'truth-false' },
  { a: 1, b: 1, out: 1, rowClass: 'truth-true' }
]

function aOn(row: AndRow | null) {
  return row?.a === 1
}

function bOn(row: AndRow | null) {
  return row?.b === 1
}

function outOn(row: AndRow | null) {
  return row?.out === 1
}

function nandIntermediate(row: AndRow | null) {
  if (!row) return false
  return row.out === 0
}
</script>

<template>
  <div class="gate-identity">
    <div class="gate-identity-label">Boolean Identity</div>
    <code class="gate-identity-formula">¬(¬(a ∧ b)) = a ∧ b</code>
  </div>

  <div class="gate-demo">
    <TruthTable
        :columns="columns"
        :rows="rows"
        :hovered-row="hovered"
        @row-enter="hovered = $event as AndRow"
        @row-leave="hovered = null"
    />

    <div class="gate-visual">
      <div class="gate-svg-wrap">
        <svg
            class="gate-svg"
            :viewBox="`0 0 ${VIEW_W} ${VIEW_H}`"
            role="img"
            aria-label="AND gate implemented with NAND gate feeding a NOT"
        >
          <!-- labels -->
          <GateLabel :x="LABEL_IN_X" :y="LABEL_A_Y" text="A" />
          <GateLabel :x="LABEL_IN_X" :y="LABEL_B_Y" text="B" />
          <GateLabel :x="LABEL_OUT_X" :y="LABEL_OUT_Y" text="Out" class="gate-text-out" />

          <!-- internal NAND, NOT implementation -->
          <g transform="translate(200 96) scale(0.22)">
            <!-- NAND -->
            <g transform="translate(-100 0)">
              <!-- Input to NAND -->
              <WireElbow
                  :x1="-100"
                  :y1="-110"
                  :x2="0"
                  :y2="25"
                  :on="aOn(hovered)"
              />

              <WireElbow
                  :x1="-100"
                  :y1="235"
                  :x2="0"
                  :y2="110"
                  :on="bOn(hovered)"
              />

              <NandBody
                  :a-on="aOn(hovered)"
                  :b-on="bOn(hovered)"
                  :out-on="nandIntermediate(hovered)"
              />
            </g>

            <!-- NOT -->
            <g transform="translate(150 0)">
              <NotBody
                  :in-on="nandIntermediate(hovered)"
                  :out-on="outOn(hovered)"
              />
            </g>
          </g>

          <!-- outer AND shell -->
          <g :transform="`translate(${VIEW_GATE_OFFSET_X} ${VIEW_GATE_OFFSET_Y})`">
            <AndBody
                :a-on="aOn(hovered)"
                :b-on="bOn(hovered)"
                :out-on="outOn(hovered)"
                :outX2="300"
            />
          </g>
        </svg>
      </div>

      <code class="gate-readout">
        AND({{ hovered?.a ?? "-" }}, {{ hovered?.b ?? "-" }}) = {{ hovered?.out ?? "-" }}
      </code>
    </div>
  </div>
</template>