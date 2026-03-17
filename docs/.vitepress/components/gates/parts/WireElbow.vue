<script setup lang="ts">
import Wire from './Wire.vue'

defineProps<{
  x1: number
  y1: number
  x2: number
  y2: number
  on?: boolean
}>()
</script>

<template>
  <g>
    <Wire :x1="x1" :y1="y1" :x2="x2" :y2="y1" :on="on" />
    <Wire :x1="x2" :y1="y1" :x2="x2" :y2="y2" :on="on" />
  </g>
</template>