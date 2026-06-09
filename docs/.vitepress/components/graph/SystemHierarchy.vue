<script setup lang="ts">
import "./graph.css";
import { computed } from "vue";
import { DEFAULT_GRAPH_DATA } from "./data";

const props = defineProps({
  graphData: {
    type: Object,
    default: () => DEFAULT_GRAPH_DATA,
  },
});

const formatListWithAmpersand = (items: string[] | string): string => {
  if (!Array.isArray(items)) return items;
  if (items.length <= 1) return items.join("");
  if (items.length === 2) return items.join(" & ");
  return items.slice(0, -1).join(", ") + " & " + items.slice(-1);
};

const generatedPaths = computed(() => {
  const rootX = 400;
  const rootY = 65;
  const childWidth = 250;
  const totalChildren = props.graphData.children.length;

  return props.graphData.children.map((_child: any, idx: number) => {
    const childX =
      totalChildren > 1
        ? 75 + idx * ((800 - 150 - childWidth) / (totalChildren - 1))
        : 275;

    const targetX = childX + childWidth / 2;
    const targetY = 110;

    return {
      d: `M ${rootX} ${rootY} L ${targetX} ${targetY}`,
      childX,
    };
  });
});
</script>

<template>
  <div class="system-hierarchy-container">
    <svg
      xmlns="http://www.w3.org/2000/svg"
      viewBox="0 0 800 240"
      class="hierarchy-svg"
    >
      <template v-for="path in generatedPaths" :key="path.d">
        <path :d="path.d" class="tree-line" />
      </template>

      <g transform="translate(275, 20)">
        <rect width="250" height="45" class="node-rect" />
        <text x="125" y="27" class="text-main">
          <tspan style="text-transform: uppercase">
            {{ props.graphData.root.type }}:
          </tspan>
          {{ props.graphData.root.name }}
        </text>
      </g>

      <template v-for="(child, idx) in props.graphData.children" :key="idx">
        <g :transform="`translate(${generatedPaths[idx].childX}, 110)`">
          <rect width="250" height="40" class="node-rect" />
          <text x="125" y="25" class="text-main">
            <tspan style="text-transform: uppercase">{{ child.type }}:</tspan>
            {{ child.name }}
          </text>
        </g>

        <template v-for="(detail, dIdx) in child.details" :key="dIdx">
          <text
            :x="generatedPaths[idx].childX"
            :y="185 + dIdx * 30"
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
