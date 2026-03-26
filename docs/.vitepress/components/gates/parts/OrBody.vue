<script setup lang="ts">
import Wire from './Wire.vue'
import {
  IN_X0,
  OUT_X1,
  IN_TOP_Y,
  IN_BOTTOM_Y,
  MID_Y,
  BODY_X,
  GATE_H,
  OR_IN_X1,
  OR_OUT_X0,
  OR_BACK_CTRL_X,
  OR_LOBE_CTRL_X,
  OR_TIP_X,
  OR_TOP_CTRL_Y,
  OR_BOTTOM_CTRL_Y
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
  <g class="or-symbol">
    <!-- inputs -->
    <Wire
        :x1="IN_X0"
        :y1="IN_TOP_Y"
        :x2="OR_IN_X1"
        :y2="IN_TOP_Y"
        :on="aOn"
    />

    <Wire
        :x1="IN_X0"
        :y1="IN_BOTTOM_Y"
        :x2="OR_IN_X1"
        :y2="IN_BOTTOM_Y"
        :on="bOn"
    />

    <!-- output -->
    <Wire
        :x1="OR_OUT_X0"
        :y1="MID_Y"
        :x2="outX2"
        :y2="MID_Y"
        :on="outOn"
    />

    <!-- body -->
    <path
        :d="`
        M${BODY_X} 0
        Q${OR_BACK_CTRL_X} ${MID_Y} ${BODY_X} ${GATE_H}
        Q${OR_LOBE_CTRL_X} ${OR_BOTTOM_CTRL_Y} ${OR_TIP_X} ${MID_Y}
        Q${OR_LOBE_CTRL_X} ${OR_TOP_CTRL_Y} ${BODY_X} 0
        Z
      `"
        class="gate-body"
    />
  </g>
</template>