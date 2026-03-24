<script setup lang="ts">
import './gate.css'
import { ref } from 'vue'
import TruthTable, { type TruthTableRow } from '../table/TruthTable.vue'

import GateLabel from './parts/GateLabel.vue'
import WireElbow from './parts/WireElbow.vue'
import NotBody from './parts/NotBody.vue'
import MuxBody from './parts/MuxBody.vue'
import {
  LABEL_IN_X,
  LABEL_OUT_X,
  LABEL_A_Y,
  LABEL_B_Y,
  LABEL_OUT_Y,
  VIEW_H,
  VIEW_W,
  VIEW_GATE_OFFSET_X,
  VIEW_GATE_OFFSET_Y
} from './parts/gateGeometry.js'
import Wire from "./parts/Wire.vue";

type MuxRow = TruthTableRow & {
  sel: number
  a: number
  b: number
  rowClass: string
}

const hovered = ref<MuxRow | null>(null)

const columns = [ 'sel', 'a', 'b']

const rows: MuxRow[] = [
  { sel: 0, a: 1, b: 0, rowClass: 'truth-true' },
  { sel: 1, a: 0, b: 1, rowClass: 'truth-true' }
]

function selOn(row: MuxRow | null) {
  return row?.sel === 1
}

function aOn(row: MuxRow | null) {
  return row?.a === 1
}

function bOn(row: MuxRow | null) {
  return row?.b === 1
}

function inOn(row: MuxRow | null) {
  return row?.a === 1 || row?.b === 1
}

function notSel(row: MuxRow | null) {
  if (!row) return false
  return row.sel === 0
}

function topAndOut(row: MuxRow | null) {
  if (!row) return false
  return row.a === 1 && row.sel === 0
}

function bottomAndOut(row: MuxRow | null) {
  if (!row) return false
  return row.b === 1 && row.sel === 1
}

</script>

<template>
  <div class="gate-identity">
    <div class="gate-identity-label">Boolean Identity</div>
    <code class="gate-identity-formula">(in ∧ ¬sel) = a, (in ∧ sel) = b</code>
  </div>

  <div class="gate-demo">
    <TruthTable
        :columns="columns"
        :rows="rows"
        :hovered-row="hovered"
        @row-enter="hovered = $event as MuxRow"
        @row-leave="hovered = null"
    />

    <div class="gate-visual">
      <div class="gate-svg-wrap">
        <svg
            class="gate-svg"
            :viewBox="`0 0 ${VIEW_W} ${VIEW_H}`"
            role="img"
            aria-label="2-to-1 multiplexer implemented with one NOT gate and three NAND gates"
        >
          <!-- labels -->
          <GateLabel :x="LABEL_OUT_X" :y="LABEL_A_Y" text="A" />
          <GateLabel :x="LABEL_OUT_X" :y="LABEL_B_Y" text="B" />
          <GateLabel :x="LABEL_IN_X" :y="LABEL_OUT_Y" text="In" class="gate-text-out" />
          <GateLabel :x="210" :y="-10" text="Sel" />

          <!-- internal NAND implementation -->
          <g transform="translate(210 67)  scale(0.18)">
            <!-- top = (a ∧ ¬sel) -->
            <g transform="translate(30 55)">
              <AndBody
                  :a-on="inOn(hovered)"
                  :b-on="notSel(hovered)"
                  :out-on="topAndOut(hovered)"
                  :out-x2="250"
              />
            </g>

            <!-- ¬(sel) -->
            <g transform="translate(-90 110) scale(0.75)">
              <NotBody
                  :in-on="selOn(hovered)"
                  :out-on="notSel(hovered)"
                  :out-x2="200"
              />
            </g>

            <!-- bottom = (b ∧ sel) -->
            <g transform="translate(30 295)">
              <AndBody
                  :a-on="inOn(hovered)"
                  :b-on="selOn(hovered)"
                  :out-on="bottomAndOut(hovered)"
                  :out-x2="250"
              />
            </g>

            <!-- In into top/btm AND -->
            <WireC
                :cx="-250"
                :y1="80"
                :y2="321"
                :dx="300"
                :on="inOn(hovered)"
            />
            <Wire :x1="-350" :y1="238" :x2="-250" :y2="238" :on="inOn(hovered)" />

            <!-- Sel into Feeder -->
            <WireElbow
                :x1="-100"
                :y1="-50"
                :x2="75"
                :y2="-150"
                :on="selOn(hovered)"
            />

            <!-- Feeder into top/btm AND -->
            <WireElbow
                :x1="30"
                :y1="397"
                :x2="-100"
                :y2="-50"
                :on="selOn(hovered)"
            />

            <!-- top AND to A -->
            <WireElbow
                :x1="500"
                :y1="27"
                :x2="290"
                :y2="120"
                :on="topAndOut(hovered)"
            />

            <!-- bottom AND to B -->
            <WireElbow
                :x1="500"
                :y1="450"
                :x2="290"
                :y2="360"
                :on="bottomAndOut(hovered)"
            />
          </g>

          <!-- outer mux symbol -->
          <g :transform="`translate(${VIEW_GATE_OFFSET_X} ${VIEW_GATE_OFFSET_Y})`">
            <MuxBody
                mode="demux"
                :top-on="aOn(hovered)"
                :bottom-on="bOn(hovered)"
                :select-on="selOn(hovered)"
                :mid-on="inOn(hovered)"
                :out-x2="280"
            />
          </g>
        </svg>
      </div>

      <code class="gate-readout">
        DMUX({{ hovered?.sel ?? "sel" }}) = {{ hovered?.a ?? "-" }}
      </code>
    </div>
  </div>
</template>