<script setup lang="ts">
import AndBody from './AndBody.vue'
import GateInversionBubble from './GateInversionBubble.vue'

defineProps<{
  aOn?: boolean
  bOn?: boolean
  outOn?: boolean
}>()
</script>

<template>
  <g class="nand-symbol">
    <AndBody :a-on="aOn" :b-on="bOn" :out-on="outOn" />
    <GateInversionBubble :cx="190" :cy="64" :r="10" />
  </g>
</template>