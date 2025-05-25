# How to get client id, client secret and the refresh token

Stopify must be connected to your Spotify account to run. This must be done by creating a Spotify app on the *Spotify for Developers* page under [https://developer.spotify.com/](https://developer.spotify.com/).

1. Open the URL above and log in (upper right corner).
2. Click on your name (upper right corner again) and select ``` Dashboard ```.
3. Click on ``` Create app ```.
4. Give your app a name, like *Stopify* or whatever you like.
5. Write a short description. Unused, but mandatory. Maybe *Noise controller app for ESP32*.
6. In field ``` Redirect URL ``` enter **http://127.0.0.1:8888/callback)** and press ``` Add ```.
7. Agree with Spotify's Developer Terms of Service.  
8. Click on ``` Save ```. Your app is configured.
9. Now **client ID** is displayed. Click on ``` View client secret  ``` to display the **client secret** too. Leave the window open.  
10. For **Windows** start **spotify_token_tool.exe** in folder ``` token_tool/Windows ```.  
  For **Linux** start **spotify_token_tool** in folder ``` token_tool/Linux ```.
11. Your Browser will open. Enter the Codes from the dasboard in ``` Client-ID ``` and ``` Client-Secret```.
12. If you are not logged in anymore, log in now.
13. Click on ``` Agree ```. Your **refresh token** is displayed.
14. Insert **client ID**, **client secret** and the **refresh token** into your sketch.

Now you can upload your code and Stopify is connected to your Spotify account.
  
The tool was written in Python using ChatGPT because the creator doesn't know Python.

