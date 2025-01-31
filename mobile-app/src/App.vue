<template>
  <ion-page>
    <ion-header>
      <ion-toolbar>
        <ion-title>IoT Dashboard</ion-title>
      </ion-toolbar>
    </ion-header>
    <ion-content>
      <LineChart
        title="Temperature"
        :data="temperature"
        scale-y="Temperature (°C)"
        :new-data="lastTemperature"
      />
      <LineChart
        title="Ambient Light"
        :data="ambientLight"
        scale-y="Light (lux)"
        :new-data="lastLight"
      />
      <LineChart
        title="Ambient Noise"
        :data="ambientNoise"
        scale-y="Noise (dB)"
        :new-data="lastNoise"
      />
      <ion-card>
        <ion-card-header>
          <ion-card-title>Controllo LED</ion-card-title>
        </ion-card-header>
        <ion-card-content>
          <ion-toggle v-model="ledStatus" @ionChange="toggleLed"></ion-toggle>
          <p>LED {{ ledStatus ? "Acceso" : "Spento" }}</p>
        </ion-card-content>
      </ion-card>
    </ion-content>
  </ion-page>
</template>

<script setup>
import { ref, onMounted, onBeforeUnmount } from "vue";
import {
  IonCard,
  IonCardContent,
  IonCardHeader,
  IonCardTitle,
  IonContent,
  IonHeader,
  IonPage,
  IonTitle,
  IonToolbar,
  IonToggle,
} from "@ionic/vue";
import LineChart from "./components/LineChart.vue";
import ApiService from "./api/request.js";

const temperature = {
  labels: [],
  datasets: [
    {
      backgroundColor: "rgba(75, 192, 192, 0.2)",
      borderColor: "rgba(75, 192, 192, 1)",
      data: [],
      tension: 0.4,
      fill: true,
    },
  ],
};
const ambientLight = {
  labels: [],
  datasets: [
    {
      backgroundColor: "rgba(255, 153, 0, 0.2)",
      borderColor: "rgba(255, 153, 0, 1)",
      data: [],
      tension: 0.4,
      fill: true,
    },
  ],
};
const ambientNoise = {
  labels: [],
  datasets: [
    {
      backgroundColor: "rgba(206, 77, 255, 0.2)",
      borderColor: "rgba(206, 77, 255, 1)",
      data: [],
      tension: 0.4,
      fill: true,
    },
  ],
};

const ledStatus = ref(false);
const lastTemperature = ref(null);
const lastLight = ref(null);
const lastNoise = ref(null);

const toggleLed = () => {
  console.log(ledStatus.value);
  ApiService.toggleLed(ledStatus.value ? "on" : "off");
};

const requestData = () => {
  ApiService.getData("temperature").then((data) => {
    lastTemperature.value = data.value;
  });
  ApiService.getData("light").then((data) => {
    lastLight.value = data.value;
  });
  ApiService.getData("noise").then((data) => {
    lastNoise.value = data.value;
  });
};

requestData();

onMounted(() => {
  setInterval(requestData, 5000);
});
</script>

<style scoped>
ion-card {
  margin: 20px;
}
</style>
