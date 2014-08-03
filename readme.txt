This is the software portion of my project to make a browser controlled remote car.
Read this for more details: <blog post link to be added> 

HW Used:
Dagu Chassis (Includes motors/tires and the plates)
Dagu Pan/Tilt kit (Servos and brackets to mount the camera)
Dagu Mini Driver (Arduino clone + Motor driver)
Raspberry Pi
A netgear ethernet to wifi adapter (Since I didn't have the raspi cam so had to save a USB port and use this instead of USB wifi adapter)
A cheap old USB webcam
6 AA Cells
A sony portable charger to power the raspberry pi and wifi adapter
Wires/Screws/Spacers/Tools (lots of them)

Features:
- Control the car from any browser
- Can control motion as well as camera pan/tilt from browser using virtual joysticks (Requires multitouch to do pan/tilt as second joystick is needed for this)
- Streams back video from car camera to the browser
- Can have multiple connections to the car (not recommended for controlling)
- 

How to:
- Setup the HW and SW (detailed in the blog)
- Start mjpg-streamer using mjpg-streamer.sh
- Start controller.py (python controller.py)
- Open browser to http://rpi:8090 (or http://<RaspberryPiIPAddress>:8090) from any device
- Touch anywhere to start controlling the car. Left half of screen corresponds to car movement joystick. Right half of screen corresponds to camera pan/tilt joystick. You can use either one of them or both simultaneously.
