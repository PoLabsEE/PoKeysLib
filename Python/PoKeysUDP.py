import socket
import select
import time

class PoKeysUDP:
    def __init__(self) -> None:
        self.devSock = None        
        self.requestID = 0

    def Connect(self, deviceIP) -> bool:
        self.devSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.devSock.setblocking(False)
        self.deviceIP = deviceIP

        return self.DeviceInfoGet() != None

    def _customRequest(self, data):
        if self.devSock is None:
            return None

        if len(data) != 64:
            raise Exception("Invalid request length") 

        data[0] = 0xBB

        for rW in range(2):
            self.requestID = (self.requestID + 1) % 256            
            data[6] = self.requestID
            data[7] = 0
            for b in range(7):
                data[7] = (data[7] + data[b]) % 256
            
            # Send data
            try:
                #print(f"Sending request: {data.hex()}")
                self.devSock.sendto(data, (self.deviceIP, 20055))                
            except:
                continue

            # Read response
            for rR in range(20):
                dataResp = None
                try:
                    dataReady = select.select([self.devSock], [], [], 0.05)
                    if dataReady[0]:
                        dataResp = self.devSock.recv(64)
                        #print(f"Response: {dataResp.hex()}")
                    else:
                        continue
                except:
                    continue

                if dataResp[0] == 0xAA and dataResp[6] == self.requestID:
                    return dataResp
        
        raise Exception("No response from device")


    def _prepareRequest(self, cmd, p, pExt):
        req = bytearray([0]*64)
        req[1] = cmd

        if p != None and len(p) > 0:
            req[2:2+len(p)] = p

        if pExt != None and len(pExt) > 0:
            req[8:8+len(pExt)] = pExt

        return req

    def DeviceInfoGet(self):
        resp = self._customRequest(self._prepareRequest(0x00, None, None))

        serial = int.from_bytes(resp[12:15], 'little')

        return { 
            'DeviceSerial' : serial
        }

    def PinFunctionSet(self, pinID, pinFunction):
        _ = self._customRequest(self._prepareRequest(0x10, [pinID - 1, pinFunction, 0], None))

    def DigitalInputsGet(self):
        resp = self._customRequest(self._prepareRequest(0xCC, None, None))

        pinStates = {}

        for i in range(55):
            pinStates[i + 1] = (resp[int(i / 8 + 8)] & (1 << (i % 8))) > 0

        return pinStates     

    def DigitalOutputSet(self, pinID, state):
        _ = self._customRequest(self._prepareRequest(0x40, [pinID - 1, 0 if state else 1], None))

    def AnalogInputsGet(self, in_Volts = False):
        resp = self._customRequest(self._prepareRequest(0x3A, None, None))

        analogInputs = {}

        factor = 3.3 / 4095 if in_Volts else 1

        for i in range(7):
            analogInputs[41 + i] = factor * int.from_bytes(resp[8+i*2:10+i*2], 'big')

        return analogInputs  

    def EasySensorsConfigGet(self, maxCount = 100):
        sensors = []

        sensorTypes = {
            0x10: "LM75",
            0x11: "SHT21",
            0x23: "Si7020",
            0x12: "BME280",
            0x13: "MCP9600",
            0x14: "iAQ-Core C",
            0x18: "DS18S20",
            0x19: "DS18B20",
            0x1A: "DS2413",
            0x20: "DHT11",
            0x21: "DHT21",
            0x22: "DHT22 / AM2302",
            0x41: "Si1141",
            0x40: "BH1750",
            0x50: "MCP3425",
            0x60: "MMA7660",
            0x61: "LIS2DH12",
            0xF0: "Analog input",
            0x80: "SimpleSensor"            
        }

        for s in range(maxCount):
            # Retrieve s-th EasySensors configuration
            sensorData = None
            try:
                resp = self._customRequest(self._prepareRequest(0x76, [s, 1], None))                
                if resp[8] != 0:    

                    # Read value
                    respValue = self._customRequest(self._prepareRequest(0x77, [s, 1], None))
                    sensorData = { 
                        'Value' : float(int.from_bytes(respValue[8:12], 'little')) / 100,
                        'Type' : sensorTypes.get(resp[8], f"Type {resp[8]}"),
                        'ReadingID' : resp[9],
                        'RefreshPeriod' : resp[10],
                        'FailsafeConfig' : resp[11],
                        'ID' : resp[12:20]
                    }
            except:
                pass

            sensors.append(sensorData)
        return sensors

    def EasySensorsValuesGet(self, maxCount = 100):
        sensorsValues = []

        for s in range(0, maxCount, 13):
            readNum = 13
            if s + 13 > maxCount:
                readNum = maxCount - s

            try:
                resp = self._customRequest(self._prepareRequest(0x77, [s, readNum], None))                                
                for t in range(readNum):
                    sensorsValues.append(float(int.from_bytes(resp[8+t*4:12+t*4], 'little')) / 100)

            except:
                for t in range(readNum):
                    sensorsValues.append(None)
                pass

        return sensorsValues           


    # I2C operations status return ePK_I2C_STATUS, described above
    
    # Retrieves I2C bus activation status
    def I2CStatusGet(self) -> int:
        resp = self._customRequest(self._prepareRequest(0xDB, [0x02], None))     
        return resp[3]                           
        
    # Execute write to the specified address
    def I2CWriteStart(self, address, buffer):
        _ = self._customRequest(self._prepareRequest(0xDB, [0x10, address, len(buffer), 0], bytes(buffer)))     

    # Execute write and read operation to the specified address
    def I2CWriteAndReadStart(self, address, buffer, bytesToRead):
        _ = self._customRequest(self._prepareRequest(0xDB, [0x10, address, len(buffer), bytesToRead], bytes(buffer)))     

    # Get write operation status
    def I2CWriteStatusGet(self):
        resp = self._customRequest(self._prepareRequest(0xDB, [0x11], None))     
        return resp[3]                           

    # Execute read from the specified address. iDataLength specifies how many bytes should be requested
    def I2CReadStart(self, address, iDataLength):
        _ = self._customRequest(self._prepareRequest(0xDB, [0x20, address, iDataLength], None))     
        return True
    
    # Get read operation results. iReadBytes returns the number of bytes read from the selected device, iMaxBufferLength specifies how many bytes buffer can accept
    def I2CReadStatusGet(self):
        resp = self._customRequest(self._prepareRequest(0xDB, [0x21], None))     
        if resp[3] == 1:
            if resp[9] > 32:
                return None
            return resp[10:10 + resp[9]]
        else:
            return []  

    def I2CWrite(self, address, data) -> bool:        
        self.I2CWriteStart(address, data)
        return self.I2CWriteWaitResponse()        

    def I2CReadWaitResponse(self):
        # Wait for response
        errCnt = 0
        while True:
            try:
                data = self.I2CReadStatusGet()

                if data is not None:
                    if len(data) > 0:
                        return data
                    else:
                        time.sleep(0.01)
                return None
            except:
                errCnt += 1
                if errCnt > 10:
                    break                

    def I2CWriteWaitResponse(self):
        # Wait for response
        errCnt = 0
        while True:
            try:
                status = self.I2CWriteStatusGet()

                if status == 0x10:
                    time.sleep(0.01)
                    continue
                elif status == 0x01:
                    return []
            except:
                errCnt += 1
                if errCnt > 10:
                    break   

    def I2CRead(self, address, dataNum):
        self.I2CReadStart(address, dataNum)
        return self.I2CReadWaitResponse()
        
    
    def I2CWriteAndRead(self, address, data, bytesToRead):
        self.I2CWriteAndReadStart(address, data, bytesToRead)

        if bytesToRead == 0:
            return self.I2CWriteWaitResponse()
        else:
            return self.I2CReadWaitResponse()    

    # Execute bus scan
    def I2CBusScanStart(self):
        _ = self._customRequest(self._prepareRequest(0xDB, [0x30], None))     
    
    # Get bus scan results. iMaxDevices specifies how big presentDevices buffer is. presentDevices returns one entry per device
    def I2CBusScanGetResults(self):
        devices = []

        resp = self._customRequest(self._prepareRequest(0xDB, [0x31], None)) 
        print(f"Scan status: {resp[3]}")
        if resp[3] != 1:
            return None

        for i in range(128):
            if (resp[int(9 + i / 8)] & (1 << (i % 8))) > 0:
                devices.append(i)
        
        return devices
    

