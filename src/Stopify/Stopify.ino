/*
    Stopify V1.0 23.05.2025 - Nikolai Radke
    https://www.monstermaker.de

    Sketch for audio analysis gadget. Pauses Spotify if noise detected.
    For standard ESP32 Dev Kit with dual core and Max9814 microphone.

    Wiring:
    Max9814       ESP32
    GND     ->    GND
    Vdd     ->    3V3
    Gain    ->    GND
    Out     ->    34
    AR      ->    Free
*/

#include <arduinoFFT.h>                          // FFT for audio analysis
#include <WiFi.h>                                // Connecting to the internet
#include <WiFiClientSecure.h>                    // Providing HTTPS
#include <Preferences.h>                         // Store token in flash memory
#include <base64.h>                              // Basic Auth-header decoding

#define ADC_PIN       34                         // MAX9814 microphone
#define REWIND        3000                       // Rewind 3 seconds after pause


// --- FFT Configuration ---
#define SAMPLES       256                        // Number of samples per FFT
#define SAMPLING_FREQ 8000                       // Sampling rate in Hz
#define THRESHOLD     5000                       // Amplitude threshold for noise detection
#define LOW_FREQ      3000                       // Low end of detection band
#define HIGH_FREQ     4000                       // High end of detection band


const uint8_t binResolution = SAMPLING_FREQ / SAMPLES;
const uint16_t indexLow = LOW_FREQ / binResolution;
const uint16_t indexHigh = HIGH_FREQ / binResolution;
const uint32_t sampling_period_us = round(1000000 / SAMPLING_FREQ);

double vReal[SAMPLES];
double vImag[SAMPLES];

const char* ssid = "Eintracht";
const char* password = "26122010Merz";

const char clientId[] = "5a35545e79eb41e7ad56295b0ffd14a9";
const char refreshTokenInitial[] = "AQC3xN_vN2XT03s98JHwAiU3-sxcecx3hqrT-MJ3IeXWu2Qiz6a3MVAFmGui84L2nx9RnAjqVuUGgjHbJLMHJ3_E_2TghoIpr4w27aGyvGcESVLkhPgAjhTSk9YwdVOz8Ys";

String refreshTokenStored = "";
String accessToken = "";

uint32_t lastTokenRefresh =0;
const uint32_t tokenRefreshInterval =50UL *60UL *1000UL;

uint8_t detectionCounter =0;
const uint8_t detectionFramesRequired =5;
bool noisePaused = false;
uint32_t progress =0;

WiFiClientSecure client;
ArduinoFFT<double> FFT = ArduinoFFT<double>();

Preferences preferences;
TaskHandle_t audioTaskHandle = NULL;

bool refreshSpotifyAccessToken(bool retry = true);

void setup() {
 Serial.begin(115200);
 delay(1000);

 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) delay(500);
 Serial.println("📶 WLAN verbunden: " + WiFi.localIP().toString());

 client.setInsecure();

 preferences.begin("stopify", false);
 refreshTokenStored = preferences.getString("refreshToken", "");
 if (refreshTokenStored == "") {
 refreshTokenStored = refreshTokenInitial;
 preferences.putString("refreshToken", refreshTokenStored);
 Serial.println("📦 Initial Refresh Token stored");
 }

 if (!refreshSpotifyAccessToken(true)) {
 Serial.println("❌ Fehler beim initialen Token-Abruf, stoppe...");
 while (true);
 }

 xTaskCreatePinnedToCore(audioTask, "AudioTask",4096, NULL,1, &audioTaskHandle,0);

}

void loop() {
  // Token handling
  checkAndRefreshToken();                        // Refresh access token if needed

  // Pause
  if (detectionCounter >= detectionFramesRequired) { // Noise detected?
    if (isSpotifyPlaying()) {                    // Only if Spotify is playing
      pauseSpotify();                            // Pause playback
      noisePaused = true;                        // Set pause flag
      progress = getCurrentPlaybackPosition();   // Save playback position
      Serial.println("🛑 Spotify paused due to noise"); // Debug output
    }
  } else {

  // Resume
    if (noisePaused) {                           // Was it previously paused?
      if (progress > REWIND) progress -= REWIND; // Rewind 3 seconds (default, seit above)
      else progress = 0;                         // Or rewind to beginning
      seekToPosition(progress);                  // Set to new position
      resumeSpotify();                           // Resume playing
      noisePaused = false;                       // Unset pause flag 
      Serial.println("▶️ Spotify resumed");       // Debug output
    }
  }

  delay(100);                                    // Small delay to ease CPU
}

void audioTask(void *pvParameters) {
 while (1) {
 uint32_t startMicros = micros();
 for (uint16_t i =0; i < SAMPLES; i++) {
 vReal[i] = analogRead(ADC_PIN);
 vImag[i] =0;
 while (micros() - startMicros < (i +1) * sampling_period_us)
 taskYIELD();
 }

 FFT.windowing(vReal, SAMPLES, FFT_WIN_TYP_HANN, FFT_FORWARD);
 FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD);
 FFT.complexToMagnitude(vReal, vImag, SAMPLES);

 double maxVal =0;
 for (uint16_t i = indexLow; i <= indexHigh; i++) {
 if (vReal[i] > maxVal) maxVal = vReal[i];
 }

 if (maxVal > THRESHOLD) detectionCounter++;
 else detectionCounter =0;

 delay(10);
 }
}

void checkAndRefreshToken() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastTokenRefresh >= tokenRefreshInterval) {
    Serial.println("🔁 Access Token erneuern...");
    if (refreshSpotifyAccessToken()) {
      Serial.println("✅ Neuer Access Token erhalten");
      lastTokenRefresh = currentMillis;
    } 
    else {
      Serial.println("❌ Token-Erneuerung fehlgeschlagen");
    }
  }
}

