<template>
  <Line :data="props.data" :options="options" />
</template>
<script setup>
import { computed } from "vue";
import { Line } from "vue-chartjs";
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
} from "chart.js";

ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend
);

const options = computed(() => {
  return {
    responsive: true,
    plugins: {
      legend: {
        display: false,
      },
      title: {
        display: false,
      },
    },
    scales: {
      x: {
        title: {
          display: true,
          text: "Time",
        },
      },
      y: {
        title: {
          display: true,
          text: props.scaleY,
        },
      },
    },
  };
});

const props = defineProps({
  data: {
    type: Object,
    required: true,
  },
  scaleY: {
    type: String,
  },
  minY: {
    type: Number,
    default: 0,
  },
  maxY: {
    type: Number,
    default: 100,
  },
});
</script>
