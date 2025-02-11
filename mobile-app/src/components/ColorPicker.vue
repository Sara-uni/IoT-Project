<template>
  <ion-grid>
    <ion-row>
      <ion-col>
        <ion-range
          aria-label="Red"
          :pin="true"
          :min="0"
          :max="255"
          :value="red"
          class="red"
          @ionChange="redChanged"
        ></ion-range>
        <ion-range
          aria-label="Green"
          :pin="true"
          :min="0"
          :max="255"
          :value="green"
          class="green"
          @ionChange="greenChanged"
        ></ion-range>
        <ion-range
          aria-label="Blue"
          :pin="true"
          :min="0"
          :max="255"
          :value="blue"
          class="blue"
          @ionChange="blueChanged"
        ></ion-range>
      </ion-col>
      <ion-col class="columnDiv" size="auto">
        <div class="colorDiv" :style="{ backgroundColor: color }"></div>
        <ion-button class="button" fill="outline" @click="emitColor">
          SET COLOR
        </ion-button>
      </ion-col>
    </ion-row>
  </ion-grid>
</template>

<script setup>
import { IonRange, IonGrid, IonRow, IonCol, IonButton } from "@ionic/vue";
import { ref, computed, onMounted } from "vue";

const red = ref(0);
const green = ref(0);
const blue = ref(0);
const emits = defineEmits(["changeColor"]);

const color = computed(
  () => "rgb(" + red.value + ", " + green.value + ", " + blue.value + ")"
);

const redChanged = (event) => {
  red.value = event.detail.value;
};
const greenChanged = (event) => {
  green.value = event.detail.value;
};
const blueChanged = (event) => {
  blue.value = event.detail.value;
};

const emitColor = () => {
  emits("changeColor", red.value, green.value, blue.value);
};

onMounted(() => {
  red.value = 255;
  green.value = 255;
  blue.value = 255;
});
</script>
<style scoped>
.red {
  --bar-background: #666666;
  --bar-background-active: #a40000;
  --knob-background: #ff0000;
  --pin-background: #a40000;
  --pin-color: #fff;
}
.green {
  --bar-background: #666666;
  --bar-background-active: #00a400;
  --knob-background: #00ff00;
  --pin-background: #00a400;
  --pin-color: #fff;
}
.blue {
  --bar-background: #666666;
  --bar-background-active: #0000a4;
  --knob-background: #0000ff;
  --pin-background: #0000a4;
  --pin-color: #fff;
}
ion-range {
  padding-top: 0;
}
.colorDiv {
  height: 3rem;
  width: 6rem;
  border-radius: 10px;
}
.columnDiv {
  display: flex;
  justify-content: space-evenly;
  align-items: center;
  flex-direction: column;
}
.button {
  --background-focused: #ffffff;
  --background-hover: #ffffff;
  --border-color: #ffffff;
  --color: #ffffff;
}
</style>
