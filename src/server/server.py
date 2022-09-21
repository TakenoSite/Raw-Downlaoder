from flask import Flask,send_file

server = Flask(__name__)

@server.route("/100byte")
def file_100():
    return send_file("./static/file_100Byte")
@server.route("/1kbyte")
def file_1k():
    return send_file("./static/file_1kbyte")
@server.route("/1mbyte")
def file_1m():
    return send_file("./static/file_1mbyte")

if __name__ == "__main__":
    server.run(debug=True ,port=81)
