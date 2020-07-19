from picamera import PiCamera
from time import sleep
from gpiozero import Button
from signal import pause
import serial
from datetime import datetime

print("starting")

ser = serial.Serial('/dev/ttyACM0',9600, timeout=1)
ser.flush()

camera = PiCamera()
camera.rotation = 180
camera.resolution = (720,480)

programbtn = Button(17,pull_up=False)
lockbtn = Button(27,pull_up=False)
unlockbtn = Button(22,pull_up=False)

def program():
    print("sending p")
    ser.write(b'p')

def lock():
    print("sending l")
    ser.write(b'l')

def unlock():
    print("sending u")
    ser.write(b'u')

def got_unlock():
    print("got u")
    curtime = datetime.now().strftime("%Y%m%d-%H%M%S")
    camera.capture('/home/pi/Desktop/logphotos/%sUa.jpg' % curtime)
    sleep(2)
    curtime = datetime.now().strftime("%Y%m%d-%H%M%S")
    camera.capture('/home/pi/Desktop/logphotos/%sUb.jpg' % curtime)
    sleep(2)
    curtime = datetime.now().strftime("%Y%m%d-%H%M%S")
    camera.capture('/home/pi/Desktop/logphotos/%sUc.jpg' % curtime)
    
def got_lock():
    print("got l")
    curtime = datetime.now().strftime("%Y%m%d-%H%M%S")
    camera.capture('/home/pi/Desktop/logphotos/%sL.jpg' % curtime)

programbtn.when_pressed = program
lockbtn.when_pressed = lock
unlockbtn.when_pressed = unlock

while True:
    x = ser.readline()
    if x == b'u':
        got_unlock()
    elif x == b'l':
        got_lock()
    elif x:
        print(x)
    

