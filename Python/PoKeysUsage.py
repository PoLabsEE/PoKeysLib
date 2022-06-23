# Example of PoKeys Python library usage
#
# Copyright (C) 2014 Matev\vz Bo\vsnak (matevz@poscope.com)
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

from PoKeys import *

import time


# Enter the device's serial number here
deviceSerial = 45000


# Load PoKeysLib dll library and list all PoKeys devices detected
mydevice = PoKeysDevice("./PoKeyslib.dll")
print("List of detected devices ------------------------------------")
mydevice.ShowAllDevices(1000)

sys.exit()

# Connect to a specific PoKeys device, use UDP connection if possible
print("Connecting to the selected device...")
if mydevice.PK_ConnectToDeviceWSerial(deviceSerial, 1, True) != 0:
    print("Device not found, quitting!")
    sys.exit(0)


def dump(name, obj):
   for attr in dir(obj):
       if not attr.startswith('_'):
           if hasattr( obj, attr ):
               print(name + ".%s = %s" % (attr, getattr(obj, attr)))

#dump("info", mydevice.device.contents.info)
#dump("DeviceData", mydevice.device.contents.DeviceData)

# Read pin configuration
mydevice.PK_PinConfigurationGet()


# Pin I/O...
testIO = False
if testIO:
    # Set pin 1 as digital output
    mydevice.device.contents.Pins[0].PinFunction = ePK_PinCap.PK_PinCap_digitalOutput

    # Set pins 41-45 as analog inputs
    for pin in range(40, 45):
        mydevice.device.contents.Pins[pin].PinFunction = ePK_PinCap.PK_PinCap_analogInput

    # Send configuration
    mydevice.PK_PinConfigurationSet()

    # Toggle the pin 1 and check its value
    print("Step 0: " + str(mydevice.device.contents.Pins[0].DigitalValueGet))
    mydevice.PK_DigitalIOSetSingle(0, 0)
    mydevice.PK_DigitalIOGet()
    print("Step 1: " + str(mydevice.device.contents.Pins[0].DigitalValueGet))
    mydevice.PK_DigitalIOSetSingle(0, 1)
    mydevice.PK_DigitalIOGet()
    print("Step 2: " + str(mydevice.device.contents.Pins[0].DigitalValueGet))

    # Read analog inputs
    mydevice.PK_AnalogIOGet()

    # List all pins
    print("Pin functions and values -------------------------------------")
    for pin in range(0, 55):
        if mydevice.device.contents.Pins[pin].PinFunction == ePK_PinCap.PK_PinCap_analogInput:
            print("Pin " + str(pin) + " (" + str(mydevice.device.contents.Pins[pin].PinFunction) + "): Analog value=" +
                                             "%1.3f"%(3.3 * mydevice.device.contents.Pins[pin].AnalogValue / 4096) + " V")
        else:    
            print("Pin " + str(pin) + " (" + str(mydevice.device.contents.Pins[pin].PinFunction) + "): Digital value=" +
                                             str(mydevice.device.contents.Pins[pin].DigitalValueGet))

testPoExtBus = True
if testPoExtBus:
		mydevice.device.contents.PoExtBusData[0] = 0
		mydevice.device.contents.PoExtBusData[1] = 127
		mydevice.device.contents.PoExtBusData[2] = 255
		mydevice.PK_PoExtBusSet()
		
testPWM = False        
if testPWM:
    # Set PWM outputs
    # Set pins 17-22 as inactive pins
    for pin in range(16, 21):
        mydevice.device.contents.Pins[pin].PinFunction = ePK_PinCap.PK_PinCap_pinRestricted

    # Send configuration
    mydevice.PK_PinConfigurationSet()

    # Read PWM configuration
    mydevice.PK_PWMConfigurationGet()

    # Setup PWM outputs
    # Due to backwards compatibility to PoKeys55 devices, output indexes are inversed
    # PWM output with index 0 is found on pin 22
    # ...
    # PWM output with index 5 is found on pin 17

    # Set the PWM frequency to 5 kHz -> set the period to PWM internal frequency / target PWM frequency
    mydevice.device.contents.PWM.PWMperiod = int(mydevice.device.contents.info.PWMinternalFrequency / 5000)
    # Enable PWM on pins 17 and 18
    mydevice.device.contents.PWM.PWMenabledChannels[5] = 1
    mydevice.device.contents.PWM.PWMenabledChannels[4] = 1
    # Set the duty cycles to 50% (pin 17) and 25% (pin 18)
    mydevice.device.contents.PWM.PWMduty[5] = int(0.5 * mydevice.device.contents.PWM.PWMperiod)
    mydevice.device.contents.PWM.PWMduty[4] = int(0.25 * mydevice.device.contents.PWM.PWMperiod)
    # Update the PWM configuration (pins, period)
    mydevice.PK_PWMConfigurationSet()
    # Update duty cycles
    mydevice.PK_PWMUpdate()


