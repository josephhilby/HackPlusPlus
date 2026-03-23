<script setup lang="ts">
import Wire from './Wire.vue'
import {
  IN_TOP_Y,
  IN_BOTTOM_Y,
  OUT_X1,
  MUX_BODY_LEFT_X,
  MUX_BODY_RIGHT_X,
  MUX_BODY_TOP_Y,
  MUX_BODY_BOTTOM_Y,
  MUX_BODY_SLOPE_INSET,
  MUX_SELECT_X,
  MUX_SELECT_Y1,
  MUX_SELECT_Y2,
  MUX_LEFT_X0,
  MUX_LEFT_X1,
  MUX_RIGHT_X0,
  MUX_MID_IO_Y
} from './gateGeometry'

withDefaults(defineProps<{
  mode?: 'mux' | 'demux'
  topOn?: boolean
  bottomOn?: boolean
  midOn?: boolean
  selectOn?: boolean
  outX2?: number
}>(), {
  mode: 'mux',
  outX2: OUT_X1
})
</script>

<template>
  <g class="mux-demux-symbol">

    <Wire
        :x1="MUX_SELECT_X"
        :y1="MUX_SELECT_Y1"
        :x2="MUX_SELECT_X"
        :y2="MUX_SELECT_Y2"
        :on="selectOn"
    />

    <!-- MUX -->
    <template v-if="mode === 'mux'">
      <Wire
          :x1="MUX_LEFT_X0"
          :y1="IN_TOP_Y"
          :x2="MUX_LEFT_X1"
          :y2="IN_TOP_Y"
          :on="topOn"
      />

      <Wire
          :x1="MUX_LEFT_X0"
          :y1="IN_BOTTOM_Y"
          :x2="MUX_LEFT_X1"
          :y2="IN_BOTTOM_Y"
          :on="bottomOn"
      />

      <Wire
          :x1="MUX_RIGHT_X0"
          :y1="MUX_MID_IO_Y"
          :x2="outX2"
          :y2="MUX_MID_IO_Y"
          :on="midOn"
      />

      <path
          :d="`
          M${MUX_BODY_LEFT_X + MUX_BODY_SLOPE_INSET} ${MUX_BODY_TOP_Y}
          L${MUX_BODY_RIGHT_X} ${MUX_BODY_TOP_Y + MUX_BODY_SLOPE_INSET}
          L${MUX_BODY_RIGHT_X} ${MUX_BODY_BOTTOM_Y - MUX_BODY_SLOPE_INSET}
          L${MUX_BODY_LEFT_X + MUX_BODY_SLOPE_INSET} ${MUX_BODY_BOTTOM_Y}
          Z
        `"
          class="gate-body"
      />
    </template>

    <!-- DEMUX -->
    <template v-else>
      <Wire
          :x1="MUX_LEFT_X0"
          :y1="MUX_MID_IO_Y"
          :x2="MUX_LEFT_X1"
          :y2="MUX_MID_IO_Y"
          :on="midOn"
      />

      <Wire
          :x1="MUX_RIGHT_X0"
          :y1="IN_TOP_Y"
          :x2="outX2"
          :y2="IN_TOP_Y"
          :on="topOn"
      />

      <Wire
          :x1="MUX_RIGHT_X0"
          :y1="IN_BOTTOM_Y"
          :x2="outX2"
          :y2="IN_BOTTOM_Y"
          :on="bottomOn"
      />

      <path
          :d="`
          M${MUX_BODY_LEFT_X} ${MUX_BODY_TOP_Y + MUX_BODY_SLOPE_INSET}
          L${MUX_BODY_RIGHT_X - MUX_BODY_SLOPE_INSET} ${MUX_BODY_TOP_Y}
          L${MUX_BODY_RIGHT_X - MUX_BODY_SLOPE_INSET} ${MUX_BODY_BOTTOM_Y}
          L${MUX_BODY_LEFT_X} ${MUX_BODY_BOTTOM_Y - MUX_BODY_SLOPE_INSET}
          Z
        `"
          class="gate-body"
      />
    </template>
  </g>
</template>