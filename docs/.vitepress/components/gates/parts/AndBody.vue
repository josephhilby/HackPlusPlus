<script setup lang="ts">
import GateWire from './GateWire.vue'
import {
  IN_X0,
  IN_X1,
  OUT_X0,
  OUT_X1,
  IN_TOP_Y,
  IN_BOTTOM_Y,
  MID_Y,
  BODY_X,
  BODY_RIGHT_X,
  BODY_RADIUS,
  GATE_H
} from './gateGeometry'

withDefaults(defineProps<{
  aOn?: boolean
  bOn?: boolean
  outOn?: boolean
  outX2?: number
}>(), {
  outX2: OUT_X1
})
</script>

<template>
  <g class="and-symbol">
    <GateWire :x1="IN_X0" :y1="IN_TOP_Y" :x2="IN_X1-1" :y2="IN_TOP_Y" :on="aOn" />
    <GateWire :x1="IN_X0" :y1="IN_BOTTOM_Y" :x2="IN_X1-1" :y2="IN_BOTTOM_Y" :on="bOn" />
    <GateWire :x1="OUT_X0+30" :y1="MID_Y" :x2="outX2" :y2="MID_Y" :on="outOn" />

    <path
        :d="`
        M${BODY_X} 0
        L${BODY_RIGHT_X} 0 A${BODY_RADIUS} ${BODY_RADIUS} 0 0 1
        ${BODY_RIGHT_X} ${GATE_H} L${BODY_X} ${GATE_H}
        Z
        `"
        class="gate-body"
    />
  </g>
</template>