testI2C = False
if testI2C:
    # I2C commands

    # Scan I2C bus 
    mydevice.PK_I2CBusScanStart()
    time.sleep(1) # Wait 1 second for the scan to complete
    devices = mydevice.PK_I2CBusScanGetResults()
    for i in range(0, 128):
        if devices[i] == 1:
            print("Device found at I2C address of " + hex(i))

    if devices[0x19] == 1:
        # Some test code for the LSM330DLC sensor
        # Write 0x20, 0x44 to device at address 0x19 (write 0x44 to register 0x20)
        mydevice.PK_I2CWrite(0x19, [0x20, 0x44])

        # Write 0xA0 to device at 0x19 (set pointer to register 0xA0), then read 32 bytes
        mydevice.PK_I2CWrite(0x19, [0xA0])
        data = mydevice.PK_I2CRead(0x19, 32)
        # Print all registers
        print(data)


        # Read repeatedly 2 bytes from register 0xAC
        for repeat in range(0, 200):
          mydevice.PK_I2CWrite(0x19, 0xA8+4)
          data = mydevice.PK_I2CRead(0x19, 2)

          value = data[1] * 256 + data[0]
          print(int(getSignedNumber(value,16)))


testSPI = False
if testSPI:
    mydevice.PK_SPIConfigure(250, 0)        # 100 kHz clock, default frame
    dataOut = [ 1, 2, 3, 4, 5, 6 ]
    dataIn = mydevice.PK_SPI(dataOut, 8)    # Use pin 8 as chip select  
    print("SPI data out:", dataOut)
    print("SPI data in:", dataIn)


#mydevice.PK_MatrixKBConfigurationGet()
#print("Matrix keyboard:", mydevice.device.contents.matrixKB.matrixKBwidth, mydevice.device.contents.matrixKB.matrixKBheight)

#mydevice.PK_LCDConfigurationGet()
#LCD = mydevice.device.contents.LCD;
#print("LCD: ", LCD.Configuration, LCD.Rows, LCD.Columns)



#s = mydevice.device.contents.info
#for field_name, field_type in s._fields_:
#    print(field_name, getattr(s, field_name))



