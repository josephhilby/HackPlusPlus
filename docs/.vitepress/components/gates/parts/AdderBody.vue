<script setup lang="ts">
import Wire from "./Wire.vue";
import {
  IN_X0,
  IN_X1,
  IN_TOP_Y,
  IN_BOTTOM_Y,
  MID_Y,
  ADDER_BODY_LEFT_X as BODY_X,
  GATE_H,
  OUT_X1,
  ADDER_WIDTH,
  ADDER_X1,
  ADDER_SUM_Y,
  ADDER_CARRY_Y,
} from "./gateGeometry";

withDefaults(
  defineProps<{
    aOn?: boolean;
    bOn?: boolean;
    cOn?: boolean;
    sumOn?: boolean;
    carryOn?: boolean;
    isFull?: boolean;
  }>(),
  {
    isFull: false,
    aOn: false,
    bOn: false,
    cOn: false,
    sumOn: false,
    carryOn: false,
  },
);
</script>

<template>
  <g class="adder-symbol">
    <!-- Input Wires -->
    <Wire :x1="IN_X0" :y1="IN_TOP_Y" :x2="BODY_X" :y2="IN_TOP_Y" :on="aOn" />
    <Wire
      v-if="isFull"
      :x1="IN_X0"
      :y1="MID_Y"
      :x2="BODY_X"
      :y2="MID_Y"
      :on="bOn"
    />
    <Wire
      v-if="!isFull"
      :x1="IN_X0"
      :y1="IN_BOTTOM_Y"
      :x2="BODY_X"
      :y2="IN_BOTTOM_Y"
      :on="bOn"
    />
    <Wire
      v-if="isFull"
      :x1="IN_X0"
      :y1="IN_BOTTOM_Y"
      :x2="BODY_X"
      :y2="IN_BOTTOM_Y"
      :on="cOn"
    />

    <!-- Output Wires -->
    <Wire
      :x1="ADDER_X1"
      :y1="ADDER_SUM_Y"
      :x2="OUT_X1"
      :y2="ADDER_SUM_Y"
      :on="sumOn"
    />
    <Wire
      :x1="ADDER_X1"
      :y1="ADDER_CARRY_Y"
      :x2="OUT_X1"
      :y2="ADDER_CARRY_Y"
      :on="carryOn"
    />

    <!-- Adder Block -->
    <rect
      :x="BODY_X"
      :y="0"
      :width="ADDER_WIDTH"
      :height="GATE_H"
      class="gate-body"
    />
  </g>
</template>
