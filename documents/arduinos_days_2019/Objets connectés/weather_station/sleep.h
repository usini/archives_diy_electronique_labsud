 void sleepMode(){
 esp_sleep_enable_timer_wakeup(SLEEPFOR);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_32, 1);
  esp_deep_sleep_start();
  }