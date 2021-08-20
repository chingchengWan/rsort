<template>
  <n-config-provider :theme-overrides="themeOverrides">
    <Menu class="menu" />
  </n-config-provider>
</template>

<script>
import { NConfigProvider } from "naive-ui";
import Menu from "@/components/Menu/index.vue";

const themeOverrides = {
  common: {
    primaryColor: "#FF0000",
  },
  Button: {
    textColor: "#FF0000",
  },
  Select: {
    peers: {
      InternalSelection: {
        textColor: "#FF0000",
      },
    },
  },
};

export default {
  data() {
    return {
      themeOverrides,
    };
  },
  components: {
    Menu,
    NConfigProvider,
  },
};
</script>
