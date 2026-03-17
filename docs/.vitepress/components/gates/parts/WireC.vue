<script setup lang="ts">
import GateWire from './Wire.vue'

withDefaults(defineProps<{
  cx: number
  y1: number
  y2: number
  dx: number
  on?: boolean
  side?: 'left' | 'right'
}>(), {
  side: 'right'
})
</script>

<template>
  <g class="wire-c">
    <!-- vertical spine -->
    <Wire
        :x1="cx"
        :y1="y1"
        :x2="cx"
        :y2="y2"
        :on="on"
    />

    <!-- top prong -->
    <Wire
        :x1="side === 'right' ? cx : cx - dx"
        :y1="y1"
        :x2="side === 'right' ? cx + dx : cx"
        :y2="y1"
        :on="on"
    />

    <!-- bottom prong -->
    <Wire
        :x1="side === 'right' ? cx : cx - dx"
        :y1="y2"
        :x2="side === 'right' ? cx + dx : cx"
        :y2="y2"
        :on="on"
    />
  </g>
</template>