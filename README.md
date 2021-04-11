# PS3ToBle

The ESP32 will allow you, without using any drivers, use your PS3 Controller on HID hosts (including Windows, Mac, Android and iOS systems). 

To make this work I had to make changes in the below libraries:
- https://github.com/jvpernis/esp32-ps3
  - I had to change the init code to get BLE and BT to cohexist.
- https://github.com/lemmingDev/ESP32-BLE-Gamepad
  - I added a descriptor to get the start and select buttons to work
