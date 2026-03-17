<script setup lang="ts">
defineProps<{
  x1: number
  y1: number
  x2: number
  y2: number
  on?: boolean
}>()
</script>

<template>
  <line
      :x1="x1"
      :y1="y1"
      :x2="x2"
      :y2="y2"
      :class="['gate-wire', on ? 'is-on' : 'is-off']"
  />
</template>