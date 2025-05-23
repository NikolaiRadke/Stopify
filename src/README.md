# Firmware für Stopify  
  
This description is intended for the Arduino IDE. However, those using a different environment should have no problems. 

### Preparing the IDE  
First of all, your IDE need ESP32 support:

1. Open ``` File > Preferences ```.  
2. Find the field ``` Additional Boards Manager URLs ```.  
3. Enter ``` https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json ```.  
4. Press ``` OK ```.  
5. Open ``` Tools > Board > Boards Manager ```.  
6. Select ``` esp32 by Espressif Systems ```.  
7. Press ``` Install ```.

### Preparing the sketch
You need the client ID, the client secret and the refresh token. There is a tool for Linux and Windows that helps getting this token. Look here: [token_tool](https://github.com/NikolaiRadke/Stopify/tree/main/token_tool])  
