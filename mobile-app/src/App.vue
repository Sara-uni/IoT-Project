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
        unit="°C"
        :new-data="lastTemperature"
      />
      <LineChart
        title="Ambient Light"
        :data="ambientLight"
        scale-y="Light (lux)"
        unit="lux"
        :new-data="lastLight"
      />
      <LineChart
        title="Ambient Noise"
        :data="ambientNoise"
        scale-y="Noise (dB)"
        unit="dB"
        :new-data="lastNoise"
      />
      <ion-card style="margin-bottom: 10rem">
        <ion-card-header>
          <ion-card-title>
            <ion-grid>
              <ion-row>
                <ion-col>LED Control</ion-col>
                <ion-col class="flex-right">
                  <ion-toggle
                    v-model="ledStatus"
                    @ionChange="toggleLed"
                  ></ion-toggle>
                </ion-col>
              </ion-row>
            </ion-grid>
          </ion-card-title>
        </ion-card-header>
        <ion-card-content v-if="ledStatus" style="padding-left: 2rem">
          <div v-if="ledStatus">
            <ion-alert
              :is-open="showSetColorError"
              header="Error Setting Color"
              message="Some went wrong setting color, try again!"
              :buttons="['Close']"
              @didDismiss="() => (showSetColorError = false)"
            ></ion-alert>
          </div>
          <ColorPicker @changeColor="(r, g, b) => setColor(r, g, b)" />
        </ion-card-content>
      </ion-card>
      <ion-fab
        slot="fixed"
        vertical="bottom"
        horizontal="end"
        @click="toggleRec"
      >
        <ion-fab-button :color="isListen ? 'danger' : 'dark'">
          <ion-icon v-if="!isListen" :icon="mic"></ion-icon>
          <ion-icon v-else :icon="micOff"></ion-icon>
        </ion-fab-button>
        <ion-toast
          :is-open="showCommand"
          :message="command"
          :duration="5000"
        ></ion-toast>
      </ion-fab>
    </ion-content>
  </ion-page>
</template>

<script setup>
import { ref, onMounted } from "vue";
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
  IonGrid,
  IonRow,
  IonCol,
  IonAlert,
  IonFab,
  IonFabButton,
  IonIcon,
  IonToast,
} from "@ionic/vue";
import LineChart from "./components/LineChart.vue";
import ApiService from "./api/request.js";
import ColorPicker from "./components/ColorPicker.vue";
import { Vosk } from "./api/vosk.js";
import { mic, micOff } from "ionicons/icons";

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

const lastTemperature = ref(null);
const lastLight = ref(null);
const lastNoise = ref(null);
const ledStatus = ref(false);
const showSetColorError = ref(false);
const isListen = ref(false);
const showCommand = ref(false);
const command = ref("");

const requestData = async () => {
  let data = await ApiService.getData("temperature");
  if (data && data.status === "success") {
    lastTemperature.value = data.value;
  } else {
    console.error("Error:", data.message);
  }

  data = await ApiService.getData("light");
  if (data && data.status === "success") {
    lastLight.value = data.value;
  } else {
    console.error("Error:", data.message);
  }

  data = await ApiService.getData("noise");
  if (data && data.status === "success") {
    lastNoise.value = data.value;
  } else {
    console.error("Error:", data.message);
  }
};

const toggleLed = () => {
  ApiService.toggleLed(ledStatus.value ? "on" : "off")
    .then((data) => {
      if (!data || data.status !== "success") {
        ledStatus.value = !ledStatus.value;
        if (data) console.error("Error:", data.message);
      }
    })
    .catch((error) => {
      console.error("Error:", error);
    });
};

const setColor = (r, g, b) => {
  ApiService.setLedColor(r, g, b).then((data) => {
    if (!data || data.status !== "success") {
      showSetColorError.value = true;
      if (data) console.error("Error:", data.message);
    }
  });
};

requestData();

const toggleRec = () => {
  if (isListen.value) {
    stopRec();
  } else {
    startRec();
  }
};

const startRec = () => {
  isListen.value = true;
  Vosk.startRecognition();
};
const stopRec = () => {
  isListen.value = false;
  Vosk.stopRecognition();
};

onMounted(() => {
  VoskPlugin.addListener("transcriptionUpdate", (data) => {
    if (data.transcription) {
      const parsedObject = JSON.parse(data.transcription);
      if (parsedObject) {
        command.value = parsedObject.text;
      }
    }
  });

  VoskPlugin.addListener("permissionGranted", () => {
    log.value = {
      message: "Premesso concesso",
      type: "info",
    };
    Vosk.startRecognition();
  });

  VoskPlugin.addListener("permissionDenied", () => {
    log.value = {
      message: "Premesso negato",
      type: "error",
    };
  });

  setInterval(requestData, 5000);
  ApiService.getLedStatus().then((data) => {
    if (!data || data.status != "success") {
      return;
    }

    ledStatus.value = data.active;
  });
});
</script>

<style scoped>
ion-card {
  margin: 20px;
}
.flex-right {
  justify-content: end;
  display: flex;
  align-items: center;
}
</style>
