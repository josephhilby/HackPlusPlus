<script setup lang="ts">
import './gate.css'
import { ref } from 'vue'
import TruthTable, { type TruthTableRow } from '../table/TruthTable.vue'

import GateInversionBubble from './parts/GateInversionBubble.vue'
import GateLabel from './parts/GateLabel.vue'
import GateWire from './parts/GateWire.vue'
import NandBody from './parts/NandBody.vue'
import NotBody from './parts/NotBody.vue'

type NotRow = TruthTableRow & {
  input: number
  out: number
  rowClass: string
}

const hovered = ref<NotRow | null>(null)

const columns = ['input', 'out']

const rows: NotRow[] = [
  { input: 0, out: 1, rowClass: 'truth-true' },
  { input: 1, out: 0, rowClass: 'truth-false' }
]
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
            class="nand-svg"
            viewBox="0 0 420 220"
            role="img"
            aria-label="NOT gate implemented with an internal NAND gate"
        >
          <GateLabel :x="34" :y="102" text="In" />
          <GateLabel :x="338" :y="102" text="Out" class="gate-text-out" />

          <!-- outer input wire -->
          <GateWire
              :x1="62"
              :y1="102"
              :x2="157"
              :y2="102"
              :on="hovered?.input === 1"
          />

          <!-- split wire to internal NAND top -->
          <GateWire
              :x1="158"
              :y1="110"
              :x2="158"
              :y2="94"
              :on="hovered?.input === 1"
          />

          <!-- outer NOT shell -->
          <g transform="translate(150 52)">
            <NotBody />
          </g>

          <!-- small internal NAND -->
          <g transform="translate(160 88) scale(0.22)">
            <NandBody
                :a-on="hovered?.input === 1"
                :b-on="hovered?.input === 1"
                :out-on="hovered?.out === 1"
            />
          </g>

          <!-- output wire -->
          <GateWire
              :x1="210"
              :y1="102"
              :x2="330"
              :y2="102"
              :on="hovered?.out === 1"
          />

          <!-- outer NOT bubble -->
          <GateInversionBubble :cx="266" :cy="102" :r="10" />
        </svg>
      </div>

      <code class="gate-readout">
        NOT({{ hovered?.input ?? "-" }}) = {{ hovered?.out ?? "-" }}
      </code>
    </div>
  </div>
</template>