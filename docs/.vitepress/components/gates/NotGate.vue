<script setup lang="ts">
import '../table/truthtable.css'
import './gate.css'
import { ref } from 'vue'

type TruthRow = {
  input: number
  out: number
  rowClass: string
}

const hovered = ref<TruthRow | null>(null)

const rows: TruthRow[] = [
  { input: 0, out: 1, rowClass: 'truth-true' },
  { input: 1, out: 0, rowClass: 'truth-false' }
]
</script>

<template>
  <div class="gate-identity">
    <span class="identity-label">Identity:</span>
    <code class="identity-formula">NOT(in) := ¬(in ∧ in)</code>
  </div>

  <div class="gate-demo">
    <div class="truth-table-wrap">
      <table class="truth-table">
        <thead>
        <tr>
          <th>in</th>
          <th>out</th>
        </tr>
        </thead>

        <tbody>
        <tr
            v-for="(row, i) in rows"
            :key="i"
            :class="[row.rowClass, { hovered: hovered === row }]"
            @mouseenter="hovered = row"
            @mouseleave="hovered = null"
        >
          <td>{{ row.input }}</td>
          <td>{{ row.out }}</td>
        </tr>
        </tbody>
      </table>
    </div>

    <div class="gate-visual">
      <div class="gate-svg-wrap">
        <svg
            class="nand-svg"
            viewBox="0 0 420 220"
            role="img"
            aria-label="NOT gate implemented with an internal NAND gate"
        >
          <!-- external labels -->
          <text x="34" y="108" class="gate-text">In</text>
          <text x="336" y="108" class="gate-text gate-text-out">Out</text>

          <!-- outer NOT triangle -->
          <path
              d="M150 52 L150 152 L255 102 Z"
              class="gate-shell"
          />

          <!-- main input wire -->
          <line
              x1="62"
              y1="102"
              x2="145"
              y2="102"
              :class="['gate-wire', hovered?.input === 1 ? 'is-on' : 'is-off']"
          />

          <!-- split wire to internal NAND top -->
          <line
              x1="145"
              y1="102"
              x2="145"
              y2="84"
              :class="['gate-wire', hovered?.input === 1 ? 'is-on' : 'is-off']"
          />
          <line
              x1="145"
              y1="84"
              x2="178"
              y2="84"
              :class="['gate-wire', hovered?.input === 1 ? 'is-on' : 'is-off']"
          />

          <!-- split wire to internal NAND bottom -->
          <line
              x1="145"
              y1="102"
              x2="145"
              y2="120"
              :class="['gate-wire', hovered?.input === 1 ? 'is-on' : 'is-off']"
          />
          <line
              x1="145"
              y1="120"
              x2="178"
              y2="120"
              :class="['gate-wire', hovered?.input === 1 ? 'is-on' : 'is-off']"
          />

          <!-- internal NAND output -->
          <line
              x1="236"
              y1="102"
              x2="255"
              y2="102"
              :class="['gate-wire', hovered?.out === 1 ? 'is-on' : 'is-off']"
          />

          <!-- small internal NAND gate -->
          <path
              d="M178 74 L204 74 A28 28 0 0 1 204 130 L178 130 Z"
              class="gate-body"
          />

          <!-- small internal NAND bubble -->
          <circle cx="230" cy="102" r="6" class="gate-bubble" />

          <!-- outer NOT bubble -->
          <circle cx="266" cy="102" r="10" class="gate-bubble" />

          <!-- output wire -->
          <line
              x1="276"
              y1="102"
              x2="330"
              y2="102"
              :class="['gate-wire', hovered?.out === 1 ? 'is-on' : 'is-off']"
          />

          <!-- internal label -->
          <text x="186" y="147" class="gate-text gate-text-small">NAND</text>
        </svg>
      </div>

      <code class="gate-readout">
        NOT({{ hovered?.input ?? "-" }}) = {{ hovered?.out ?? "-" }}
      </code>
    </div>
  </div>
</template>