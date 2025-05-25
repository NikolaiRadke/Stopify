# How to get Client ID, Client Secret and Refresh Token

To work properly, Stopify needs access to your Spotify account. This is done by creating a **Spotify app** through the [https://developer.spotify.com/](https://developer.spotify.com/) portal. Don't worry, ou only need to do this **once*.

1. Open the URL above and log in (top right corner).
2. Click on your profile name (again, top righ) and select ``` Dashboard ```.
3. Click on ``` Create app ```.
4. Give your app a name, like *Stopify* or anything you like.
5. Enter a short description. It's mandatory, even though it's unused. Example: *Noise controller app for ESP32*.
6. In the ``` Redirect URL ``` field, enter **http://127.0.0.1:8888/callback)** and click ``` Add ```.
7. Agree to Spotify's Developer Terms of Service.  
8. Click ``` Save ```. 

Your app is now set up. Now get the Token.
1. Your **Client ID** is now visible. Click **View Client Secret** to show the secret. **Keep this page open**.
2 Start the token tool:
  For **Windows** run **spotify_token_tool.exe** from``` token_tool/Windows ```.  
  For **Linux** run **spotify_token_tool** from ``` token_tool/Linux ```.
11. Your Browser will open. Enter the **Client ID** and **Client Secret** from the Spotify dashboard.
12. If you are not logged to Spotify, log in now.
13. Click ``` Agree ```.  to authorize the app. Your **refresh token** is now displayed.
14. Copy the **Client ID**, **Client Secret**, and **Refresh Token** into your Stopify sketch. That’s it — your ESP32 is now linked
    to Spotify.
      
*Fun fact:*
The token tool was written in Python — using ChatGPT — because the creator didn’t know any Python.   
It works anyway.  
