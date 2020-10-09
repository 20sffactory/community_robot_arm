import serial
import time

#PORT DECLARATIONS
port = '/dev/cu.usbserial-14530' #SEE PORT NAME IN ARDUINO IDE: TOOLS>PORT:
baud = 115200 #BY DEFAULT 115200
timeout = None #LEAVE AS IS
ser = serial.Serial(port,baud,timeout=timeout)
time.sleep(2)

#LIST OF COMMANDS TO BE EXECUTED BY ROBOT ARM, CHANGE ACCORDINGLY
cmdList = [
"G0 X0 Y120 Z120 F165",
"G0 X0 Y120 Z60 F165",
"G0 X0 Y120 Z120 F165",
"G0 X0 Y120 Z60 F165",
"G0 X0 Y120 Z120 F165",
"G0 X100 Y120 Z60 F165",
]

bCmdList = []
for cmd in cmdList:
    cmd_temp = cmd + '\r'
    bCmdList.append(cmd_temp.encode('utf-8'))

#ROBOT HOMES ITSELF
ser.write(b'G28\r')
print("homing in progress")
time.sleep(10)
print(ser.readline())

def wait_complete():
    waitstatus = 1
    while True:
        a = ser.readline()
        if "ok" in a.decode("utf-8"):
            waitstatus = 0
            break

#ROBOT BEGINS WHEN USER INPUTS "y"
begin = input('type y to begin: ')

while True:
    if begin == 'y':
        break

for bCmd in bCmdList:
    ser.write(bCmd)
    print(bCmd)
    wait_complete()