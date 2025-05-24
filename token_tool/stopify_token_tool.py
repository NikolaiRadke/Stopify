# The tool was written in Python using ChatGPT because the creator doesn't know Python.

import threading
import webbrowser
from flask import Flask, request, render_template_string, redirect
import requests
import urllib.parse

app = Flask(__name__)

CLIENT_ID = ''
CLIENT_SECRET = ''
REDIRECT_URI = 'http://localhost:8080/callback'
SCOPES = 'user-read-private user-read-email'

HTML_TEMPLATE = '''
<!DOCTYPE html>
<html>
<head>
    <title>Stopify Token Tool</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            background-color: #191414;
            color: #1DB954;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            flex-direction: column;
        }}
        .container {{
            background-color: #282828;
            padding: 30px;
            border-radius: 8px;
            box-shadow: 0 0 20px rgba(29, 185, 84, 0.5);
            text-align: center;
            width: 400px;
        }}
        input[type=text], input[type=password] {{
            width: 100%;
            padding: 10px;
            margin: 10px 0 20px 0;
            border: none;
            border-radius: 4px;
        }}
        button {{
            background-color: #1DB954;
            color: #191414;
            padding: 12px 20px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            font-weight: bold;
            font-size: 1em;
        }}
        button:hover {{
            background-color: #17a34a;
        }}
        code {{
            background: #1DB954;
            color: #191414;
            padding: 8px 12px;
            border-radius: 4px;
            font-size: 1.1em;
            word-break: break-all;
            display: inline-block;
            margin-top: 20px;
        }}
        a {{
            color: #1DB954;
            text-decoration: none;
            margin-top: 15px;
            display: inline-block;
        }}
    </style>
</head>
<body>
    <div class="container">
        {content}
    </div>
</body>
</html>
'''

@app.route('/', methods=['GET', 'POST'])
def index():
    global CLIENT_ID, CLIENT_SECRET
    if request.method == 'POST':
        CLIENT_ID = request.form.get('client_id').strip()
        CLIENT_SECRET = request.form.get('client_secret').strip()
        if not CLIENT_ID or not CLIENT_SECRET:
            content = '''
            <h2>Stopify Token Tool</h2>
            <p style="color:#ff4c4c;">Bitte Client-ID und Client-Secret eingeben.</p>
            ''' + login_form()
            return render_template_string(HTML_TEMPLATE.format(content=content))
        # Build Spotify auth URL
        params = {
            'client_id': CLIENT_ID,
            'response_type': 'code',
            'redirect_uri': REDIRECT_URI,
            'scope': SCOPES,
            'show_dialog': 'true'
        }
        url = 'https://accounts.spotify.com/authorize?' + urllib.parse.urlencode(params)
        return redirect(url)
    else:
        content = '''
        <h2>Stopify Token Tool</h2>
        ''' + login_form()
        return render_template_string(HTML_TEMPLATE.format(content=content))

def login_form():
    return '''
    <form method="post">
        <input type="text" name="client_id" placeholder="Client-ID" required>
        <input type="password" name="client_secret" placeholder="Client-Secret" required>
        <button type="submit">Login bei Spotify</button>
    </form>
    '''

@app.route('/callback')
def callback():
    code = request.args.get('code')
    if not code:
        content = '<h2>Fehler</h2><p>Kein Code erhalten.</p>'
        return render_template_string(HTML_TEMPLATE.format(content=content))

    # Tausche Code gegen Token
    auth_response = requests.post(
        'https://accounts.spotify.com/api/token',
        data={
            'grant_type': 'authorization_code',
            'code': code,
            'redirect_uri': REDIRECT_URI,
            'client_id': CLIENT_ID,
            'client_secret': CLIENT_SECRET
        },
        headers={'Content-Type': 'application/x-www-form-urlencoded'}
    )

    if auth_response.status_code != 200:
        content = f'<h2>Fehler</h2><p>Fehler beim Abrufen des Tokens:</p><code>{auth_response.text}</code>'
        return render_template_string(HTML_TEMPLATE.format(content=content))

    json_response = auth_response.json()
    refresh_token = json_response.get('refresh_token')

    content = f'''
        <h2>✅ Refresh Token erhalten</h2>
        <p><strong>Refresh Token:</strong></p>
        <code>{refresh_token}</code>
        <p>Du kannst dieses Fenster nun schließen.</p>
        <a href="/">Zurück zur Anmeldung</a>
    '''
    return render_template_string(HTML_TEMPLATE.format(content=content))


def start_flask():
    app.run(port=8080)

if __name__ == '__main__':
    threading.Thread(target=start_flask).start()
    webbrowser.open("http://localhost:8080")