def PEv2_example1(dev):
    # ******************************************************************************************
    # Configuration
    # ******************************************************************************************
    print("Configuring pulse engine...")

    # Setup pulse engine
    dev.device.contents.PEv2.PulseEngineEnabled = 8   # Enable 8 axes
    dev.device.contents.PEv2.ChargePumpEnabled = 0    # Don't enable safety charge pump
    dev.device.contents.PEv2.PulseGeneratorType = 0 | (1<<7) # Use PoKeysCNCaddon

    dev.device.contents.PEv2.EmergencySwitchPolarity = 0 	# Normal emergency switch polarity (NC switch)
    #dev.device.contents.PEv2.EmergencySwitchPolarity = 1 	# Inverted emergency switch polarity (NO switch)
    dev.device.contents.PEv2.AxisEnabledStatesMask = 0 	# Disable axis power when not in Running state

    dev.PK_PEv2_PulseEngineSetup()


    axisID = 0 # Select the axis to configure (0 to 7)

    print("Configuring axis", axisID)
    # Setup the axis {axisID}
    # - use internal planner in velocity mode and soft limits enabled
    dev.device.contents.PEv2.AxesConfig[axisID] = ePK_PEv2_AxisConfig.PK_AC_ENABLED | ePK_PEv2_AxisConfig.PK_AC_INTERNAL_PLANNER
    # - axis is equipped with home switch only
    dev.device.contents.PEv2.AxesSwitchConfig[axisID] = ePK_PEv2_AxisSwitchOptions.PK_ASO_SWITCH_HOME
    # - use dedicated home input on the PoKeysCNCaddon (pin ID 0) or PoKeys pin (pin ID as printed on the PoKeys device)
    dev.device.contents.PEv2.PinHomeSwitch[axisID] = 0

    # - max speed: 75 kHz
    dev.device.contents.PEv2.MaxSpeed[axisID] = 75
    # - max acceleration: 30 kHz / s
    dev.device.contents.PEv2.MaxAcceleration[axisID] = 0.030
    # - max deceleration: 60 kHz / s
    dev.device.contents.PEv2.MaxDecceleration[axisID] = 0.060

    # - use 10% of max speed for homing
    dev.device.contents.PEv2.HomingSpeed[axisID] = 10
    # - use 50% of the homing speed for fine homing
    dev.device.contents.PEv2.HomingReturnSpeed[axisID] = 50
    
    # - don't use MPG encoder for jogging
    dev.device.contents.PEv2.MPGjogEncoder[axisID] = 0
    dev.device.contents.PEv2.MPGjogMultiplier[axisID] = 0

    # - use dedicated axis power pin on PoKeysCNCaddond
    dev.device.contents.PEv2.AxisEnableOutputPins[axisID] = 0

    # Save axis index to param1
    dev.device.contents.PEv2.param1 = axisID
    dev.PK_PEv2_AxisConfigurationSet()
    
    




    
    # ******************************************************************************************
    # Test
    # ******************************************************************************************
  
    print("Setting axis 1 position to 0...")
    dev.device.contents.PEv2.PositionSetup[0] = 0
    dev.device.contents.PEv2.param2 = (1<<0)
    dev.PK_PEv2_PositionSet()

    print("Changing state to RUNNING...")
    # Change mode to 'RUNNING'
    dev.device.contents.PEv2.PulseEngineStateSetup = ePK_PEState.PK_PEState_peRUNNING
    dev.PK_PEv2_PulseEngineStateSet()

    print("Checking state...")
    # Check the state...
    dev.PK_PEv2_StatusGet()
    if dev.device.contents.PEv2.PulseEngineState != ePK_PEState.PK_PEState_peRUNNING:
        print("Pulse engine not in running state. Quiting!")
        return


    print("Moving at constant speed for 2 seconds")
    # Set the constant velocity mode for 2 seconds
    dev.device.contents.PEv2.ReferencePositionSpeed[0] = 9000
    dev.PK_PEv2_PulseEngineMove()    
    time.sleep(2)

    print("Stopping...")
    # STOP!
    dev.device.contents.PEv2.ReferencePositionSpeed[0] = 0
    dev.PK_PEv2_PulseEngineMove()
    time.sleep(0.5)


    print("Changing to position mode...")
    # Change to position mode and jump between two points for 5 times
    dev.device.contents.PEv2.AxesConfig[0] |= ePK_PEv2_AxisConfig.PK_AC_POSITION_MODE
    dev.device.contents.PEv2.param1 = 0
    dev.PK_PEv2_AxisConfigurationSet()

    for repeat in range(5):
        print("Go to 0")
        # Go to 0
        dev.device.contents.PEv2.ReferencePositionSpeed[0] = 0
        dev.PK_PEv2_PulseEngineMove()

        # Wait for position...
        for wait in range(200):
            if abs(dev.device.contents.PEv2.CurrentPosition[0] - dev.device.contents.PEv2.ReferencePositionSpeed[0]) < 10:
                break

            dev.PK_PEv2_StatusGet()
            time.sleep(0.01)

        print("Go to +5000")
        # Go to +5000
        dev.device.contents.PEv2.ReferencePositionSpeed[0] = 5000
        dev.PK_PEv2_PulseEngineMove()

        # Wait for position...
        for wait in range(200):
            if abs(dev.device.contents.PEv2.CurrentPosition[0] - dev.device.contents.PEv2.ReferencePositionSpeed[0]) < 10:
                break

            dev.PK_PEv2_StatusGet()
            time.sleep(0.01)

    print("Go to 0")
    # Go to 0
    dev.device.contents.PEv2.ReferencePositionSpeed[0] = 0
    dev.PK_PEv2_PulseEngineMove()

    time.sleep(1)

    # Switch to STOPPED
    print("Changing state to STOPPED...")
    dev.device.contents.PEv2.PulseEngineStateSetup = ePK_PEState.PK_PEState_peSTOPPED
    dev.PK_PEv2_PulseEngineStateSet()




