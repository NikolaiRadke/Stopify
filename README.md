![Stopify](http://www.nikolairadke.de/stopify/stopify_back_2.jpg)


# What the... are they sawing again?

You're on your own and have just put on an audiobook, when the noise starts. That was clear. Stopify pauses playback until the noise stops. Easy to build, easy to configure. Save your nerves and build stopify today!  

*Oje, Englisch? Kann ich nicht. Schnell [hier hin](https://github.com/NikolaiRadke/Stopify/wiki)*.  
  
Stopify is an audio analysis tool that connects to your Wifi and Spotify account. While a track is playing, it analyzes the ambient noise and pauses Spotify at certain frequencies, 
such as those produced by a drill and and resumes playback as soon as the disturbance is over.

What's new?  
* **23.05.2025** First commit. Hello world.    
    -- More news? See the [newsblog](https://github.com/NikolaiRadke/Stopify/tree/main/NEWS.md).
     
### Prerequisites
  
Except for the tools and some time, you need:
* Arduino IDE 1.6.6 or higher
* ESP32 core support and the base64 library
* Optional an audio analyis app, like Spectroid (Android) or SpectrumView (IOS)
* ESP32 Board with dual core, like the standard ESP32 Dev Kit
* MAX9814 microphone
* A power source for the ESP32

### Content
  
```
Stopify/
Root directory with some explanation files.  
|
├── src/
|   └── Stopify/
|       The source code. Only one file.
└── token_tool/
    The python tool to get the refresh token.
    ├── Linux/
    |   Precompiled Linux tool. Only one file.
    └── Windows/
        Precompiled Windows tool. Only one file.
```

### Let's get started!

Ready? Great! Let's start here: [German wiki](https://github.com/NikolaiRadke/Stopify/wiki).  