bool refreshSpotifyAccessToken(bool retry) {
 if (!client.connect("accounts.spotify.com",443)) {
 Serial.println("❌ Verbindung zu Spotify fehlgeschlagen");
 return false;
 }

 String postData = "grant_type=refresh_token&refresh_token=" + refreshTokenStored +
 "&client_id=" + String(clientId);

 client.print(String("POST /api/token HTTP/1.1\r\n") +
 "Host: accounts.spotify.com\r\n" +
 "Content-Type: application/x-www-form-urlencoded\r\n" +
 "Content-Length: " + postData.length() + "\r\n\r\n" +
 postData);

 String response = client.readString();
 client.stop();

 Serial.println("🔍 Antwort von Spotify: " + response);
 if (response.indexOf("invalid_grant") != -1) {
 Serial.println("⚠️ Refresh Token ungültig oder widerrufen");

 if (retry) {
 refreshTokenStored = refreshTokenInitial;
 preferences.putString("refreshToken", refreshTokenStored);
 Serial.println("🧠 Initialen Token neu gespeichert");
 return refreshSpotifyAccessToken(false);
 }

 Serial.println("🛑 Kein gültiger Refresh Token verfügbar");
 return false;
 }

 int pos = response.indexOf("\"access_token\":\"");
 if (pos >=0) {
 accessToken = response.substring(pos +16, response.indexOf("\"", pos +16));

 int refreshTokenPos = response.indexOf("\"refresh_token\":\"");
 if (refreshTokenPos >=0) {
 refreshTokenStored = response.substring(refreshTokenPos +17, response.indexOf("\"", refreshTokenPos +17));
 preferences.putString("refreshToken", refreshTokenStored);
 }

 return true;
 }

 return false;
}

void pauseSpotify() {                            // Pause playback
  if (!client.connect("api.spotify.com", 443)) return; // Connect to Spotify API
  client.print(String("PUT /v1/me/player/pause HTTP/1.1\r\n") +
               "Host: api.spotify.com\r\n" +     // Set the Host header
               "Authorization: Bearer " + accessToken + "\r\n" + // Add OAuth access token
               "Content-Length: 0\r\n\r\n");     // No body content
  client.stop();                                 // Close the connection
} 

void resumeSpotify() {                           // Resume playback
  if (!client.connect("api.spotify.com", 443)) return; // Connect to Spotify API
  client.print(String("PUT /v1/me/player/play HTTP/1.1\r\n") +
               "Host: api.spotify.com\r\n" +     // Set the Host header
               "Authorization: Bearer " + accessToken + "\r\n" + // Add OAuth access token
               "Content-Length: 0\r\n\r\n");     // No body content
  client.stop();                                 // Close the connection
} 

bool isSpotifyPlaying() {                        // Check if Spotify is currently playing
  if (!client.connect("api.spotify.com", 443)) return false; // Connect to Spotify API
  client.print(String("GET /v1/me/player HTTP/1.1\r\n") +
               "Host: api.spotify.com\r\n" +     // Set the Host header
               "Authorization: Bearer " + accessToken + "\r\n\r\n"); // Add OAuth access token
  String response;                               // Store response
  while (client.connected()) {
    String line = client.readStringUntil('\n');  // Read headers
    if (line == "\r") break;                     // End of headers
  }
  while (client.available()) {
    response += client.readString();             // Append response content
  }
  client.stop();                                 // Close the connection
  if (response.indexOf("\"is_playing\"") != -1) {
    int pos = response.indexOf("\"is_playing\""); // Find "is_playing" key
    String isPlayingSnippet = response.substring(pos, pos + 25); // Extract value
    isPlayingSnippet.replace(" ", "");           // Remove spaces
    if (isPlayingSnippet.indexOf(":true") > 0) return true; // Spotify is playing
    if (isPlayingSnippet.indexOf(":false") > 0) return false; // Spotify is paused
  }
  return false;                                  // Default to false if unknown state
} 

uint32_t getCurrentPlaybackPosition() {          // Get the current playback position in ms
  if (!client.connect("api.spotify.com", 443)) return 0; // Connect to Spotify API
  client.print(String("GET /v1/me/player HTTP/1.1\r\n") +
               "Host: api.spotify.com\r\n" +     // Set the Host header
               "Authorization: Bearer " + accessToken + "\r\n\r\n"); // Add OAuth access token
  while (client.connected() && client.readStringUntil('\n') != "\r"); // Skip headers
  String response = client.readString();         // Read response body
  client.stop(); // Close the connection
  int pos = response.indexOf("\"progress_ms\""); // Find progress_ms key
  if (pos >= 0) {
    pos = response.indexOf(":", pos);            // Locate value start
    int end = response.indexOf(",", pos);        // Locate value end
    String progressString = response.substring(pos + 1, end); // Extract value string
    progressString.trim();                       // Remove whitespace
    return progressString.toInt();               // Convert to integer (ms)
  }
  return 0;                                      // Default to 0 if not found
}

void seekToPosition(uint32_t time_ms) {          // Seek to a specific position in the paused track
  if (!client.connect("api.spotify.com", 443)) return; // Connect to Spotify API
  client.print(String("PUT /v1/me/player/seek?position_ms=") + time_ms + " HTTP/1.1\r\n" +
               "Host: api.spotify.com\r\n" +     // Set the Host header
               "Authorization: Bearer " + accessToken + "\r\n" + // Add OAuth access token
               "Content-Length: 0\r\n\r\n");     // No body content
  client.stop();                                 // Close the connection
} 