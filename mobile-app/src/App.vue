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
  console.log(`LED Status: ${ledStatus.value ? "On" : "Off"}`);
};

// let socket;
// const ip_address = "192.168.1.110";
// onMounted(() => {
//   socket = new WebSocket("wss://" + ip_address + ":8080");

//   socket.onmessage = (event) => {
//     const messageJson = JSON.parse(event.data);
//     if (messageJson.type === "temperature") {
//       lastTemperature.value = messageJson.value;
//     } else if (messageJson.type === "light") {
//       lastLight.value = messageJson.value;
//     } else if (messageJson.type === "noise") {
//       lastNoise.value = messageJson.value;
//     }
//   };
// });

// onBeforeUnmount(() => {
//   if (socket) {
//     socket.close();
//   }
// });

onMounted(() => {
  setInterval(simulateMessages, 1000);
});

const getRandomTemperature = () => {
  return parseFloat((Math.random() * (30 - 15) + 15).toFixed(2));
};

const getRandomLightLevel = () => {
  return parseFloat((Math.random() * (1000 - 100) + 100).toFixed(2));
};

const getRandomNoiseLevel = () => {
  return parseFloat((Math.random() * (80 - 30) + 30).toFixed(2));
};

const simulateMessages = () => {
  let rand = Math.floor(Math.random() * 3) + 1;
  switch (rand) {
    case 1:
      lastTemperature.value = getRandomTemperature();
      break;
    case 2:
      lastLight.value = getRandomLightLevel();
      break;
    case 3:
      lastNoise.value = getRandomNoiseLevel();
      break;
  }
};
</script>

<style scoped>
ion-card {
  margin: 20px;
}
</style>
