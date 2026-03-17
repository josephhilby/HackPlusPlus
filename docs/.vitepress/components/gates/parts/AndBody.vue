<script setup lang="ts">
import Wire from './Wire.vue'
import {
  IN_X0,
  IN_X1,
  IN_TOP_Y,
  IN_BOTTOM_Y,
  MID_Y,
  BODY_X,
  GATE_H,
  OUT_X1,
  AND_BODY_RIGHT_X,
  AND_BODY_RADIUS,
  AND_OUT_X0
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
    <Wire :x1="IN_X0" :y1="IN_TOP_Y" :x2="IN_X1" :y2="IN_TOP_Y" :on="aOn" />
    <Wire :x1="IN_X0" :y1="IN_BOTTOM_Y" :x2="IN_X1" :y2="IN_BOTTOM_Y" :on="bOn" />
    <Wire :x1="AND_OUT_X0" :y1="MID_Y" :x2="outX2" :y2="MID_Y" :on="outOn" />

    <path
        :d="`
        M${BODY_X} 0
        L${AND_BODY_RIGHT_X} 0
        A${AND_BODY_RADIUS} ${AND_BODY_RADIUS} 0 0 1 ${AND_BODY_RIGHT_X} ${GATE_H}
        L${BODY_X} ${GATE_H}
        Z
      `"
        class="gate-body"
    />
  </g>
</template>