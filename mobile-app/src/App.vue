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
          <ColorPicker @changeColor="(r, g, b) => setColor(r, g, b)" />
        </ion-card-content>
      </ion-card>
      <ion-alert
        :is-open="showVocalCommandResponse"
        :header="vocalCommandResponse.header"
        :message="vocalCommandResponse.message"
        :buttons="['Close']"
        :cssClass="vocalCommandResponse.cssClass"
        @didDismiss="() => (showVocalCommandResponse = false)"
      ></ion-alert>
      <ion-alert
        :is-open="showErrorAlert"
        :header="alertError.header"
        :message="alertError.message"
        :buttons="['Close']"
        cssClass="error-alert"
        @didDismiss="() => (showErrorAlert = false)"
      ></ion-alert>
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
import { ref, onMounted, onUnmounted, reactive } from "vue";
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
import { mic, micOff, settingsSharp } from "ionicons/icons";
import { registerPlugin } from "@capacitor/core";
import {
  requestPermission,
  startListening,
  stopListening,
} from "./api/speech-recognition.js";

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
const showErrorAlert = ref(false);
const alertError = reactive({
  header: "",
  message: "",
});
const isListening = ref(false);
const showCommand = ref(false);
const command = ref("");
const showVocalCommandResponse = ref(false);
const vocalCommandResponse = reactive({
  header: "",
  message: "",
  cssClass: "",
});
const defaultErrorMsg = "Something went wrong :(";

const requestData = async () => {
  let data = await ApiService.getData("temperature");
  if (data && !data.error && data.type == "temperature") {
    lastTemperature.value = data.value;
    lastTemperatureTime.value = new Date(data.time).toLocaleTimeString(
      "it-IT",
      {
        timeZone: "Europe/Rome",
      }
    );
  } else {
    console.error("Error receiving temperature");
  }

  data = await ApiService.getData("light");
  if (data && !data.error && data.type == "light") {
    lastLight.value = data.value;
    lastLightTime.value = new Date(data.time).toLocaleTimeString("it-IT", {
      timeZone: "Europe/Rome",
    });
  } else {
    console.error("Error receiving light");
  }

  data = await ApiService.getData("noise");
  if (data && !data.error && data.type == "noise") {
    lastNoise.value = data.value;
    lastNoiseTime.value = new Date(data.time).toLocaleTimeString("it-IT", {
      timeZone: "Europe/Rome",
    });
  } else {
    console.error("Error receiving noise");
  }

  setTimeout(requestData, 5000);
};

const toggleLed = () => {
  ApiService.toggleLed(ledStatus.value ? "on" : "off").then((data) => {
    if (!data || data.error) {
      ledStatus.value = !ledStatus.value;
      alertError.header = "Error switching led";
      alertError.message = `${data?.error || defaultErrorMsg}`;
      console.error("Error:", alertError.message);
      showErrorAlert.value = true;
    }
  });
};

const setColor = (r, g, b) => {
  ApiService.setLedColor(r, g, b).then((data) => {
    if (!data || data.error) {
      alertError.header = "Error setting color:";
      alertError.message = `${data?.error || defaultErrorMsg}`;
      console.error("Error:", alertError.message);
      showErrorAlert.value = true;
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

const stopRec = () => {
  isListening.value = false;
  stopListening();
};

const startRec = async () => {
  isListening.value = true;
  const result = await startListening();
  isListening.value = false;
  command.value = "Comando inviato: " + result;
  showCommand.value = true;
  ApiService.sendCommand(result).then((data) => {
    if (data && !data.error) {
      if (data.type === "temperature") {
        vocalCommandResponse.header =
          "The temperature is " + data.value + " °C";
        vocalCommandResponse.message =
          "Detected at " +
          new Date(data.time).toLocaleTimeString("it-IT", {
            timeZone: "Europe/Rome",
          });
        vocalCommandResponse.cssClass = "temperature-alert";
        showVocalCommandResponse.value = true;
      } else if (data.type === "light") {
        vocalCommandResponse.header =
          "The ambient illumination is " + data.value + " lux";
        vocalCommandResponse.message =
          "Detected at " +
          new Date(data.time).toLocaleTimeString("it-IT", {
            timeZone: "Europe/Rome",
          });
        vocalCommandResponse.cssClass = "light-alert";
        showVocalCommandResponse.value = true;
      } else if (data.type === "noise") {
        vocalCommandResponse.header =
          "The noise level is " + data.value + " dB";
        vocalCommandResponse.message =
          "Detected at " +
          new Date(data.time).toLocaleTimeString("it-IT", {
            timeZone: "Europe/Rome",
          });
        vocalCommandResponse.cssClass = "noise-alert";
        showVocalCommandResponse.value = true;
      } else if (data.executed === "LED_ON") {
        ledStatus.value = true;
      } else if (data.executed === "LED_OFF") {
        ledStatus.value = false;
      }
    } else {
      alertError.header = "Error handling vocal command";
      alertError.message = `${data?.error || defaultErrorMsg}`;
      console.error("Error handling vocal request:", alertError.message);
      showErrorAlert.value = true;
    }
  });
};

onMounted(() => {
  requestPermission();

  ApiService.getLedStatus().then((data) => {
    if (!data || data.error) {
      alertError.header = "Error getting led status";
      alertError.message = `${data?.error || defaultErrorMsg}`;
      showErrorAlert.value = true;
      console.error("Error:", alertError.message);
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
<style>
.temperature-alert .alert-button {
  color: rgb(25, 116, 116) !important;
}
.noise-alert .alert-button {
  color: rgb(118, 29, 153) !important;
}
.light-alert .alert-button {
  color: rgb(175, 105, 0) !important;
}

.error-alert .alert-button {
  color: rgb(235, 0, 0) !important;
}
</style>
