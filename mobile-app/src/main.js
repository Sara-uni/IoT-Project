import { createApp } from "vue";
import { IonicVue } from "@ionic/vue";
import App from "./App.vue";

// Import Ionic CSS
import "@ionic/vue/css/core.css";
import "@ionic/vue/css/normalize.css";
import "@ionic/vue/css/structure.css";
import "@ionic/vue/css/typography.css";
import "@ionic/vue/css/palettes/dark.always.css";

const app = createApp(App);
app.use(IonicVue);
app.mount("#app");
