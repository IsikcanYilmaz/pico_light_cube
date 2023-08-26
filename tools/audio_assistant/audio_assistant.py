#!/usr/bin/env python3
# import serial
# def main():
#     # ser = serial.Serial(PICO_SERIAL_FD)
#     # with serial.Serial() as ser:
#     #     ser.baudrate = 19200
#     #     ser.port = PICO_SERIAL_FD
#     #     ser.open()
#     #     for i in range(0, 10):
#     #         print(ser.readline())
#     #     ser.close()
#
#     return
#
#     ser = serial.Serial(PICO_SERIAL_FD)
#     try:
#         data = []
#         for i in range(0, 10000):
#             lineStr = str(ser.readline())[2:-6]
#             lineArr = [float(i) for i in lineStr.split(" ")]
#             print(lineArr)
#             data.append(lineArr)
#         ser.close()
#  
#     except Exception as e:
#         print(e)
#         ser.close()
#
#


import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
import random
import time
import serial

PICO_SERIAL_FD = "/dev/cu.usbmodem144101"
NUM_FREQS = 400

fig, ax = plt.subplots()
ser = None

def initSerial():
    global ser
    try:
        ser = serial.Serial(PICO_SERIAL_FD)
        # ser.baudrate = 19200
        # ser.port = PICO_SERIAL_FD
        # ser.open()
        return True
    except Exception as e:
        print(e)
        return False

def deinitSerial():
    global ser
    ser.close()

def readFreqs():
    global ser
    linearr = [float(i) for i in ser.readline().decode().replace(" \r\n","").split(" ")]
    return linearr

def animate(i):
    global ser
    # y = [random.randint(0,100) for j in range(0, 100)]
    y = readFreqs()
    x = range(0, NUM_FREQS)
    ax.clear()
    ax.plot(x, y)
    ax.set_xlim([0,NUM_FREQS])
    ax.set_ylim([0,500000])

def main():
    global ser
    if not initSerial():
        print("ERROR!")
        return
    ani = FuncAnimation(fig, animate, frames = 20, interval = 1, repeat = True)
    plt.show()
    deinitSerial()
    print("DONE!")

if __name__ == "__main__":
    main()
