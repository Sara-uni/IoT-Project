<template>
  <ion-card>
    <ion-card-header>
      <ion-grid :fixed="true">
        <ion-row class="ion-justify-content-between">
          <ion-col>
            <ion-card-title>{{ props.title }}</ion-card-title>
          </ion-col>
          <ion-col size="auto">
            <IonIcon
              v-if="!realtime"
              :icon="refreshCircle"
              @click="setRealtime"
              size="large"
            ></IonIcon>
          </ion-col>
        </ion-row>
      </ion-grid>
    </ion-card-header>
    <ion-card-content>
      <div style="width: 100%; max-width: 1000px">
        <Line
          v-if="props.newData"
          :data="chartData"
          :options="options"
          ref="chartRef"
        />
        <p v-else>There is no data to display.</p>
      </div>
    </ion-card-content>
  </ion-card>
</template>

<script setup>
import { computed, watch, ref, nextTick } from "vue";
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
import zoomPlugin from "chartjs-plugin-zoom";
import {
  IonIcon,
  IonCard,
  IonCardTitle,
  IonCardHeader,
  IonCardContent,
  IonRow,
  IonCol,
  IonGrid,
} from "@ionic/vue";
import { refreshCircle } from "ionicons/icons";

ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  Filler,
  zoomPlugin
);

const props = defineProps({
  title: {
    type: String,
    required: true,
  },
  data: {
    type: Object,
    required: true,
  },
  scaleY: {
    type: String,
  },
  unit: {
    type: String,
  },
  newData: {
    type: Number,
  },
  newTime: {
    type: Date,
  },
});

const chartRef = ref(null);
const chartData = { ...props.data };
const realtime = ref(true);
const maxDataPoints = 15;

const setRealtime = () => {
  realtime.value = true;
  goToLatestData();
};

const options = computed(() => ({
  responsive: true,
  scales: {
    x: {
      title: {
        display: true,
        text: "Time",
      },
      ticks: {
        maxTicksLimit: maxDataPoints / 3,
      },
      min: 0,
      max: maxDataPoints,
    },
    y: {
      title: {
        display: true,
        text: props.scaleY,
      },
    },
  },
  interaction: {
    mode: "nearest",
    intersect: false,
  },
  plugins: {
    legend: { display: false },
    title: { display: false },
    tooltip: {
      callbacks: {
        label: function (context) {
          let value = context.raw;
          return `${value} ${props.unit}`;
        },
      },
    },
    zoom: {
      pan: {
        enabled: true,
        mode: "x",
        speed: 10,
        threshold: 10,
        onPanStart: () => {
          if (realtime.value) {
            realtime.value = false;
          }
        },
      },
    },
  },
}));

const goToLatestData = () => {
  nextTick(() => {
    const chart = chartRef.value.chart;
    const totalDataPoints = chartData.labels.length;

    if (totalDataPoints > 0) {
      chart.scales.x.options.min =
        totalDataPoints - maxDataPoints > 0
          ? totalDataPoints - maxDataPoints
          : 0;
      chart.scales.x.options.max = totalDataPoints - 1;

      chart.update();
    }
  });
};

watch(
  () => props.newTime,
  () => {
    if (chartRef.value && props.newData) {
      chartData.labels.push(props.newTime);
      chartData.datasets[0].data.push(props.newData);
      chartRef.value.chart.update();

      if (realtime.value) {
        nextTick(() => {
          const chart = chartRef.value.chart;
          const totalDataPoints = chartData.datasets[0].data.length;

          if (totalDataPoints > maxDataPoints) {
            const currentMax = chart.scales.x.max;
            const newMax = Math.min(currentMax + 1, totalDataPoints - 1);
            chart.scales.x.options.max = newMax;
            chart.scales.x.options.min = newMax - maxDataPoints;
            chart.update();
          }
        });
      }
    }
  }
);
</script>
