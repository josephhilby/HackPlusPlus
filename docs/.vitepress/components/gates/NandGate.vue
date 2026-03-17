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

import GateLabel from './parts/GateLabel.vue'
import NandBody from './parts/NandBody.vue'

type NandRow = TruthTableRow & {
  a: number
  b: number
  out: number
  rowClass: string
}

const hovered = ref<NandRow | null>(null)

const columns = ['a', 'b', 'out']

const rows: NandRow[] = [
  { a: 0, b: 0, out: 1, rowClass: 'truth-true' },
  { a: 0, b: 1, out: 1, rowClass: 'truth-true' },
  { a: 1, b: 0, out: 1, rowClass: 'truth-true' },
  { a: 1, b: 1, out: 0, rowClass: 'truth-false' }
]
</script>

<template>
  <div class="gate-identity">
    <div class="gate-identity-label">Boolean Identity</div>
    <code class="gate-identity-formula">¬(a ∧ b)</code>
  </div>

  <div class="gate-demo">
    <TruthTable
        :columns="columns"
        :rows="rows"
        :hovered-row="hovered"
        @row-enter="hovered = $event as NandRow"
        @row-leave="hovered = null"
    />

    <div class="gate-visual">
      <div class="gate-svg-wrap">
        <svg
            class="gate-svg"
            :viewBox="`0 0 ${VIEW_W} ${VIEW_H}`"
            role="img"
            aria-label="NAND gate visualization"
        >
          <GateLabel :x="LABEL_IN_X" :y="LABEL_A_Y" text="A" />
          <GateLabel :x="LABEL_IN_X" :y="LABEL_B_Y" text="B" />
          <GateLabel :x="LABEL_OUT_X" :y="LABEL_OUT_Y" text="Out" class="gate-text-out" />

          <g :transform="`translate(${VIEW_GATE_OFFSET_X} ${VIEW_GATE_OFFSET_Y})`">
            <NandBody
                :a-on="hovered?.a === 1"
                :b-on="hovered?.b === 1"
                :out-on="hovered?.out === 1"
                :outX2="300"
            />
          </g>
        </svg>
      </div>

      <code class="gate-readout">
        NAND({{ hovered?.a ?? "-" }}, {{ hovered?.b ?? "-" }}) = {{ hovered?.out ?? "-" }}
      </code>
    </div>
  </div>
</template>