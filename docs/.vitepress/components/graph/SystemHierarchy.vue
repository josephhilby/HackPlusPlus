<script setup lang="ts">
import "./graph.css";
import { computed } from "vue";
import {
  DEFAULT_GRAPH_DATA,
  HARDWARE_GRAPH_DATA,
  SOFTWARE_GRAPH_DATA,
} from "./data";

const props = defineProps({
  dataType: {
    type: String,
    required: true,
  },
});

const DATA_MAP: Record<string, any> = {
  default: DEFAULT_GRAPH_DATA,
  hardware: HARDWARE_GRAPH_DATA,
  software: SOFTWARE_GRAPH_DATA,
};

const graphData = computed(
  () => DATA_MAP[props.dataType] || DEFAULT_GRAPH_DATA,
);

const formatListWithAmpersand = (items: string[] | string): string => {
  if (!Array.isArray(items)) return items;
  if (items.length <= 1) return items.join("");
  if (items.length === 2) return items.join(" & ");
  return items.slice(0, -1).join(", ") + " & " + items.slice(-1);
};

const generatedPaths = computed(() => {
  const rootX = 400;
  const rootY = 110;
  const childWidth = 250;
  const svgWidth = 800;

  const children = graphData.value.children;
  const totalChildren = children.length;

  return children.map((_child: any, idx: number) => {
    // Dynamic spacing logic:
    // 1. Calculate the total width occupied by all child boxes
    // 2. Divide the remaining space equally to create margins
    const gap = 20; // Space between nodes
    const totalWidth = totalChildren * childWidth + (totalChildren - 1) * gap;
    const startX = (svgWidth - totalWidth) / 2;

    const childX = startX + idx * (childWidth + gap);
    const targetY = 170;

    return {
      d: `M ${rootX} ${rootY} L ${childX + childWidth / 2} ${targetY}`,
      childX,
      targetY,
    };
  });
});
</script>

<template>
  <div class="system-hierarchy-container">
    <svg
      xmlns="http://www.w3.org/2000/svg"
      viewBox="0 0 800 320"
      class="hierarchy-svg"
    >
      <template v-for="path in generatedPaths" :key="path.d">
        <path :d="path.d" class="tree-line" />
      </template>

      <g transform="translate(275, 20)">
        <rect width="250" height="90" class="node-rect" />
        <text x="125" y="27" class="text-main" text-anchor="middle">
          <tspan style="text-transform: uppercase">
            {{ graphData.root.type }}:
          </tspan>
          {{ graphData.root.name }}
        </text>

        <template v-for="(detail, dIdx) in graphData.root.details" :key="dIdx">
          <text x="15" :y="52 + dIdx * 22" text-anchor="start" class="text-sub">
            <tspan v-if="detail.prefix" class="label-prefix">
              {{ detail.prefix }}
            </tspan>
            {{ formatListWithAmpersand(detail.value) }}
          </text>
        </template>
      </g>

      <template v-for="(child, idx) in graphData.children" :key="idx">
        <g
          :transform="`translate(${generatedPaths[idx].childX}, ${generatedPaths[idx].targetY})`"
        >
          <rect width="250" height="90" class="node-rect" />
          <text x="125" y="27" class="text-main" text-anchor="middle">
            <tspan style="text-transform: uppercase">{{ child.type }}:</tspan>
            {{ child.name }}
          </text>
        </g>

        <template v-for="(detail, dIdx) in child.details" :key="dIdx">
          <text
            :x="generatedPaths[idx].childX + 15"
            :y="generatedPaths[idx].targetY + 50 + dIdx * 22"
            text-anchor="start"
            class="text-sub"
          >
            <tspan v-if="detail.prefix" class="label-prefix">
              {{ detail.prefix }}
            </tspan>
            {{ formatListWithAmpersand(detail.value) }}
          </text>
        </template>
      </template>
    </svg>
  </div>
</template>
