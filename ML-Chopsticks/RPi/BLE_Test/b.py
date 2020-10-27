import struct
import binascii
import bluepy
from bluepy import btle

# DELEGATE CLASS FOR NOTIFICATION


class MyDelegate(btle.DefaultDelegate):
    def __init__(self, name):
        btle.DefaultDelegate.__init__(self)
        self.name = name

    # MESSAGE PARSING AND GPIO CONTROL
    def handleNotification(self, cHandle, data):
        #print("handleNotification()")
        print(data)


# BLE DEVICE SETTINGS
p = btle.Peripheral("AA:BB:CC:DD:EE:FF", "public")
p.setDelegate(MyDelegate("CHOP"))

svc1 = p.getServiceByUUID('bea5760d-503d-4920-b000-101e7306b000')
print(svc1.getCharacteristics())
ch1 = svc1.getCharacteristics()[0]

print(ch1.valHandle)

p.writeCharacteristic(ch1.valHandle+1, bytes("\x01", encoding='utf8'))

# WAIT FOR NOTIFICATIONS
try:
    while True:
        if p.waitForNotifications(1.0):
            continue
except Exception as e:
    print(str(e))