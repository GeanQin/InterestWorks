from flask import Flask
import os
app = Flask(__name__)

@app.route('/d')
def hello_world():
    os.system("toDesktop.exe")
    return 'get d'

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=5000)