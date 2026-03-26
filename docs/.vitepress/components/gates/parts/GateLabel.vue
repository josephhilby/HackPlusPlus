<script setup lang="ts">
defineProps<{
  x: number
  y: number
  text: string
  small?: boolean
}>()
</script>

<template>
  <text
      :x="x"
      :y="y"
      :class="['gate-text', { 'gate-text-small': small, 'gate-text-out': text === 'Out' }]"
  >
    {{ text }}
  </text>
</template>