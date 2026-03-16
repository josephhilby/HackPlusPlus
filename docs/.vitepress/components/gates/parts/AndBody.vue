<script setup lang="ts">
import GateWire from './GateWire.vue'

defineProps<{
  aOn?: boolean
  bOn?: boolean
  outOn?: boolean
}>()
</script>

<template>
  <g class="and-symbol">
    <GateWire :x1="0" :y1="26" :x2="88" :y2="26" :on="aOn" />
    <GateWire :x1="0" :y1="102" :x2="88" :y2="102" :on="bOn" />
    <GateWire :x1="148" :y1="64" :x2="260" :y2="64" :on="outOn" />

    <path
        d="M78 0 L148 0 A72 72 0 0 1 148 128 L78 128 Z"
        class="gate-body"
    />
  </g>
</template>