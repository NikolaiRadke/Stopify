![Stopify](http://www.nikolairadke.de/stopify/stopify_back_3.jpg)
# What the... are they sawing again?

You're home alone, just sat down with an audiobook – and then it starts. Noise. That was clear. Of course. **Stopify** pauses 
the playback until the noise stops. Simple to build, simple to configure. Save your nerves and build Stopify today!

# WARNING: STOPIFY IS NOT WORKING RIGHT NOW!
Spotiy switched from OAuth2 to PKCE authenticifation process. I have to rework almost everything.

*Oje, Englisch? Kann ich nicht. Schnell [hier hin](https://github.com/NikolaiRadke/Stopify/wiki)*.  

![Stopify](http://www.nikolairadke.de/stopify/stopify_3.jpg)
  
Stopify is an audio analysis tool that connects to your Wi-Fi and Spotify account. While a track is playing, it monitors ambient 
noise and pauses Spotify playback when specific sound patterns are detected – such as those produced by a vacuum cleaner, drill, 
or similar. Playback resumes automatically once the noise is gone.

🆕 What's new?  
* **26.05.2025** Reworking auth process from OAuth2 to PKCE. *Sigh*.  
    -- More news? Check the [newsblog](https://github.com/NikolaiRadke/Stopify/tree/main/NEWS.md).
     
### Prerequisites
  
Besides some tools and a bit of time, you'll need:
* Arduino IDE 1.6.6 or newer
* ESP32 core support, the base64 and the arduinoFFT libraries
* (Optional) An audio analysis app like Spectroid (Android) or SpectrumView (IOS)
* AN ESP32 Board with dual-core, like the standard **ESP32 Dev Kit**
* **MAX9814** microphone
* A power source for the ESP32
* A 2.4 GHz Wifi access

### Content
  
```
Stopify/
Root directory with some explanation files.  
|
├── src/
|   └── Stopify/
|       The source code. Only one file.
└── token_tool/
    The Python tool to get the refresh token.
    ├── Linux/
    |   Precompiled Linux tool. Only one file.
    └── Windows/
        Precompiled Windows tool. Only one file.
```

### Let's get started!

Ready? Great! Let's start here: [German wiki](https://github.com/NikolaiRadke/Stopify/wiki).  



