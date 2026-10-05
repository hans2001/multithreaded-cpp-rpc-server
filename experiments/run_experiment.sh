#!/bin/bash
# Run one PA2 experiment (customers = 1..256) from the CLIENT machine and save a CSV.
#
# Usage:
#   ./run_experiment.sh <exp 1-4> <server_ip> <port> [server_host]
#
#   server_host (optional): Khoury hostname of the server machine, e.g. linux-072.khoury.northeastern.edu
#     - given  : the script starts/stops ./server on that machine over ssh (needs passwordless ssh;
#                home dirs are shared on Khoury, so the same src path works there)
#     - omitted: the script tells you what to run on the server machine and waits for Enter
#
# Env vars:
#   SRC_DIR     directory with the built ./client and ./server (default: ../src next to this script)
#   TARGET_SEC  target run time per data point in seconds (default 20; README wants 10..60;
#               the probe slightly overestimates throughput, so real runs land a bit under target)
#   CUSTOMERS   override the customer list (default "1 2 4 8 16 32 64 128 256")
#
# Output: results/exp<N>.csv with columns
#   exp,customers,experts,orders_per_customer,avg_us,min_us,max_us,throughput,wall_s

set -u

EXP=${1:?usage: $0 <exp 1-4> <server_ip> <port> [server_host]}
SERVER_IP=${2:?missing server_ip}
PORT=${3:?missing port}
SERVER_HOST=${4:-}

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
SRC_DIR=${SRC_DIR:-$SCRIPT_DIR/../src}
SRC_DIR=$(cd "$SRC_DIR" && pwd)
TARGET_SEC=${TARGET_SEC:-20}
CUSTOMERS=${CUSTOMERS:-"1 2 4 8 16 32 64 128 256"}
OUT_DIR=$SCRIPT_DIR/results
OUT=$OUT_DIR/exp$EXP.csv

case $EXP in
  1) ROBOT=0; FIXED_EXPERTS=1  ;;   # regular robots; expert count does not matter
  2) ROBOT=1; FIXED_EXPERTS=1  ;;
  3) ROBOT=1; FIXED_EXPERTS=16 ;;
  4) ROBOT=1; FIXED_EXPERTS="" ;;   # experts = customers, server restarted for every point
  *) echo "exp must be 1..4" >&2; exit 1 ;;
esac

if [ ! -x "$SRC_DIR/client" ]; then
  echo "ERROR: $SRC_DIR/client not found. Run 'make' in $SRC_DIR first." >&2; exit 1
fi
if [ "$PORT" -lt 10000 ] || [ "$PORT" -gt 65535 ]; then
  echo "ERROR: port must be 10000..65535" >&2; exit 1
fi

echo "client host : $(hostname)"
echo "server      : $SERVER_IP:$PORT  (ssh host: ${SERVER_HOST:-manual})"
echo "somaxconn   : $(cat /proc/sys/net/core/somaxconn 2>/dev/null) (listen backlog is capped by this)"
case " $(hostname -I 2>/dev/null) " in
  *" $SERVER_IP "*) echo "WARNING: server_ip is THIS machine. README requires client and server on different machines." >&2 ;;
esac

mkdir -p "$OUT_DIR"
[ -f "$OUT" ] || echo "exp,customers,experts,orders_per_customer,avg_us,min_us,max_us,throughput,wall_s" > "$OUT"

start_server() {   # $1 = number of experts
  if [ -n "$SERVER_HOST" ]; then
    ssh -o BatchMode=yes "$SERVER_HOST" "pkill -u \$USER -x server; cd '$SRC_DIR' && nohup ./server $PORT $1 >/dev/null 2>&1 &" \
      || { echo "ERROR: ssh to $SERVER_HOST failed (set up passwordless ssh, or omit server_host)" >&2; exit 1; }
    sleep 1
  else
    echo
    echo ">>> On the SERVER machine: stop any running server (Ctrl+C), then run:"
    echo ">>>     cd $SRC_DIR && ./server $PORT $1"
    read -r -p ">>> Press Enter when the server is running... " _
  fi
}

stop_server() {
  [ -n "$SERVER_HOST" ] && ssh -o BatchMode=yes "$SERVER_HOST" "pkill -u \$USER -x server" 2>/dev/null
  return 0
}

run_client() {     # $1 customers, $2 orders -> prints "avg min max thr wall" or nothing on failure
  local t0 t1 line
  t0=$(date +%s.%N)
  line=$("$SRC_DIR/client" "$SERVER_IP" "$PORT" "$1" "$2" "$ROBOT" 2>/dev/null | tail -n 1)
  t1=$(date +%s.%N)
  [ -z "$line" ] && return 1
  echo "$line $(echo "$t1 - $t0" | bc)"
}

[ -n "$FIXED_EXPERTS" ] && start_server "$FIXED_EXPERTS"

for C in $CUSTOMERS; do
  EXPERTS=${FIXED_EXPERTS:-$C}
  [ -z "$FIXED_EXPERTS" ] && start_server "$EXPERTS"

  # 1) short probe run to estimate throughput, 2) size the real run to ~TARGET_SEC seconds
  PROBE_ORDERS=$(( (2000 + C - 1) / C )); [ $PROBE_ORDERS -lt 5 ] && PROBE_ORDERS=5
  read -r _ _ _ PTHR _ <<< "$(run_client "$C" "$PROBE_ORDERS")" || true
  if [ -z "${PTHR:-}" ]; then echo "ERROR: probe run failed for customers=$C (is the server up?)" >&2; exit 1; fi
  ORDERS=$(echo "$TARGET_SEC * $PTHR / $C" | bc)
  [ "$ORDERS" -lt 10 ] && ORDERS=10

  read -r AVG MIN MAX THR WALL <<< "$(run_client "$C" "$ORDERS")" || true
  if [ -z "${WALL:-}" ]; then echo "ERROR: run failed for customers=$C" >&2; exit 1; fi

  printf "exp%s customers=%-3s experts=%-3s orders=%-7s avg=%-10s min=%-6s max=%-9s thr=%-10s wall=%.1fs\n" \
    "$EXP" "$C" "$EXPERTS" "$ORDERS" "$AVG" "$MIN" "$MAX" "$THR" "$WALL"
  if (( $(echo "$WALL < 10" | bc) )); then echo "  WARNING: run shorter than 10 s; raise TARGET_SEC" >&2; fi
  if (( $(echo "$WALL > 60" | bc) )); then echo "  WARNING: run longer than 60 s; lower TARGET_SEC" >&2; fi
  echo "$EXP,$C,$EXPERTS,$ORDERS,$AVG,$MIN,$MAX,$THR,$WALL" >> "$OUT"
done

stop_server
echo "saved -> $OUT"
