# PS3ToBle

By just using a ESP32, this project will allow you to use your PS3 Controller on Windows, Mac, Android and iOS systems (or any HID host!).

No drivers needed, it will just work!

This project, although functional, required some changes in 2 of the libs it uses:

- https://github.com/jvpernis/esp32-ps3
  - I had to change the init code to get BLE and BT to cohexist.
- https://github.com/lemmingDev/ESP32-BLE-Gamepad
  - I added a descriptor to get the start and select buttons to work

I am hoping the owners of these two libs can evaluate my changes and incorporate or suggest changes in my code as they see fit. 

