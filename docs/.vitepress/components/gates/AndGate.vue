<script setup lang="ts">
import './gate.css'
import { ref } from 'vue'
import TruthTable, { type TruthTableRow } from '../table/TruthTable.vue'

import GateInversionBubble from './parts/GateInversionBubble.vue'
import GateLabel from './parts/GateLabel.vue'
import GateWire from './parts/GateWire.vue'
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

function nandIntermediate(row: AndRow | null) {
  if (!row) return false
  return row.out === 0
}
</script>

<template>
  <div class="gate-identity">
    <span class="identity-label">Identity:</span>
    <code class="identity-formula">AND(a, b) := ¬(¬(a ∧ b)) = a ∧ b</code>
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
            class="nand-svg"
            viewBox="0 0 450 220"
            role="img"
            aria-label="AND gate implemented with an internal NAND followed by NOT"
        >
          <GateLabel :x="28" :y="66" text="A" />
          <GateLabel :x="28" :y="140" text="B" />
          <GateLabel :x="380" :y="103" text="Out" class="gate-text-out" />

          <!-- outer input wires -->
          <GateWire
              :x1="54"
              :y1="64"
              :x2="150"
              :y2="64"
              :on="hovered?.a === 1"
          />
          <GateWire
              :x1="54"
              :y1="140"
              :x2="150"
              :y2="140"
              :on="hovered?.b === 1"
          />

          <!-- large outer AND shell -->
          <path
              d="M140 38 L250 38 A72 72 0 0 1 250 166 L140 166 Z"
              class="gate-shell"
          />

          <g transform="translate(170 72)">
            <!-- nand -->
            <g transform="translate(0 17) scale(0.18)">
              <NandBody
                  :a-on="hovered?.a === 1"
                  :b-on="hovered?.b === 1"
                  :out-on="nandIntermediate(hovered)"
              />
            </g>

            <!-- not -->
            <g transform="translate(50 17) scale(0.24)">
              <NotBody />
            </g>

            <!-- inversion bubble -->
            <GateInversionBubble :cx="80" :cy="30" :r="5" />
          </g>

          <!-- final output wire -->
          <GateWire
              :x1="260"
              :y1="102"
              :x2="370"
              :y2="102"
              :on="hovered?.out === 1"
          />
        </svg>
      </div>

      <code class="gate-readout">
        AND({{ hovered?.a ?? "-" }}, {{ hovered?.b ?? "-" }}) = {{ hovered?.out ?? "-" }}
      </code>
    </div>
  </div>
</template>