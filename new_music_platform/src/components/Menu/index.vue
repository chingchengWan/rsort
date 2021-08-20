<template>
  <div class="top-nav">
    <a href="#" class="item">
      <n-icon>
        <home />
      </n-icon>
    </a>
    <a href="#" class="item">發現</a>
    <div class="item">
      <n-popover trigger="click">
        <template #trigger>
          <n-icon>
            <search />
          </n-icon>
        </template>
        <div>
          搜尋
          <n-input type="text" placeholder="可以清除" clearable/>
        </div>
      </n-popover>
    </div>
    <div class="right-link">
      <a href="#" class="item">登入 / 註冊</a>
    </div>
  </div>
</template>

<script>
import { defineComponent } from "vue";
import { Home, Search } from "@vicons/carbon";

export default defineComponent({
  components: {
    Home,
    Search,
  },
});
</script>

<style lang="scss" scoped>
$primary-color: #f9595f;

.top-nav {
  display: flex;
  overflow: hidden;
  background-color: rgba(0, 0, 0, 0.3);
  padding: 0 7vw;

  .right-link {
    margin-left: auto;
    order: 2;
  }
}

.item {
  display: flex;
  align-items: center;
  color: #fff;
  padding: 14px 16px;
  text-decoration: none;
  &:hover {
    color: $primary-color;
  }
  &:active {
    color: $primary-color;
  }
}

@media screen and (max-width: 600px) {
  .top-nav a:not(:first-child) {
    display: none;
  }
  .top-nav a.icon {
    float: right;
    display: block;
  }
}

@media screen and (max-width: 600px) {
  .top-nav.responsive {
    position: relative;
  }
  .top-nav.responsive .icon {
    position: absolute;
    right: 0;
    top: 0;
  }
  .top-nav.responsive a {
    float: none;
    display: block;
    text-align: left;
  }
}

i {
  font-size: 1rem;
}

</style>
