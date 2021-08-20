import {
  create,
  NMenu,
  NIcon,
  NPopover,
  NInput,
} from 'naive-ui'

export default (app) => {
  const naive = create({
    components: [
      NMenu,
      NIcon, 
      NPopover,
      NInput,
    ]
  });

  app.use(naive);
};
