<script setup lang="ts">
import AndBody from './AndBody.vue'
import GateInversionBubble from './GateInversionBubble.vue'

import {
  NAND_BUBBLE_CX,
  NAND_BUBBLE_CY,
  NAND_BUBBLE_R,
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
  <g class="nand-symbol">
    <AndBody
        :a-on="aOn"
        :b-on="bOn"
        :out-on="outOn"
        :out-x2="outX2"
    />
    <GateInversionBubble
        :cx="NAND_BUBBLE_CX"
        :cy="NAND_BUBBLE_CY"
        :r="NAND_BUBBLE_R"
    />
  </g>
</template>