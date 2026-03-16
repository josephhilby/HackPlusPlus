<script setup lang="ts">
defineProps<{
  cx: number
  cy: number
  r?: number
}>()
</script>

<template>
  <circle
      :cx="cx"
      :cy="cy"
      :r="r ?? 10"
      class="gate-bubble"
  />
</template>