set -e
echo "[*]Install.."
pip3 install flask
echo "[*]Compiling.."
gcc -Wall -O2 ./src/module/main.c ./src/module/downloadModule.c -o SimpleDownloader
echo "[*]Set Server"
echo "python3 ./src/server/server.py" > server_run.sh
chmod +x server_run.sh

echo "[*] completed"
