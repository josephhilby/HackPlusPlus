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
  // Pushed rootY down slightly because the root card is taller now
  const rootY = 110;
  const childWidth = 250;
  const totalChildren = props.graphData.children.length;

  return props.graphData.children.map((_child: any, idx: number) => {
    const childX =
      totalChildren > 1
        ? 75 + idx * ((800 - 150 - childWidth) / (totalChildren - 1))
        : 275;

    const targetX = childX + childWidth / 2;
    // Pushed children down to Y=170 to give the root details breathing room
    const targetY = 170;

    return {
      d: `M ${rootX} ${rootY} L ${targetX} ${targetY}`,
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
            {{ props.graphData.root.type }}:
          </tspan>
          {{ props.graphData.root.name }}
        </text>

        <template
          v-for="(detail, dIdx) in props.graphData.root.details"
          :key="dIdx"
        >
          <text x="15" :y="52 + dIdx * 22" text-anchor="start" class="text-sub">
            <tspan v-if="detail.prefix" class="label-prefix">
              {{ detail.prefix }}
            </tspan>
            {{ formatListWithAmpersand(detail.value) }}
          </text>
        </template>
      </g>

      <template v-for="(child, idx) in props.graphData.children" :key="idx">
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
