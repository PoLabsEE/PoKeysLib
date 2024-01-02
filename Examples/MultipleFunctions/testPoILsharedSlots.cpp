#include "testPoILsharedSlots.h"

void testPoILsharedSlots(sPoKeysDevice * devPtr)
{
    int32_t slotValue = 0;

    // Read something from shared slot 0, add 100 to it and write it back to shared slot 1
    PK_PoILReadSharedSlot(devPtr, 0, 1, &slotValue);
    slotValue += 100;
    PK_PoILWriteSharedSlot(devPtr, 0, 1, &slotValue);
}
