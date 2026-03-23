<script setup lang="ts">
import Wire from './Wire.vue'
import {
  GATE_H,
  IN_TOP_Y,
  IN_BOTTOM_Y,
  BODY_X,
  OUT_X1,
  MUX_BODY_RIGHT_X,
  MUX_SELECT_X,
  MUX_SELECT_Y1,
  MUX_SELECT_Y2,
  MUX_LEFT_X0,
  MUX_LEFT_X1,
  MUX_RIGHT_X0,
  MUX_MID_IO_Y
} from './gateGeometry'

withDefaults(defineProps<{
  mode?: 'mux' | 'demux'
  topOn?: boolean
  bottomOn?: boolean
  midOn?: boolean
  selectOn?: boolean
  outX2?: number
}>(), {
  mode: 'mux',
  outX2: OUT_X1
})

const LEFT_X = BODY_X - 25
const RIGHT_X = MUX_BODY_RIGHT_X

/**
 * Amount each slanted side shifts vertically-aligned corners inward.
 * Increase for a steeper wedge.
 */
const SLOPE_INSET = 20
</script>

<template>
  <g class="mux-demux-symbol">

    <!-- MUX: 2 inputs → 1 output -->
    <template v-if="mode === 'mux'">
      <Wire
          :x1="MUX_LEFT_X0"
          :y1="IN_TOP_Y"
          :x2="MUX_LEFT_X1"
          :y2="IN_TOP_Y"
          :on="topOn"
      />

      <Wire
          :x1="MUX_LEFT_X0"
          :y1="IN_BOTTOM_Y"
          :x2="MUX_LEFT_X1"
          :y2="IN_BOTTOM_Y"
          :on="bottomOn"
      />

      <Wire
          :x1="MUX_RIGHT_X0"
          :y1="MUX_MID_IO_Y"
          :x2="outX2"
          :y2="MUX_MID_IO_Y"
          :on="midOn"
      />

      <!-- narrow left, wide right -->
      <path
          :d="`
          M${LEFT_X + SLOPE_INSET} 0
          L${RIGHT_X} ${SLOPE_INSET}
          L${RIGHT_X} ${GATE_H - SLOPE_INSET}
          L${LEFT_X + SLOPE_INSET} ${GATE_H}
          Z
        `"
          class="gate-body"
      />
    </template>

    <!-- DEMUX: 1 input → 2 outputs -->
    <template v-else>
      <Wire
          :x1="MUX_LEFT_X0"
          :y1="MUX_MID_IO_Y"
          :x2="MUX_LEFT_X1"
          :y2="MUX_MID_IO_Y"
          :on="midOn"
      />

      <Wire
          :x1="MUX_RIGHT_X0"
          :y1="IN_TOP_Y"
          :x2="outX2"
          :y2="IN_TOP_Y"
          :on="topOn"
      />

      <Wire
          :x1="MUX_RIGHT_X0"
          :y1="IN_BOTTOM_Y"
          :x2="outX2"
          :y2="IN_BOTTOM_Y"
          :on="bottomOn"
      />

      <!-- wide left, narrow right -->
      <path
          :d="`
          M${LEFT_X} ${SLOPE_INSET}
          L${RIGHT_X - SLOPE_INSET} 0
          L${RIGHT_X - SLOPE_INSET} ${GATE_H}
          L${LEFT_X} ${GATE_H - SLOPE_INSET}
          Z
        `"
          class="gate-body"
      />
    </template>

    <!-- Select line -->
    <Wire
        :x1="MUX_SELECT_X"
        :y1="MUX_SELECT_Y1"
        :x2="MUX_SELECT_X"
        :y2="MUX_SELECT_Y2"
        :on="selectOn"
    />
  </g>
</template>