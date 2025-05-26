# Firmware for Stopify  
  
This guide is written for the Arduino IDE, but if you're using another development environment, you should be able to adapt it easily.

### Preparing the IDE  
To get started, you’ll need to add ESP32 support to your Arduino IDE:

1. Open ``` File > Preferences ```.  
2. In the field ``` Additional Boards Manager URLs ``` enter:
   ``` https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json ```.  
3. Click ``` OK ```.  
4. Go to ``` Tools > Board > Boards Manager ```.  
5. Search for ``` esp32 by Espressif Systems ```.  and click ``` Install ```.
6. Then open ``` Tools > Manage Libraries ```.
7. Search for **base64** and click ``` Install ```.
8. Search for **arduinoFFT** and click ``` Install ```.

### Preparing the sketch
You’ll need your Client ID and the Refresh Token. There’s a tool available for Linux and Windows to help you get the token:  [token_tool](https://github.com/NikolaiRadke/Stopify/tree/main/token_tool). Now open the sketch file and make the following edits:  

1. Line 41: enter your Wi-Fi **SSID** in ``` ssid = ""; ```.`
2. Line 42: enter your Wi-Fi **password** in ``` password = ""; ```.
3. Line 45: paste your **Client ID** into `` clientId[] = ""; ```. 
4. Line 46: paste the **Refresh Token** into ``` refreshToken[] = ""; ```.

### Configuring the sketch
You can customize Stopify’s behavior:

* Line 28: ``` SAMPLING_FREQ ``` – change this if you want to detect higher frequencies. It must be at least **twice** the value of ``` HIGH_FREQ ``` (Nyquist theorem!).
* Line 29: ``` THRESHOLD ``` – adjust this to change the sensitivity.
* Line 30 & 31: ``` LOW_FREQ ``` and ``` HIGH_FREQ ``` – define the frequency band to monitor (e.g., 3000–4000 Hz for power tools).

### Uploading the sketch
1. Go to ``` Tools > Board > esp32 ``` **ESP32 Dev Module** or whatever kind of board you are using.
2. Plug in your Board and select the correct port under ``` Tools > Port ```.
3. Click on the **Upload** button in the IDE.

### Check status
Once uploaded, open the Serial Monitor (button in the top right of the IDE). If everything went well, you’ll see a message confirming 
Wi-Fi connection. The ESP32 will print its assigned **IP address**. Now play something on Spotify, make some noise with a drill, vacuum 
cleaner, or use a tone generator, and watch the **Serial Monitor** for debug messages.
      

