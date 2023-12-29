#!/bin/sh
set -u

for p in gst jackd; do
    pid_file="/run/$p.pid"
    if [ -f "$pid_file" ]; then
        kill "$(cat $pid_file)" 2>/dev/null
        rm "$pid_file"
    fi
done
