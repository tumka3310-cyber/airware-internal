#!/system/bin/sh
set -e

DIR="$(cd "$(dirname "$0")" && pwd)"
SO="$DIR/libairware.so"
INJ="$DIR/injector"
TARGET="com.axlebolt.standoff2"

[ ! -f "$SO" ]  && { echo "[-] missing $SO";  exit 1; }
[ ! -f "$INJ" ] && { echo "[-] missing $INJ"; exit 1; }
chmod 755 "$INJ" "$SO"

[ "$(id -u)" != "0" ] && { echo "[-] root required"; exit 1; }

echo "[*] waiting for $TARGET ..."
PID=""
for i in $(seq 1 180); do
  PID="$(pidof "$TARGET" 2>/dev/null || true)"
  [ -z "$PID" ] && PID="$(pgrep -f "$TARGET" 2>/dev/null | head -n1 || true)"
  [ -n "$PID" ] && break
  sleep 1
done
[ -z "$PID" ] && { echo "[-] timeout"; exit 1; }

echo "[+] pid = $PID"
sleep 3
echo "[*] injecting"
"$INJ" "$PID" "$SO" || { echo "[-] injector failed"; exit 1; }
echo "[+] airware active"
