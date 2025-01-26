<template>
  <ion-page>
    <ion-header>
      <ion-toolbar>
        <ion-title>IoT Dashboard</ion-title>
      </ion-toolbar>
    </ion-header>
    <ion-content>
      <ion-card>
        <ion-card-header>
          <ion-card-title>Sensore Temperatura</ion-card-title>
        </ion-card-header>
        <ion-card-content>
          <ion-label>Temperatura attuale:</ion-label>
          <h2>{{ temperature }} °C</h2>
          <ion-button @click="refreshTemperature">Aggiorna</ion-button>
        </ion-card-content>
      </ion-card>
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
import { ref } from "vue";
import {
  IonButton,
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
  IonLabel,
} from "@ionic/vue";

const temperature = ref(22.5);
const ledStatus = ref(false);

const refreshTemperature = () => {
  // Simula un'API IoT per ottenere la temperatura
  temperature.value = (Math.random() * 10 + 20).toFixed(1);
};

const toggleLed = () => {
  // Simula l'invio dello stato LED all'API IoT
  console.log(`LED Status: ${ledStatus.value ? "On" : "Off"}`);
};
</script>

<style scoped>
ion-card {
  margin: 20px;
}
</style>
