import { createApp } from "vue";
import App from "./App.vue";
import router from "./router";
import store from "./store";
import pluginNaive from "./plugins/naive-ui-vue"

const app = createApp(App);

pluginNaive(app);

app.use(store).use(router).mount("#app");
