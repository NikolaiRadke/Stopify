/*
    Stopify V1.0 23.05.2025 - Nikolai Radke
    https://www.monstermaker.de

    Sketch for audio analysis gadget. Pauses Spotify if noise detected.
    For standard ESP32 Dev Kit with dual core and Max9814 microfone.

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
#include <base64.h>                              // Basic Auth-header decoding

#define ADC_PIN 34                               // Max9814 microfone
#define REWIND 3000                              // Rewind 3 seconds after pause

// --- FFT Configuration ---
#define SAMPLES 256                              // Number of samples per FFT
#define SAMPLING_FREQUENCY 8000                  // Sampling rate in Hz
#define THRESHOLD 5000.0                         // Amplitude threshold for noise detection
#define LOW_FREQ  3000.0                         // Low end of detection band
#define HIGH_FREQ 4000.0                         // High end of detection band

const uint8_t  binResolution = SAMPLING_FREQUENCY / SAMPLES; // Hz per FFT bin
const uint16_t indexLow  = LOW_FREQ / binResolution; // Low bin index
const uint16_t indexHigh = HIGH_FREQ / binResolution; // High bin index
const uint32_t sampling_period_us = round(1000000.0 / SAMPLING_FREQUENCY); // Time between samples in microseconds
double         vReal[SAMPLES];                   // Array for real parts of FFT input
double         vImag[SAMPLES];                   // Array for imaginary parts (set to 0)

// --- WiFi credentials ---
const char*    ssid     = "Eintracht";           // WiFi SSID
const char*    password = "26122010Merz";        // WiFi password

// --- Spotify OAuth credentials ---
char clientId[]     = "";                        // Your Spotify client ID
char clientSecret[] = "";                        // Your Spotify client secret
char refreshToken[] = "";                        // Your refresh token from the stopify token tool 

String         accessToken;                      // Active OAuth token
uint32_t       lastTokenRefresh = 0;             // Timestamp of last token refresh
const uint32_t tokenRefreshInterval = 50UL * 60UL * 1000UL; // Refresh every 50 minutes
uint8_t        detectionCounter = 0;             // Count of consecutive detections
const uint8_t  detectionFramesRequired = 5;      // Threshold for confirming detection
bool           noisePaused = false;              // Is playback currently paused due to noise?
uint32_t       progress = 0;                     // Playback position for resume

WiFiClientSecure client;                         // HTTPS client
ArduinoFFT<double> FFT = ArduinoFFT<double>();   // FFT instance

TaskHandle_t audioTaskHandle = NULL;             // Handle for the audio analysis task

void setup() {
  Serial.begin(115200);                          // Start serial monitor
  delay(1000);                                   // Give time for USB connection
  WiFi.begin(ssid, password);                    // Connect to WiFi
  while (WiFi.status() != WL_CONNECTED) delay(500); // Wait until connected
  Serial.println("📶 WiFi connected: " + WiFi.localIP().toString());
  client.setInsecure();                          // Skip certificate validation

  if (!refreshSpotifyAccessToken()) {            // Try to get initial access token
    Serial.println("❌ Failed to get Access Token. Stopping.");
    while(true);                                 // Halt execution if no token present
  }

  xTaskCreatePinnedToCore(audioTask, "AudioTask", 4096, NULL, 1, &audioTaskHandle, 0); // Run audio task on core 0
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

void audioTask(void *pvParameters) {             // Analyse audio
  while(1) {                                     // Forever. It has an own Core 0
    uint32_t startMicros = micros();             // Start sampling timestamp
    for (uint16_t i = 0; i < SAMPLES; ++i) {     // Analyse every sample
      vReal[i] = analogRead(ADC_PIN);            // Read analog sample
      vImag[i] = 0;                              // Imaginary part is 0
      while (micros() - startMicros < (i + 1) * sampling_period_us)
        taskYIELD();                             // Wait until next sample
    }

    FFT.windowing(vReal, SAMPLES, FFT_WIN_TYP_HANN, FFT_FORWARD); // Apply window
    FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD); // Perform FFT
    FFT.complexToMagnitude(vReal, vImag, SAMPLES); // Get magnitudes

    double maxVal = 0.0;                         // Reset amplitude
    for (uint16_t i = indexLow; i <= indexHigh; i++) // Scan relevant band
      if (vReal[i] > maxVal) maxVal = vReal[i];

    if (maxVal > THRESHOLD) detectionCounter++;  // Noise detected
    else detectionCounter = 0;                   // Reset counter

    delay(10);                                   // Prevent CPU hog
  }
}

void checkAndRefreshToken() {                    // // Check periodically and refresh Spotify OAuth access token if needed
  unsigned long currentMillis = millis();        // Get current time in milliseconds
  if (currentMillis - lastTokenRefresh >= tokenRefreshInterval) { // Check if token refresh interval passed
    Serial.println("🔁 Access Token erneuern..."); // Log token refresh start
    accessToken = refreshSpotifyAccessToken();   // Attempt to refresh the Spotify access token
    if (accessToken.length() > 0) {              // Check if a new token was successfully obtained
      Serial.println("✅ Received new access token"); // Debug output
      lastTokenRefresh = currentMillis;          // Update last refresh timestamp
    } else {
      Serial.println("❌ Could not refresh token."); // Debug output
    }
  }
} 

bool refreshSpotifyAccessToken() {               // Perform OAuth refresh token request to get a new access token
  if (!client.connect("accounts.spotify.com", 443)) { // Connect to Spotify account via HTTPS
    Serial.println("❌ Failed to connect to Spotify"); // Debug output
    return false;                                // Fail if connection unsuccessful
  }
  String credentials = String(clientId) + ":" + String(clientSecret); // Prepare client credentials string
  String encodedCredentials = base64::encode(credentials); // Base64 encode clientId:clientSecret
  String postData = "grant_type=refresh_token&refresh_token=" + String(refreshToken); // Prepare POST data for token refresh
  client.print(String("POST /api/token HTTP/1.1\r\n") + // Send POST request to token endpoint
               "Host: accounts.spotify.com\r\n" + 
               "Authorization: Basic " + encodedCredentials + "\r\n" + // Add Basic Auth header
               "Content-Type: application/x-www-form-urlencoded\r\n" + // Content type header
               "Content-Length: " + postData.length() + "\r\n\r\n" + // Content length header
               postData);                        // POST body with grant_type and refresh_token
  String response = client.readString();         // Read the response from server
  client.stop();                                 // Close connection
  int pos = response.indexOf("\"access_token\":\""); // Look for access_token in JSON response
  if (pos >= 0) {
    accessToken = response.substring(pos + 16, response.indexOf("\"", pos + 16)); // Extract access token string
    return true;                                 // Return true is successfull
  }
  return false;                                  // Return false if token not found
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
