# How to get client id, client secret and the refresh token

Stopify must be connected to your Spotify account to run. This must be done by creating a Spotify app on the *Spotify for Developers* page under [https://developer.spotify.com/](https://developer.spotify.com/).

1. Open the URL above and log in (upper right corner)
2. Click on your name (upper right corner again) and select ``` Dashboard ```.
3. Click on ``` Create app ```.
4. Give your app a name, like *Stopify* or whatever you like
5. Write a short description. Unused, but mandatory. Maybe *Noise controller app for ESP32*.
6. In field ``` Redirect URL ``` enter **http://localhost:8080/callback**
7. Click on ``` Save ```. Your app is configured.
8. Now ***Client ID** is displayed. Click on ``` View client secret  ``` to display the secret too.
