# How to get Client ID and Refresh Token

To work properly, Stopify needs access to your Spotify account. This is done by creating a **Spotify app** through the [https://developer.spotify.com/](https://developer.spotify.com/) portal. Don't worry, ou only need to do this **once*.

If you feel unsafe to run an executable from an unkown author like me, see [security.txt](https://github.com/NikolaiRadke/Stopify/blob/main/token_tool/security.txt) for security informations.  
  
1. Open the URL above and log in (top right corner).
2. Click on your profile name (again, top righ) and select ``` Dashboard ```.
3. Click on ``` Create app ```.
4. Give your app a name, like *Stopify* or anything you like.
5. Enter a short description. It's mandatory, even though it's unused. Example: *Noise controller app for ESP32*.
6. In the ``` Redirect URL ``` field, enter **`http://127.0.0.1:8888/callback`** and click ``` Add ```.
7. Agree to Spotify's Developer Terms of Service.  
8. Click ``` Save ```. Your app is now set up. Now get the Token.  
9. Your **Client ID** is now visible. 
10. Start the token tool:  
  For **Windows** run  ``` spotify_token_tool.exe  ```  from``` token_tool/Windows ```.    
  For **Linux** run  ``` spotify_token_tool ```  from ``` token_tool/Linux ```.  
12. Your Browser will open. Enter the **Client ID** from the Spotify dashboard.  
13. If you are not logged to Spotify, log in now.  
14. Click ``` Agree ```.  to authorize the app. Your **refresh token** is now displayed.  
15. Copy the **Client ID** and **Refresh Token** into your Stopify sketch. That’s it — your ESP32 is now linked to Spotify.  

The file was compiled with PyInstaller. the source code is in this folder. To run it in a Python enviroment, you need to install **flask** and **requests**. 
      
*Fun fact:*
The token tool was written in Python — using ChatGPT — because the creator didn’t know any Python.   
It works anyway.  
