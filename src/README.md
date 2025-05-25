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
8. Open ``` Tools > Manage Libraries ```.
9. In the library manager search for **base64** and press ``` Install ```.
10. Again in the library manager search for **arduinoFFT** and press ``` Install ```.

### Preparing the sketch
You need the client ID, the client secret and the refresh token. There is a tool for Linux and Windows that helps getting this token. Look here: [token_tool](https://github.com/NikolaiRadke/Stopify/tree/main/token_tool)  

1. Insert the **client ID** from the dashboard into line 44 in ``` clientId[] = ""; ```.
2. Insert the**client secret** from the dashboard into line 45 in ``` clientSecret[] = ""; ```.
3. Insert the (long) **refresh token** from the token tool in line 46 in ``` refreshToken[] = ""; ```.
4. Enter **your Wifi SSID** in Line 40 in ``` ssid = ""; ```.
5. Enter **your Wifi password** in Line 42 in ``` password = ""; ```.

### Configuring the sketch
You can edit the detection band in line 29 in **LOW_FREQ** and line 30 in **HIGH_FREQ**, If you need to analyse a higher frequence, you must change **SAMPLING_FREQ**. It must be at least twice as high as **HIGH_FREQ** and . You can adjust the sensivity with **THRESHOLD** in line 27.  

### Uploading the sketch
1. Select ``` Board > esp32 > ESP32 Dev Module ``` or whatever kind of board you are using.
2. Plug in your Board and select ``` Port ``` with your used USB port.
3. Click on the upload button in the IDE.

You can open the serial monitor. If everthing is fine, the ESP32 will connect to Wifi and display its current IP. Now you can start Spotify, play a track, make some noise, maybe with a cordless screwdriver, and see what happens. the serial monitor will print debug informations.


      

