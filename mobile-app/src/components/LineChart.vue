<template>
  <Line :data="chartData" :options="options" ref="chartRef" />
</template>
<script setup>
import { computed, watch, ref } from "vue";
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
  Filler,
} from "chart.js";

ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  Filler
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
  newData: {
    type: Number,
  },
});

const chartRef = ref(null);
const chartData = props.data;

watch(
  () => props.newData,
  () => {
    if (chartRef.value && props.newData) {
      chartData.labels.push(
        new Date().toLocaleTimeString([], {
          hour: "2-digit",
          minute: "2-digit",
        })
      );
      chartData.datasets[0].data.push(props.newData);
      chartRef.value.chart.update();
    }
  }
);
</script>
