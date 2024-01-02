#include <iostream>
#include "PoKeysLib.h"
#include <stddef.h>

#include "test1wire.h"
#include "testPoILsharedSlots.h"

#include "Windows.h" // Only for Sleep functionality

using namespace std;

int main()
{
    int usbDevNum = 0;
    int netDevNum = 0;

    long devSerial = 0;

    printf("PoKeys example project using PoKeysLib\n");

    printf("Enumerating USB devices... \t\t");
    usbDevNum = PK_EnumerateUSBDevices();
    printf("%d devices found!\n", usbDevNum);

    sPoKeysNetworkDeviceSummary netDevices[16];
    printf("Enumerating network devices...\t\t ");
    netDevNum = PK_EnumerateNetworkDevices(netDevices, 1000);
    printf("%d devices found!\n", netDevNum);

    printf("\n----------------------\n");


    // This is how the connection with network PoKeys devices is established if
    // IP of the device is stored in netDevices structure - see universal call below
    if (netDevNum > 0)
    {
        // Connect to first network device...
        netDevices[0].useUDP = 1; // Use UDP connection
        sPoKeysDevice * netDevice = PK_ConnectToNetworkDevice(&netDevices[0]);

        if (netDevice != NULL)
        {
            devSerial = netDevice->DeviceData.SerialNumber;
            printf("Connection established to network PoKeys device with serial number %d\n", devSerial);

            // Do something here...

            // Disconnect
            PK_DisconnectDevice(netDevice);
        } else
        {
            printf("Connecting to network PoKeys device failed!\n");
        }
    }

    // This is how the connection with USB PoKeys devices is established - see universal call below
    if (usbDevNum > 0)
    {
        // Connect to first USB device
        sPoKeysDevice * dev = PK_ConnectToDevice(0);

        if (dev != NULL)
        {
            devSerial = dev->DeviceData.SerialNumber;
            printf("Connection established to USB PoKeys device with serial number %d\n", devSerial);

            PK_DisconnectDevice(dev);
        } else
        {
            printf("Connecting to USB PoKeys device failed!\n");
        }
    }

    if (devSerial == 0) return 0;

		// Preferred method to establish connection with PoKeys device is by using
		// serial number directly
    devSerial = 45000;

    // This is the universal call for establishing the connection with devices
    sPoKeysDevice * devPtr = PK_ConnectToDeviceWSerial_UDP(devSerial, 1000);
    PK_SetEthernetRetryCountAndTimeout(devPtr, 2, 2, 20);

    if (devPtr != NULL)
    {
        printf("Connected to %s (serial %d)\n", devPtr->DeviceData.DeviceName, devPtr->DeviceData.SerialNumber);

        // Read the configuration
        PK_PinConfigurationGet(devPtr);
        PK_EncoderConfigurationGet(devPtr);
        PK_LCDConfigurationGet(devPtr);
        PK_MatrixLEDConfigurationGet(devPtr);

        /*
        // List all encoders and their settings
        for (int e = 0; e < devPtr->info.iEncodersCount; e++)
        {
            printf("Encoder %d: A=%d, B=%d, settings=%d\n", e+1, devPtr->Encoders[e].channelApin, devPtr->Encoders[e].channelBpin, devPtr->Encoders[e].encoderOptions);
        }*/


        // Set pin 1 as digital input
        devPtr->Pins[0].PinFunction = PK_PinCap_digitalInput;

        // Set pin 2 as digital output
        devPtr->Pins[1].PinFunction = PK_PinCap_digitalOutput;

        // Set pin 3 as inverted digital input
        devPtr->Pins[2].PinFunction = PK_PinCap_digitalInput | PK_PinCap_invertPin;

        // Set encoder 1 to use pins 4 and 5
        devPtr->Pins[3].PinFunction = PK_PinCap_digitalInput;
        devPtr->Pins[4].PinFunction = PK_PinCap_digitalInput;

        devPtr->Encoders[0].channelApin = 3;
        devPtr->Encoders[0].channelBpin = 4;
        devPtr->Encoders[0].encoderOptions = 3; // enable encoder with 4x sampling
        // Send the configuration
        PK_PinConfigurationSet(devPtr);
        PK_EncoderConfigurationSet(devPtr);


        // Set digital output on pin 2
        devPtr->Pins[1].DigitalValueSet = 1;
        PK_DigitalIOSet(devPtr);

        // Read digital input on pin 1
        PK_DigitalIOGet(devPtr);
        unsigned char input = devPtr->Pins[0].DigitalValueGet;

        // Set and read digital IO
        devPtr->Pins[1].DigitalValueSet = 0;
        PK_DigitalIOSetGet(devPtr);
        input = devPtr->Pins[0].DigitalValueGet;

        // Set output on pin 2
        PK_DigitalIOSetSingle(devPtr, 1, 1);

        // Read input on pin 3
        PK_DigitalIOGetSingle(devPtr, 2, &input);


        // Read encoder value
        PK_EncoderValuesGet(devPtr);
        long encoderValue = devPtr->Encoders[0].encoderValue;



        // Matrix keyboard
        //        Pin 1: Column B   -> input
        //        Pin 2: Row 1      -> output
        //        Pin 3: Column A   -> input
        //        Pin 4: Row 4      -> output
        //        Pin 5: Column C   -> input
        //        Pin 7: Row 2      -> output
        //        Pin 8: Row 3      -> output

        // Read matrix keyboard current configuration
        PK_MatrixKBConfigurationGet(devPtr);

        // Configure pins
        devPtr->Pins[0].PinFunction = PK_PinCap_digitalInput;
        devPtr->Pins[1].PinFunction = PK_PinCap_digitalOutput;
        devPtr->Pins[2].PinFunction = PK_PinCap_digitalInput;
        devPtr->Pins[3].PinFunction = PK_PinCap_digitalOutput;
        devPtr->Pins[4].PinFunction = PK_PinCap_digitalInput;
        devPtr->Pins[6].PinFunction = PK_PinCap_digitalOutput;
        devPtr->Pins[7].PinFunction = PK_PinCap_digitalOutput;

        // Configure matrix keyboard
        devPtr->matrixKB.matrixKBheight = 4;    // 4 rows
        devPtr->matrixKB.matrixKBwidth = 3;     // 3 columns

        devPtr->matrixKB.matrixKBrowsPins[0] = 1;   // Row 1
        devPtr->matrixKB.matrixKBrowsPins[1] = 6;   // Row 2
        devPtr->matrixKB.matrixKBrowsPins[2] = 7;   // Row 3
        devPtr->matrixKB.matrixKBrowsPins[3] = 3;   // Row 4

        devPtr->matrixKB.matrixKBcolumnsPins[0] = 2;    // Column A
        devPtr->matrixKB.matrixKBcolumnsPins[1] = 0;    // Column B
        devPtr->matrixKB.matrixKBcolumnsPins[2] = 4;    // Column C

        devPtr->matrixKB.matrixKBconfiguration = 1; // Matrix keyboard is enabled

        PK_MatrixKBConfigurationSet(devPtr);

        /*
         *
         * Do other stuff here...
         *
         */

        // Read matrix keyboard status
        PK_MatrixKBStatusGet(devPtr);

        // Each key of the matrix keyboard row is bit-mapped to matrixKBvalues (bit 0 = column A)
        devPtr->matrixKB.matrixKBvalues[0]; // Contains bit-mapped keys of the first row
        devPtr->matrixKB.matrixKBvalues[1]; // Contains bit-mapped keys of the second row
        devPtr->matrixKB.matrixKBvalues[2]; // Contains bit-mapped keys of the third row
        devPtr->matrixKB.matrixKBvalues[3]; // Contains bit-mapped keys of the forth row

        testPoILsharedSlots(devPtr);
        test1wire(devPtr);

        // Set pin 45 as analog input
        devPtr->Pins[44].PinFunction = PK_PinCap_analogInput;
        PK_PinConfigurationSet(devPtr);

        printf("\n");
        for (int k = 0; k < 2000; k++)
        {
            // Read analog input on pin 45
            if (PK_AnalogIOGet(devPtr) == PK_OK)
            {
                long analogInput = devPtr->Pins[44].AnalogValue;

                printf("\rAnalog: %u    ", analogInput);
            } else
            {
                printf("\nError in communication. Quitting!");
                PK_DisconnectDevice(devPtr);
                devPtr = NULL;
                break;
            }            
        }

        // Check if we still have the pointer to the device
        if (devPtr == NULL) return 0;

        // Enable and setup 4x20 LCD on primary pins
        devPtr->LCD.Configuration = 1;
        devPtr->LCD.Rows = 4;
        devPtr->LCD.Columns = 20;
        PK_LCDConfigurationSet(devPtr);

        // Write something to LCD
        sprintf_s((char *)devPtr->LCD.line1, 20, "This is line 1");
        devPtr->LCD.RowRefreshFlags |= (1<<0); // Set bit 0 in this flag to 1 to refresh the first line of the LCD

        sprintf_s((char *)devPtr->LCD.line3, 20, "This is line 3");
        devPtr->LCD.RowRefreshFlags |= (1<<2); // Set bit 2 in this flag to 1 to refresh the third line of the LCD

        PK_LCDUpdate(devPtr);

        // Fill some random data to PoExtBus buffer
        for (int i = 0; i < 10; i++)
        {
            devPtr->PoExtBusData[i] = i;
        }
        PK_PoExtBusSet(devPtr);



        printf("Scanning I2C bus...\n");
        PK_I2CBusScanStart(devPtr);
        Sleep(1000);
        uint8_t status, devices[128];
        PK_I2CBusScanGetResults(devPtr, &status, devices, 128);
        if (status == PK_I2C_STAT_COMPLETE)
        {
            printf("The following I2C devices were found on the I2C bus:\n");
            for (int i = 0; i < 128; i++)
            {
                if (devices[i] != 0)
                {
                    printf("[%X] ", i);
                }
            }
        }
        printf("\nDone\n");





        PK_DisconnectDevice(devPtr);

        printf("Complete!\n");
    }

    return 0;
}

