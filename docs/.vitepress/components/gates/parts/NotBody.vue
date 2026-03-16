<script setup lang="ts">
import GateInversionBubble from './GateInversionBubble.vue'
import GateWire from './GateWire.vue'
import {
  IN_X0,
  IN_X1,
  OUT_X0,
  OUT_X1,
  IN_SINGLE_Y,
  MID_Y,
  BODY_X,
  NOT_TOP_Y,
  NOT_BOTTOM_Y,
  NOT_TIP_X,
  NOT_BUBBLE_CX,
  NOT_BUBBLE_CY,
  NOT_BUBBLE_R
} from './gateGeometry'

withDefaults(defineProps<{
  inOn?: boolean
  outOn?: boolean
  outX2?: number
}>(), {
  outX2: OUT_X1
})
</script>

<template>
  <g class="not-symbol">
    <GateWire :x1="IN_X0" :y1="IN_SINGLE_Y" :x2="IN_X1" :y2="IN_SINGLE_Y" :on="inOn" />
    <GateWire :x1="OUT_X0" :y1="MID_Y" :x2="outX2" :y2="MID_Y" :on="outOn" />

    <path
        :d="`M${BODY_X} ${NOT_TOP_Y} L${BODY_X} ${NOT_BOTTOM_Y} L${NOT_TIP_X} ${MID_Y} Z`"
        class="gate-body"
    />

    <GateInversionBubble :cx="NOT_BUBBLE_CX" :cy="NOT_BUBBLE_CY" :r="NOT_BUBBLE_R" />
  </g>
</template>