testPE = True
if testPE:
    print("Testing Pulse engine...")
    mydevice.PK_PEv2_StatusGet()
    PE = mydevice.device.contents.PEv2

    print("Pulse engine status")
    print(" - info (axes): ", PE.info.nrOfAxes)
    print(" - Enabled: ", PE.PulseEngineEnabled)
    print(" - Activated: ", PE.PulseEngineActivated)
    print(" - State: ", PE.PulseEngineState)
    print(" - Charge pump enabled: ", PE.ChargePumpEnabled)
    print(" - Pulse generator type: ", PE.PulseGeneratorType)

    # Retrieve configuration of all axes
    for i in range(8):
        PE.param1 = i
        mydevice.PK_PEv2_AxisConfigurationGet()        

    print(" - Axes states: ", convertToPythonArray(PE.AxesState, 8))
    print(" - Axes config: ", convertToPythonArray(PE.AxesConfig, 8))
    print(" - Axes switch config: ", convertToPythonArray(PE.AxesSwitchConfig, 8))
    print(" - Axes positions: ", convertToPythonArray(PE.CurrentPosition, 8))


    PEv2_example1(mydevice)


    

# Read RTC
mydevice.PK_RTCGet()
print("Time: %02d:%02d:%02d" % (mydevice.device.contents.RTC.HOUR, mydevice.device.contents.RTC.MIN, mydevice.device.contents.RTC.SEC))





testEasySensors = False
autoAdd1Wire = True

if testEasySensors:
    if mydevice.device.contents.info.iEasySensors > 0:

        print("Retrieving sensor configuration...")
        mydevice.PK_EasySensorsSetupGet()
        print("Retrieving sensor values...")
        mydevice.PK_EasySensorsValueGetAll()
        #print(mydevice.device.contents)

        existingROMs = []
        
        # Print the configuration
        for i in range(mydevice.device.contents.info.iEasySensors):
            S = mydevice.device.contents.EasySensors[i]

            if S.sensorType != 0:
                print("Sensor ", i, ": value=", float(S.sensorValue) / 100)
                print(" - type: ", S.sensorType)
                print(" - reading: ", S.sensorReadingID)
                print(" - refreshPeriod: ", S.sensorRefreshPeriod)
                print(" - failsafe: ", S.sensorFailsafeConfig)

                print(" - ID", ":".join("{:02x}".format(c) for c in S.sensorID))

                # Save 1-wire ROMs
                if S.sensorType >= 0x18 and S.sensorType <= 0x1A:
                    types = { 0x18:0x10, 0x19:0x28, 0x1A:0x3A }
                    tmp = [S.sensorID[i] for i in range(8)]
                    tmp[0] = types[S.sensorType]

                    existingROMs.append(tmp)


        # Search for new 1-wire sensors
        pin = 54
        print("1-wire sensors found on pin ", pin+1)
        ROMs = mydevice.PK_1WireScan(pin, 5)
        print(ROMs)

        if autoAdd1Wire:
            # Configure all that haven't been added before...
            for s in range(len(ROMs)):
                newROM = ROMs[s]

                result = False
                # Check if it exists already...
                for e in range(len(existingROMs)):
                    if all([existingROMs[e][i] == newROM[i] for i in range(8)]):
                        result = True
                        break

                # If ROM has already been found, just skip to next one...
                if result == True:
                    continue            

                # Find first empty slot...
                slot = -1
                for i in range(100):                
                    if mydevice.device.contents.EasySensors[i].sensorType == 0:
                        slot = i
                        break

                if slot >= 0:
                    print("Saving ROM: ", newROM, " to EasySensors slot ", slot)
                    # Add 1-wire sensor to the list
                    mydevice.PK_EasySensorConfigure_1wire(slot, pin, newROM, 0, 10, 0)


print("Done...")
mydevice.Disconnect()

