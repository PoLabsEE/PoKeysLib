#include "test1wire.h"

#include "Windows.h" // for sleep()
#include <iostream>
#include <stddef.h>

using namespace std;

void test1wire(sPoKeysDevice * devPtr)
{
    uint8_t stat = 0;
    uint8_t data[16];
    uint8_t len;

    printf("\nReading 1-wire device ID...");

    // Activate 1-wire
    PK_1WireStatusSet(devPtr, 1);

    // Read current 1-wire device ID
    data[0] = 0x33;
    PK_1WireWriteReadStart(devPtr, 1, 8, data);

    // Wait 10 ms for operation to be completed by the PoKeys device (all calls are non-blocking)
    Sleep(10);

    PK_1WireReadStatusGet(devPtr, &stat, &len, data);

    // Check the return value...
    if (stat == 1)
    {
        printf("\nID: ");

        for (int n = 0; n < 8; n++)
        {
            printf("%02X ", data[n]);
        }

        // Now read the temperature if the ID starts with 0x10 (DS18S20 sensor)
        if (data[0] == 0x10)
        {
            printf("\n");
            while(1)
            {
                printf("\rReading temperature: ");
                // Create temperature start request
                data[0] = 0xCC;
                data[1] = 0x44;
                PK_1WireWriteReadStart(devPtr, 2, 0, data); // send 2 bytes, read none

                // Wait 1 second
                Sleep(1000);

                // Create scratchpad read request
                data[0] = 0xCC;
                data[1] = 0xBE;
                PK_1WireWriteReadStart(devPtr, 2, 9, data); // send 2 bytes, read 9

                // Sleep for up to 1 second
                for (int i = 0; i < 100; i++)
                {
                    // Sleep
                    Sleep(10);

                    PK_1WireReadStatusGet(devPtr, &stat, &len, data);

                    if (stat == 0x10) // 1-wire is still busy
                        continue;
                    else
                        break;
                }

                if (stat == 1)
                {
                    int16_t temperature = 100 * (int16_t)(((uint16_t)data[1] << 8) + data[0]) / 2;
                    printf("%d.%02u degC              ", temperature / 100, temperature % 100);
                }
                else
                {
                    printf("Error!");
                }
            }
        }
    }
    else
    {
        printf("\nError reading ID");
    }
}
