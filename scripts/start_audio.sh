#!/bin/sh
set -eux

jackd -r -d dummy -r 48000 -p 640 &
echo "$!" > /run/jackd.pid

gst-launch-1.0 filesrc location=sample.wav ! wavparse ! audioconvert ! audioresample ! audio/x-raw,format=F32LE ! jackaudiosink &
echo "$!" > /run/gst.pid

wait
