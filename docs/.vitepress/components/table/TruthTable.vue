<script setup lang="ts">
export type TruthTableRow = Record<string, string | number | boolean | null>

const props = defineProps<{
  columns: string[]
  rows: TruthTableRow[]
  hoveredRow?: TruthTableRow | null
}>()

const emit = defineEmits<{
  (e: 'row-enter', row: TruthTableRow): void
  (e: 'row-leave'): void
}>()

function rowClass(row: TruthTableRow) {
  return row.rowClass ?? ''
}

function isHovered(row: TruthTableRow) {
  return props.hoveredRow === row
}
</script>

<template>
  <div class="truth-table-wrap">
    <table class="truth-table">
      <thead>
      <tr>
        <th v-for="column in columns" :key="column">
          {{ column }}
        </th>
      </tr>
      </thead>

      <tbody>
      <tr
          v-for="(row, i) in rows"
          :key="i"
          :class="[rowClass(row), { hovered: isHovered(row) }]"
          @mouseenter="emit('row-enter', row)"
          @mouseleave="emit('row-leave')"
      >
        <td v-for="column in columns" :key="column">
          {{ row[column] }}
        </td>
      </tr>
      </tbody>
    </table>
  </div>
</template>

<style scoped>
@import './truthtable.css';
</style>