<template>
  <ion-page>
    <ion-header>
      <ion-toolbar>
        <ion-buttons slot="primary">
          <ion-button id="ip-settings">
            <ion-icon
              slot="icon-only"
              :md="settingsSharp"
              color="dark"
            ></ion-icon>
          </ion-button>
        </ion-buttons>
        <ion-title>IoT Dashboard</ion-title>
      </ion-toolbar>
    </ion-header>
    <ion-content>
      <ion-alert
        trigger="ip-settings"
        header="Insert the IP:"
        :buttons="[
          'Cancel',
          { text: 'Confirm', handler: (data) => setIp(data.ip) },
        ]"
        :inputs="[
          {
            name: 'ip',
            placeholder: '192.168.0.1',
            value: getIp(),
          },
        ]"
      ></ion-alert>
      <LineChart
        title="Temperature"
        :data="temperature"
        scale-y="Temperature (°C)"
        unit="°C"
        :new-data="lastTemperature"
        :new-time="lastTemperatureTime"
      />
      <LineChart
        title="Ambient Light"
        :data="ambientLight"
        scale-y="Light (lux)"
        unit="lux"
        :new-data="lastLight"
        :new-time="lastLightTime"
      />
      <LineChart
        title="Ambient Noise"
        :data="ambientNoise"
        scale-y="Noise (dB)"
        unit="dB"
        :new-data="lastNoise"
        :new-time="lastNoiseTime"
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
        <ion-fab-button :color="isListening ? 'danger' : 'dark'">
          <ion-icon v-if="!isListening" :icon="mic"></ion-icon>
          <ion-icon v-else :icon="micOff"></ion-icon>
        </ion-fab-button>
        <ion-toast
          :is-open="showCommand"
          :message="command"
          :duration="5000"
          position="top"
          animated="true"
          color="dark"
          @didDismiss="() => (showCommand = false)"
        ></ion-toast>
      </ion-fab>
    </ion-content>
  </ion-page>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from "vue";
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
import { mic, micOff, settingsSharp } from "ionicons/icons";
import { registerPlugin } from "@capacitor/core";

const VoskPlugin = registerPlugin("VoskPlugin");

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
const lastTemperatureTime = ref(null);
const lastLightTime = ref(null);
const lastNoiseTime = ref(null);
const ledStatus = ref(false);
const showSetColorError = ref(false);
const isListening = ref(false);
const showCommand = ref(false);
const command = ref("");

const requestData = async () => {
  let data = await ApiService.getData("temperature");
  if (data && !data.error && data.type == "temperature") {
    lastTemperature.value = data.value;
    lastTemperatureTime.value = new Date(data.time);
  } else {
    console.error("Error:", data.message);
  }

  data = await ApiService.getData("light");
  if (data && !data.error && data.type == "light") {
    lastLight.value = data.value;
    lastLightTime.value = new Date(data.time);
  } else {
    console.error("Error:", data.message);
  }

  data = await ApiService.getData("noise");
  if (data && !data.error && data.type == "noise") {
    lastNoise.value = data.value;
    lastNoiseTime.value = new Date(data.time);
  } else {
    console.error("Error:", data.message);
  }
};

const toggleLed = () => {
  ApiService.toggleLed(ledStatus.value ? "on" : "off")
    .then((data) => {
      if (!data || data.error) {
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
    if (!data || data.error) {
      showSetColorError.value = true;
      if (data) console.error("Error:", data.message);
    }
  });
};

const getIp = () => {
  return localStorage.getItem("ip");
};

const setIp = (ip) => {
  ip = ip.trim();
  const ipRegex =
    /^(25[0-5]|2[0-4][0-9]|1?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|1?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|1?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|1?[0-9][0-9]?)$/;
  if (!ipRegex.test(ip)) {
    alert("⚠️ Insert a valid IPv4 address!");
    return false;
  }
  localStorage.setItem("ip", ip);
  ApiService.setServerIP();
  return true;
};

requestData();

const toggleRec = () => {
  if (isListening.value) {
    stopRec();
  } else {
    startRec();
  }
};

const startRec = () => {
  isListening.value = true;
  Vosk.startRecognition();
};
const stopRec = () => {
  isListening.value = false;
  Vosk.stopRecognition();
};

onMounted(() => {
  VoskPlugin.addListener("transcriptionUpdate", (data) => {
    if (data.transcription) {
      const parsedObject = JSON.parse(data.transcription);
      console.log(parsedObject);
      if (parsedObject) {
        command.value = "Comando inviato: " + parsedObject.text;
        showCommand.value = true;
        ApiService.sendCommand(parsedObject.text);
      }
    }
    isListening.value = false;
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

onUnmounted(() => {
  VoskPlugin.removeAllListeners();
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
