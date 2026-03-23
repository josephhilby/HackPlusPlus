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
  LABEL_OUT_X,
  LABEL_OUT_Y
} from './parts/gateGeometry'

import GateLabel from './parts/GateLabel.vue'
import Wire from './parts/Wire.vue'
import NandBody from './parts/NandBody.vue'
import NotBody from './parts/NotBody.vue'

type NotRow = TruthTableRow & {
  in: number
  out: number
  rowClass: string
}

const hovered = ref<NotRow | null>(null)

const columns = ['in', 'out']

const rows: NotRow[] = [
  { in: 0, out: 1, rowClass: 'truth-true' },
  { in: 1, out: 0, rowClass: 'truth-false' }
]

function inputOn(row: NotRow | null) {
  return row?.in === 1
}

function outputOn(row: NotRow | null) {
  return row?.out === 1
}
</script>

<template>
  <div class="gate-identity">
    <div class="gate-identity-label">Boolean Identity</div>
    <code class="gate-identity-formula">¬(in ∧ in) = ¬in</code>
  </div>

  <div class="gate-demo">
    <TruthTable
        :columns="columns"
        :rows="rows"
        :hovered-row="hovered"
        @row-enter="hovered = $event as NotRow"
        @row-leave="hovered = null"
    />

    <div class="gate-visual">
      <div class="gate-svg-wrap">
        <svg
            class="gate-svg"
            :viewBox="`0 0 ${VIEW_W} ${VIEW_H}`"
            role="img"
            aria-label="NOT gate implemented with NAND gate"
        >
          <!-- labels -->
          <GateLabel :x="LABEL_IN_X" :y="LABEL_OUT_Y" text="In" />
          <GateLabel :x="LABEL_OUT_X" :y="LABEL_OUT_Y" text="Out" class="gate-text-out" />

          <!-- internal NAND implementation -->
          <g transform="translate(165 96) scale(0.22)">
            <!-- wire input to nand -->
            <Wire
                :x1="-50"
                :y1="65"
                :x2="0"
                :y2="65"
                :on="inputOn(hovered)"
            />

            <!-- wire into both NAND inputs -->
            <Wire
                :x1="0"
                :y1="30"
                :x2="0"
                :y2="100"
                :on="inputOn(hovered)"
            />

            <NandBody
                :a-on="inputOn(hovered)"
                :b-on="inputOn(hovered)"
                :out-on="outputOn(hovered)"
                :out-x2="410"
            />
          </g>

          <!-- outer NOT shell -->
          <g :transform="`translate(${VIEW_GATE_OFFSET_X} ${VIEW_GATE_OFFSET_Y})`">
            <NotBody
                :in-on="inputOn(hovered)"
                :out-on="outputOn(hovered)"
                :out-x2="300"
            />
          </g>
        </svg>
      </div>

      <code class="gate-readout">
        NOT({{ hovered?.in ?? "in" }}) = {{ hovered?.out ?? "-" }}
      </code>
    </div>
  </div>
</template>