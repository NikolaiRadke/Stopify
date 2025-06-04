# The token tool was written in Python — using ChatGPT — because the creator didn’t know any Python.   
# It works anyway. 
#
# Requires flask and requsts  
# See security.txt for security informations

import os
import sys
import time
import base64
import hashlib
import secrets
import threading
import urllib.parse
import webbrowser
from flask import Flask, request, redirect, render_template_string
import requests


app = Flask(__name__)
code_verifier = ""
client_id = ""
redirect_uri = "http://127.0.0.1:8888/callback"

HTML_FORM = """
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Stopify PKCE Tool</title>
    <style>
        body {
            background-color: #1db954;
            font-family: Arial, sans-serif;
            color: white;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            height: 100vh;
            margin: 0;
        }
        h1 {
            margin-bottom: 20px;
        }
        input {
            padding: 10px;
            font-size: 16px;
            width: 300px;
            border-radius: 5px;
            border: none;
            margin-bottom: 10px;
        }
        button {
            background-color: white;
            color: #1db954;
            font-weight: bold;
            padding: 10px 20px;
            border: none;
            font-size: 16px;
            border-radius: 5px;
            cursor: pointer;
        }
        button.shutdown {
            background-color: #ff4d4d;
            color: white;
            margin-top: 20px;
        }
        .token-box {
            background: white;
            color: #1db954;
            padding: 20px;
            border-radius: 10px;
            width: 80%;
            max-width: 700px;
            font-size: 14px;
            word-break: break-all;
            margin-top: 30px;
            text-align: center;
        }
    </style>
</head>
<body>
    <h1>🎧 Stopify PKCE Tool</h1>
    <form action="/auth" method="post">
        <input type="text" name="client_id" placeholder="Spotify Client ID" required><br>
        <button type="submit">Login mit Spotify</button>
    </form>
</body>
</html>
"""

@app.route('/')
def index():
    return render_template_string(HTML_FORM)

@app.route('/auth', methods=['POST'])
def auth():
    global code_verifier, client_id
    client_id = request.form['client_id']
    code_verifier = secrets.token_urlsafe(64)
    code_challenge = base64.urlsafe_b64encode(
        hashlib.sha256(code_verifier.encode()).digest()
    ).rstrip(b'=').decode('utf-8')

    params = {
        "client_id": client_id,
        "response_type": "code",
        "redirect_uri": redirect_uri,
        "code_challenge_method": "S256",
        "code_challenge": code_challenge,
        "scope": "user-read-playback-state user-modify-playback-state"
    }

    auth_url = "https://accounts.spotify.com/authorize?" + urllib.parse.urlencode(params)
    return redirect(auth_url)

@app.route('/callback')
def callback():
    global code_verifier, client_id
    code = request.args.get('code')
    if not code:
        return "Fehler: Kein Code erhalten."

    token_data = {
        "grant_type": "authorization_code",
        "code": code,
        "redirect_uri": redirect_uri,
        "client_id": client_id,
        "code_verifier": code_verifier
    }

    headers = {"Content-Type": "application/x-www-form-urlencoded"}
    response = requests.post("https://accounts.spotify.com/api/token", data=token_data, headers=headers)

    if response.status_code == 200:
        tokens = response.json()
        refresh = tokens.get("refresh_token")
        return f"""
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Stopify Tokens</title>
    <style>
        body {{
            background-color: #1db954;
            font-family: Arial, sans-serif;
            color: white;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            height: 100vh;
            margin: 0;
            padding: 20px;
        }}
        h2 {{
            color: white;
            margin-bottom: 10px;
        }}
        .token-box {{
            background: white;
            color: #1db954;
            padding: 20px;
            border-radius: 10px;
            width: 80%;
            max-width: 700px;
            font-size: 14px;
            word-break: break-word;
            margin-bottom: 20px;
        }}
        button {{
            background-color: white;
            color: #1db954;
            font-weight: bold;
            padding: 10px 20px;
            border: none;
            font-size: 16px;
            border-radius: 5px;
            cursor: pointer;
        }}
        p {{
            text-align: center;
            margin-top: 20px;
            font-size: 14px;
        }}
    </style>
</head>
<body>
    <h2>✅ Refresh Token erhalten!</h2>
    <div class="token-box">{refresh}</div>
    <p><em>Kopiere den Token und füge ihn in deinen ESP32-Sketch ein.</em></p>
    <form action="/shutdown" method="post">
        <button type="submit">🛑 Server beenden</button>
    </form>
</body>
</html>
"""
    else:
        return f"<h2>Fehler</h2><pre>{response.text}</pre>"

@app.route('/shutdown', methods=['POST'])
def shutdown():
    func = request.environ.get('werkzeug.server.shutdown')
    if func is None:
        print("❌ Werkzeug Shutdown nicht verfügbar, benutze os._exit(0)")
        # Hier starten wir den Exit in 1 Sekunde, keine Access auf request in Thread nötig
        threading.Timer(1.0, lambda: os._exit(0)).start()
    else:
        # Shutdown in Thread, um sofort Antwort zurückzugeben
        threading.Thread(target=func).start()

    # Abschiedsseite sofort zurückgeben
    return """
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset="utf-8">
        <title>Auf Wiedersehen!</title>
        <style>
            body {
                background-color: #1db954;
                font-family: Arial, sans-serif;
                color: white;
                display: flex;
                flex-direction: column;
                align-items: center;
                justify-content: center;
                height: 100vh;
                margin: 0;
            }
            h2 {
                margin-bottom: 10px;
            }
            p {
                font-size: 16px;
            }
        </style>
    </head>
    <body>
        <h2>👋 Server wurde beendet</h2>
        <p>Du kannst das Fenster jetzt schließen.</p>
    </body>
    </html>
    """


def open_browser():
    webbrowser.open("http://127.0.0.1:8888")

if __name__ == '__main__':
    threading.Timer(1, open_browser).start()
    app.run(port=8888, debug=True, use_reloader=False)
