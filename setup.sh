set -e
echo "[*]Install.."
pip3 install flask
echo "[*]Compiling.."
gcc ./src/module/main.c ./src/module/downloadModule.c -o SimpleDownloader
echo "[*]Set Server"
echo "python3 ./src/server/server.py" > server_run.sh
chmod +x server_run.sh
