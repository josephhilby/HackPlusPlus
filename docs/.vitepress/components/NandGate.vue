<script setup>
import './truthtable.css'
import { ref } from 'vue'

const hovered = ref(null)

const rows = [
  { a: 0, b: 0, out: 1, rowClass: 'truth-true' },
  { a: 0, b: 1, out: 1, rowClass: 'truth-true' },
  { a: 1, b: 0, out: 1, rowClass: 'truth-true' },
  { a: 1, b: 1, out: 0, rowClass: 'truth-false' }
]
</script>

<template>
  <div class="gate-demo">
    <div class="gate-visual">
      <code>
        NAND(a={{ hovered?.a ?? "-" }}, b={{ hovered?.b ?? "-" }}) = {{ hovered?.out ?? "-"}}
      </code>
    </div>

    <table class="truth-table">
      <thead>
      <tr>
        <th>a</th>
        <th>b</th>
        <th>out</th>
      </tr>
      </thead>

      <tbody>
      <tr
          v-for="(row,i) in rows"
          :key="i"
          :class="row.rowClass"
          @mouseenter="hovered = row"
          @mouseleave="hovered = null"
      >
        <td>{{row.a}}</td>
        <td>{{row.b}}</td>
        <td>{{row.out}}</td>
      </tr>
      </tbody>
    </table>

  </div>
</template>
