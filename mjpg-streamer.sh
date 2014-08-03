#!/bin/sh
./mjpg_streamer -i "./input_uvc.so" -o "./output_http.so -p 8090 -w /home/pi/dev/picar/web"
