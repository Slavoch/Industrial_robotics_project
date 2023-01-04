import argparse
import serial
import time

def GripperManipulator(statusint, COM):
    #COM = 'COM' + str(COM)
    status = str(statusint)
    arduino=serial.Serial(COM, 9600)  
    time.sleep(2)
    if status == '1':
        arduino.write(b'1')
        #print("Gripper opened")
    elif status == '0':
        arduino.write(b'0')
        #print("Gripper closed")

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--statusint', dest='statusint', type=int)
    parser.add_argument('--COM', dest='COM', type=str)
    args = parser.parse_args()
    GripperManipulator(args.statusint,args.COM)

