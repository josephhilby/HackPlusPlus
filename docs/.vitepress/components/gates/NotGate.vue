<script setup lang="ts">
import './gate.css'
import { ref } from 'vue'
import TruthTable, { type TruthTableRow } from '../table/TruthTable.vue'

import GateLabel from './parts/GateLabel.vue'
import GateWire from './parts/GateWire.vue'
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
    <span class="identity-label">Identity:</span>
    <code class="identity-formula">NOT(in) := ¬(in ∧ in) = ¬in</code>
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
            viewBox="0 0 420 220"
            role="img"
            aria-label="NOT gate implemented with an internal NAND gate"
        >
          <!-- labels -->
          <GateLabel :x="34" :y="102" text="In" />
          <GateLabel :x="338" :y="102" text="Out" class="gate-text-out" />

          <!-- internal NAND implementation -->
          <g transform="translate(157 87) scale(0.22)">
            <!-- wire input to nand -->
            <GateWire
                :x1="-50"
                :y1="65"
                :x2="0"
                :y2="65"
                :on="inputOn(hovered)"
            />

            <!-- wire into both NAND inputs -->
            <GateWire
                :x1="0"
                :y1="38"
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
          <g transform="translate(70 37)">
            <NotBody
                :in-on="inputOn(hovered)"
                :out-on="outputOn(hovered)"
                :out-x2="240"
            />
          </g>
        </svg>
      </div>

      <code class="gate-readout">
        NOT({{ hovered?.in ?? "-" }}) = {{ hovered?.out ?? "-" }}
      </code>
    </div>
  </div>
</template>