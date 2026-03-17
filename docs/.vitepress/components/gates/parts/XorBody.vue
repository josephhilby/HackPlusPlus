<script setup lang="ts">
import OrBody from './OrBody.vue'

import {
  BODY_X,
  GATE_H,
  MID_Y,
  OR_BACK_CTRL_X,
  XOR_BACK_OFFSET_X,
  OUT_X1
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
  <g class="xor-symbol">
    <!-- base OR shape -->
    <OrBody
        :a-on="aOn"
        :b-on="bOn"
        :out-on="outOn"
        :out-x2="outX2"
    />

    <!-- XOR leading curve (stroke only) -->
    <path
        :d="`
        M${BODY_X - XOR_BACK_OFFSET_X} 0
        Q${OR_BACK_CTRL_X - XOR_BACK_OFFSET_X} ${MID_Y}
         ${BODY_X - XOR_BACK_OFFSET_X} ${GATE_H}
      `"
        class="gate-body"
    />
  </g>
</template>