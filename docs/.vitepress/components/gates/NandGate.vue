<script setup lang="ts">
import './gate.css'
import { ref } from 'vue'
import TruthTable, { type TruthTableRow } from '../table/TruthTable.vue'

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
    <span class="identity-label">Identity:</span>
    <code class="identity-formula">NAND(a, b) := ¬(a ∧ b)</code>
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
            class="nand-svg"
            viewBox="0 0 420 220"
            role="img"
            aria-label="NAND gate visualization"
        >
          <GateLabel :x="36" :y="66" text="A" />
          <GateLabel :x="36" :y="140" text="B" />
          <GateLabel :x="340" :y="103" text="Out" class="gate-text-out" />

          <g transform="translate(62 38)">
            <NandBody
                :a-on="hovered?.a === 1"
                :b-on="hovered?.b === 1"
                :out-on="hovered?.out === 1"
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