if __name__ == "__main__":
    # Test PoKeysUDP
    IP = "192.168.88.112"
    dev = PoKeysUDP()
    
    print(f"Connecting to target device at {IP}")
    if dev.Connect(IP):
        devData = dev.DeviceInfoGet()
        print(f"Connected to device {devData['DeviceSerial']}")

        print("\nDigital inputs:")
        print(dev.DigitalInputsGet())

        # Set pin 20 as inverted digital output
        dev.PinFunctionSet(20, 4 + 128)
        dev.DigitalOutputSet(20, False)

        # Set pin 41 as analog input
        dev.PinFunctionSet(41, 8)

        print("\nAnalog inputs:")
        print(dev.AnalogInputsGet(False)) # Set to True to return results in Volts

        print("\nEasySensors:")
        sensors = dev.EasySensorsConfigGet()
        print(sensors)

        values = dev.EasySensorsValuesGet()
        print(values)

        print("\nRunning I2C bus scan...")
        dev.I2CBusScanStart()
        time.sleep(0.5)
        print("I2C address scan results:")
        devList = dev.I2CBusScanGetResults()
        print([hex(addr) for addr in devList])

        if 0x48 in devList:        
            # Is it a LM75?
            print("\nLM75 temperature:")

            # Use address 0x48, set pointer to registry 0x00 (write), then read 2 bytes
            result = dev.I2CWriteAndRead(0x48, [0x00], 2)
            if result is not None:
                T = int.from_bytes(result, 'big') / 256.0
                print(T)
            else:
                print("Result not available, try again")

            # Or maybe an ADS7828?

            print("\nADS7828 A/D")
            # Measure single-ended on ch.6, keep ADC on afterwards
            result = dev.I2CWriteAndRead(0x48, [0xBC], 2)
            if result is not None:
                ADC_in = int.from_bytes(result, 'big')
                print(ADC_in)
            else:
                print("Result not available, try again")

    else:
        print("Device not available")
