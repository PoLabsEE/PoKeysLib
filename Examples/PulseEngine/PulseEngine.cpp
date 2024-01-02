#include <QCoreApplication>
#include "PoKeysLib.h"
#include <iostream>
#include "conio.h"
#include "Windows.h"

void example1(sPoKeysDevice * dev)
{
    printf("Configuring pulse engine...\n");

    // Setup pulse engine
    dev->PEv2.PulseEngineEnabled = 3;   // Enable 3 axes
    dev->PEv2.ChargePumpEnabled = 0;    // Don't enable safety charge pump

    //dev->PEv2.PulseGeneratorType = 1;   // Use internal pulse generator
    //dev->PEv2.PulseGeneratorType = 0;   // Use external pulse generator and no IO
    dev->PEv2.PulseGeneratorType = 0 | (1<<7); // Use external pulse generator and IO (PoKeysCNCaddon)

    dev->PEv2.EmergencySwitchPolarity = 0; // Normal emergency switch polarity (NC switch)
    //dev->PEv2.EmergencySwitchPolarity = 1; // Inverted emergency switch polarity (NO switch)
    dev->PEv2.AxisEnabledStatesMask = (1<<0); // Enable axis power in 'STOPPED' state - see 'PoKeys protocol documentation', 0x85/0x01 Setup pulse engine

    PK_PEv2_PulseEngineSetup(dev);

    // Reboot the pulse engine - do this if generator type or AxisEnabledStatesMask have been changed
    printf("Rebooting...\n");
    PK_PEv2_PulseEngineReboot(dev);

    // Wait for a few moment...
    Sleep(1000);


    printf("Configuring axis 1...\n");
    // Setup the axis 1 (index 0)
    // - use internal planner in velocity mode and soft limits enabled
    dev->PEv2.AxesConfig[0] = PK_AC_ENABLED | PK_AC_INTERNAL_PLANNER | PK_AC_SOFT_LIMIT_ENABLED;
    // - axis is equipped with home and limit+ switches, limit+ switch has inverted polarity (NO switch)
    //dev->PEv2.AxesSwitchConfig[0] = PK_ASO_SWITCH_HOME | PK_ASO_SWITCH_LIMIT_P | PK_ASO_SWITCH_INVERT_LIMIT_P;
    // - use dedicated home input on the PoKeysCNCaddon
    dev->PEv2.PinHomeSwitch[0] = 0;
    // - use pin 10 for limit+ switch input
    dev->PEv2.PinLimitPSwitch[0] = 10;

    // - max speed: 15 kHz
    dev->PEv2.MaxSpeed[0] = 15;
    // - max acceleration: 30 kHz / s
    dev->PEv2.MaxAcceleration[0] = 0.030f;
    // - max deceleration: 60 kHz / s
    dev->PEv2.MaxDecceleration[0] = 0.060f;

    // - use 10% of max speed for homing
    dev->PEv2.HomingSpeed[0] = 10;
    // - use 50% of the homing speed for fine homing
    dev->PEv2.HomingReturnSpeed[0] = 50;
    // - don't use MPG encoder for jogging
    dev->PEv2.MPGjogEncoder[0] = 0;
    dev->PEv2.MPGjogMultiplier[0] = 0;

    // - soft limit-: -100.000
    dev->PEv2.SoftLimitMinimum[0] = -100000;
    // - soft limit+: +100.000
    dev->PEv2.SoftLimitMaximum[0] = 100000;

    // - use dedicated axis power pin on PoKeysCNCaddond
    dev->PEv2.AxisEnableOutputPins[0] = 0;


    // Save axis index to param1
    dev->PEv2.param1 = 0;
    PK_PEv2_AxisConfigurationSet(dev);

    printf("Setting axis 1 position to 0...\n");
    dev->PEv2.PositionSetup[0] = 0;
    dev->PEv2.param2 = (1<<0);
    PK_PEv2_PositionSet(dev);

    printf("Changing state to RUNNING...\n");
    // Change mode to 'RUNNING'
    dev->PEv2.PulseEngineStateSetup = PK_PEState_peRUNNING;
    PK_PEv2_PulseEngineStateSet(dev);

    printf("Checking state...\n");
    // Check the state...
    PK_PEv2_StatusGet(dev);
    if (dev->PEv2.PulseEngineState != PK_PEState_peRUNNING)
    {
        printf("Pulse engine not in running state. Quiting!\n");
        return;
    }


    printf("Moving at constant speed for 2 seconds\n");
    // Set the constant velocity mode for 2 seconds
    dev->PEv2.ReferencePositionSpeed[0] = 9000;
    PK_PEv2_PulseEngineMove(dev);
    Sleep(2000);

    printf("Stopping...\n");
    // STOP!
    dev->PEv2.ReferencePositionSpeed[0] = 0;
    PK_PEv2_PulseEngineMove(dev);
    Sleep(500);


    printf("Changing to position mode...\n");
    // Change to position mode and jump between two points for 5 times
    dev->PEv2.AxesConfig[0] |= PK_AC_POSITION_MODE;
    dev->PEv2.param1 = 0;
    PK_PEv2_AxisConfigurationSet(dev);

    for (int repeat = 0; repeat < 5; repeat++)
    {
        printf("Go to 0\n");
        // Go to 0
        dev->PEv2.ReferencePositionSpeed[0] = 0;
        PK_PEv2_PulseEngineMove(dev);

        // Wait for position...
        for (int wait = 0; wait < 200 && (abs(dev->PEv2.CurrentPosition[0]-dev->PEv2.ReferencePositionSpeed[0])>10); wait++)
        {
            PK_PEv2_StatusGet(dev);
            Sleep(10);
        }

        printf("Go to +5000\n");
        // Go to +5000
        dev->PEv2.ReferencePositionSpeed[0] = 5000;
        PK_PEv2_PulseEngineMove(dev);

        // Wait for position...
        for (int wait = 0; wait < 200 && (abs(dev->PEv2.CurrentPosition[0]-dev->PEv2.ReferencePositionSpeed[0])>10); wait++)
        {
            PK_PEv2_StatusGet(dev);
            Sleep(10);
        }
    }

    printf("Go to 0\n");
    // Go to 0
    dev->PEv2.ReferencePositionSpeed[0] = 0;
    PK_PEv2_PulseEngineMove(dev);

    Sleep(1000);

    // Switch to STOPPED
    printf("Changing state to STOPPED...\n");
    dev->PEv2.PulseEngineStateSetup = PK_PEState_peSTOPPED;
    PK_PEv2_PulseEngineStateSet(dev);

}

void example2(sPoKeysDevice * dev)
{
    const int pts = 10*1000;
    float tmp[pts];
    uint8_t buffer[pts];
    float diff;
    int bufferInd = 0;

    // Create some motion...
    for (int t = 0; t < pts; t++)
    {
        tmp[t] = 1000*cos(6.28f*(float)t/1000) + 500*sin(6.28f*4.0f*(float)t/1000)*((float)t/pts);
        if (t > 0)
        {
            diff = (tmp[t]-tmp[t-1]);
            buffer[t] = abs((int8_t)diff) & 127;
            if (diff < 0) buffer[t] |= (1<<7);
        } else buffer[t] = 0;
    }


    // Check if pulse engine is enabled...
    if (dev->PEv2.PulseEngineEnabled != 3)
    {
        printf("Run example1 first");
        return;
    }


    // Configure axis 1 for buffer mode...
    printf("Changing to buffer mode...\n");
    dev->PEv2.AxesConfig[0] &= ~(PK_AC_INTERNAL_PLANNER);
    dev->PEv2.param1 = 0;
    PK_PEv2_AxisConfigurationSet(dev);

    // Clear buffer
    PK_PEv2_BufferClear(dev);

    // Switch to running mode...
    printf("Changing state to RUNNING...\n");
    // Change mode to 'RUNNING'
    dev->PEv2.PulseEngineStateSetup = PK_PEState_peRUNNING;
    PK_PEv2_PulseEngineStateSet(dev);

    printf("Checking state...\n");
    // Check the state...
    PK_PEv2_StatusGet(dev);
    if (dev->PEv2.PulseEngineState != PK_PEState_peRUNNING)
    {
        printf("Pulse engine not in running state. Quiting!\n");
        return;
    }

    dev->PEv2.motionBufferEntriesAccepted = 0;
    dev->PEv2.newMotionBufferEntries = 0;

    int maxEntries = 18; // floor(56 bytes / number of axes enabled) = floor(56 / 3)

    // Fill buffer till it is empty...
    while (bufferInd < pts)
    {
        PK_PEv2_StatusGet(dev);
        printf("\rPosition: %04d  \tBuffer filling progress: %d%%  ", dev->PEv2.CurrentPosition[0], 100*bufferInd/pts);

        for (int i = dev->PEv2.newMotionBufferEntries; i < maxEntries; i++)
        {
            if (bufferInd >= pts) break; // End of buffer?

            // Buffer must contains data for all enabled axes!!
            dev->PEv2.MotionBuffer[i*3] = buffer[bufferInd++];  // Axis x
            dev->PEv2.MotionBuffer[i*3 + 1] = 0;                // Axis y
            dev->PEv2.MotionBuffer[i*3 + 2] = 0;                // Axis z

            dev->PEv2.newMotionBufferEntries = i+1;
        }

        // Send buffer to device
        PK_PEv2_BufferFill(dev);

        // Check how many entries have been accepted
        for (int i = dev->PEv2.motionBufferEntriesAccepted; i < maxEntries; i++)
        {
            memcpy(&dev->PEv2.MotionBuffer[(i - dev->PEv2.motionBufferEntriesAccepted) * 3], &dev->PEv2.MotionBuffer[i*3], 3);
        }

        dev->PEv2.newMotionBufferEntries -= dev->PEv2.motionBufferEntriesAccepted;
        dev->PEv2.motionBufferEntriesAccepted = 0;
    }

    // Wait for the buffer to empty... (max. 128 ms)
    Sleep(130);

    // Switch to STOPPED
    printf("Changing state to STOPPED...\n");
    dev->PEv2.PulseEngineStateSetup = PK_PEState_peSTOPPED;
    PK_PEv2_PulseEngineStateSet(dev);

    return;
}

// Homing test
void example3(sPoKeysDevice * dev)
{
    // Check if pulse engine is enabled...
    if (dev->PEv2.PulseEngineEnabled != 3)
    {
        printf("Run example1 first");
        return;
    }

    // Switch to STOPPED
    printf("Changing state to STOPPED...\n");
    dev->PEv2.PulseEngineStateSetup = PK_PEState_peSTOPPED;
    PK_PEv2_PulseEngineStateSet(dev);


    dev->PEv2.param1 = 0;
    PK_PEv2_AxisConfigurationGet(dev);


    printf("Configuring axis 1...\n");
    // - axis is equipped with home and limit+ switches, both limit+ and home switches have inverted polarity (NO switch)
    dev->PEv2.AxesSwitchConfig[0] = PK_ASO_SWITCH_HOME | PK_ASO_SWITCH_LIMIT_P | PK_ASO_SWITCH_INVERT_HOME | PK_ASO_SWITCH_INVERT_LIMIT_P;
    // - use dedicated home input on the PoKeysCNCaddon
    dev->PEv2.PinHomeSwitch[0] = 0;

    // - max speed: 15 kHz
    dev->PEv2.MaxSpeed[0] = 15;
    // - max acceleration: 30 kHz / s
    dev->PEv2.MaxAcceleration[0] = 0.030f;
    // - max deceleration: 60 kHz / s
    dev->PEv2.MaxDecceleration[0] = 0.060f;

    // - use 10% of max speed for homing
    dev->PEv2.HomingSpeed[0] = 10;
    // - use 50% of the homing speed for fine homing
    dev->PEv2.HomingReturnSpeed[0] = 50;

    // Save axis index to param1
    dev->PEv2.param1 = 0;
    PK_PEv2_AxisConfigurationSet(dev);



    // Start homing procedure for axis 1
    dev->PEv2.HomingStartMaskSetup = (1<<0); // Home axis 1 only (bit 0)
    PK_PEv2_HomingStart(dev);

    int cancel = 0;
    printf("Homing... Press any key to cancel\n");
    // Wait for homing to complete
    while(!kbhit() && !cancel)
    {
        PK_PEv2_StatusGet(dev);

        // Check homing status...
        switch (dev->PEv2.AxesState[0])
        {
            case 0:
                printf("\rStopped... ");
                break;

            case PK_PEAxisState_axHOME:
                // Axis is homed
                printf("\rAxis homed           \n");
                break;

            case PK_PEAxisState_axHOMINGSTART:
                // Should not see this...
                printf("\rHoming starting...");
                break;

            case PK_PEAxisState_axHOMINGSEARCH:
                printf("\rCoarse homing... ");
                break;

            case PK_PEAxisState_axHOMINGBACK:
                printf("\rFine homing...   ");
                break;

            default:
                printf("\rUnknown state encountered\n");
                cancel = 1;
                break;
        }

        switch (dev->PEv2.PulseEngineState)
        {
            case PK_PEState_peHOME:
                PK_PEv2_HomingFinish(dev);
                printf("Homing complete!\nPress any key to continue...\n");
                cancel = 1;
                break;
        }

        Sleep(100);
    }
    getch();
    printf("\n");

    // Switch to STOPPED
    printf("Changing state to STOPPED...\n");
    dev->PEv2.PulseEngineStateSetup = PK_PEState_peSTOPPED;
    PK_PEv2_PulseEngineStateSet(dev);

}

// Probing test
void example4(sPoKeysDevice * dev)
{
    // Check if pulse engine is enabled...
    if (dev->PEv2.PulseEngineEnabled != 3)
    {
        printf("Run example1 first");
        return;
    }

    // Switch to STOPPED
    printf("Changing state to STOPPED...\n");
    dev->PEv2.PulseEngineStateSetup = PK_PEState_peSTOPPED;
    PK_PEv2_PulseEngineStateSet(dev);


    // Set current position to 0
    printf("Setting axis 1 position to 0...\n");
    dev->PEv2.PositionSetup[0] = 0;
    dev->PEv2.param2 = (1<<0);
    PK_PEv2_PositionSet(dev);


    // Use pin 3 of PoKeys device for probe input
    dev->PEv2.ProbeInput = 3 + 8;
    // Use external input 7 for probe input
    //dev->PEv2.ProbeInput = 7;

    dev->PEv2.ProbeInputPolarity = 1;
    dev->PEv2.ProbeMaxPosition[0] = 1000; // max position also determines the direction of move (max position is compared to current position)
    dev->PEv2.ProbeSpeed = 0.1f;

    // Start probing procedure for axis 1
    dev->PEv2.ProbeStartMaskSetup = (1<<0); // Probe axis 1 only (bit 0)
    PK_PEv2_ProbingStart(dev);

    int cancel = 0;
    printf("Probing... Press any key to cancel\n");
    // Wait for probing to complete
    while(!kbhit() && !cancel)
    {
        PK_PEv2_StatusGet(dev);

        // Check homing status...
        switch (dev->PEv2.AxesState[0])
        {
            case 0:
                printf("\rStopped... ");
                break;

            case PK_PEAxisState_axPROBED:
                break;

            case PK_PEAxisState_axPROBESTART:
                // Should not see this...
                printf("\rProbing starting...");
                break;

            case PK_PEAxisState_axPROBESEARCH:
                printf("\rProbing (%04d) ", dev->PEv2.CurrentPosition[0]);
                break;

            default:
                printf("\rUnknown state encountered\n");
                cancel = 1;
                break;
        }

        switch (dev->PEv2.PulseEngineState)
        {
            case PK_PEState_pePROBEERROR:
                printf("\rProbing error!              \n");
                cancel = 1;
                break;

            case PK_PEState_pePROBECOMPLETE:
                PK_PEv2_ProbingFinish(dev);
                printf("\rProbing complete, position = %d\nPress any key to continue...\n", dev->PEv2.ProbePosition[0]);
                cancel = 1;
                break;
        }

        Sleep(100);
    }

    getch();
    printf("\n");

    // Switch to STOPPED
    printf("Changing state to STOPPED...\n");
    dev->PEv2.PulseEngineStateSetup = PK_PEState_peSTOPPED;
    PK_PEv2_PulseEngineStateSet(dev);
}

// PoKeysCNCaddond IO test
void example5(sPoKeysDevice * dev)
{
    // Check if IO is enabled...
    if (dev->PEv2.PulseEngineEnabled != 3 || dev->PEv2.PulseGeneratorType != (1<<7))
    {
        printf("Run example1 first");
        return;
    }

    // Read the outputs' states first
    PK_PEv2_ExternalOutputsGet(dev);

    printf("Turning all relays off...\n");
    // Turn off all relays
    dev->PEv2.ExternalRelayOutputs = 0;
    PK_PEv2_ExternalOutputsSet(dev);

    Sleep(1000);

    // Turn on the relays one by one (from center to outmost one)
    printf("Relay 1 ON\n");
    dev->PEv2.ExternalRelayOutputs |= (1<<0);
    PK_PEv2_ExternalOutputsSet(dev);
    Sleep(500);

    printf("Relay 2 ON\n");
    dev->PEv2.ExternalRelayOutputs |= (1<<1);
    PK_PEv2_ExternalOutputsSet(dev);
    Sleep(500);

    printf("Relay 3 ON\n");
    dev->PEv2.ExternalRelayOutputs |= (1<<2);
    PK_PEv2_ExternalOutputsSet(dev);
    Sleep(500);


    printf("All relays OFF...\n");
    // Turn off all relays
    dev->PEv2.ExternalRelayOutputs = 0;
    PK_PEv2_ExternalOutputsSet(dev);


    // OC outputs
    printf("Turning all OC outputs off...\n");
    dev->PEv2.ExternalOCOutputs = 0;
    PK_PEv2_ExternalOutputsSet(dev);
    Sleep(1000);

    printf("OC 1 ON\n");
    dev->PEv2.ExternalOCOutputs |= (1<<3);
    PK_PEv2_ExternalOutputsSet(dev);
    Sleep(500);

    printf("OC 2 ON\n");
    dev->PEv2.ExternalOCOutputs |= (1<<4);
    PK_PEv2_ExternalOutputsSet(dev);
    Sleep(500);

    printf("OC 3 ON\n");
    dev->PEv2.ExternalOCOutputs |= (1<<5);
    PK_PEv2_ExternalOutputsSet(dev);
    Sleep(500);

    printf("OC 4 ON\n");
    dev->PEv2.ExternalOCOutputs |= (1<<6);
    PK_PEv2_ExternalOutputsSet(dev);
    Sleep(500);

    printf("Turning all OC outputs off...\n");
    dev->PEv2.ExternalOCOutputs = 0;
    PK_PEv2_ExternalOutputsSet(dev);

    printf("Connect pin 17 of PoKeys device to pin 3 of PoKeysCNCaddon (0-10V output). Press any key to exit...\n");

    int tmp;
    // Now display the inputs status and feed PWM values...
    while(!kbhit())
    {
        PK_PEv2_StatusGet(dev);

        tmp = dev->PEv2.MiscInputStatus;
        // Display input statuses:
        printf("\rMisc inputs: "); // IN5, IN6, IN7, Spindle error input
        for (int i = 4; i < 8; i++) printf("%d", (tmp>>i)&1);

        tmp = dev->PEv2.ErrorInputStatus;
        printf(" Error: ");
        for (int i = 0; i < 8; i++) printf("%d", (tmp>>i)&1);

        Sleep(100);
    }
    getch();
    printf("\n");
}

// MPG jogging test
void example6(sPoKeysDevice * dev)
{
    // Check if pulse engine is enabled...
    if (dev->PEv2.PulseEngineEnabled != 3)
    {
        printf("Run example1 first");
        return;
    }

    PK_EncoderConfigurationGet(dev);

    // Setup encoder 1 on pins 1 and 2
    dev->Encoders[0].channelApin = 0;
    dev->Encoders[0].channelBpin = 1;
    dev->Encoders[0].encoderOptions = (1<<0) | (1<<1); // Enable encoder with 4x sampling

    PK_EncoderConfigurationSet(dev);

    // Configure the encoder 1 for axis 1 with 10x multiplier
    dev->PEv2.MPGjogEncoder[0] = 1;
    dev->PEv2.MPGjogMultiplier[0] = 10;

    dev->PEv2.param1 = 0;
    PK_PEv2_AxisConfigurationSet(dev);

    // Switch to JOGGING mode
    printf("Changing state to JOGGING...\n");
    dev->PEv2.PulseEngineStateSetup = PK_PEState_peJOGGING;
    PK_PEv2_PulseEngineStateSet(dev);

    // Check state...
    PK_PEv2_StatusGet(dev);
    if (dev->PEv2.PulseEngineState != PK_PEState_peJOGGING)
    {
        // Error...
        printf("Not in JOGGING state - check LIMIT and emergency switches!\n");
        return;
    }

    printf("Press any key to exit jogging mode!\n");

    while(!kbhit())
    {
        PK_PEv2_StatusGet(dev);
        Sleep(100);

        printf("\rPosition: %d   ",dev->PEv2.CurrentPosition[0]);
    }
    getch();
    printf("\n");

    // Switch to STOPPED
    printf("Changing state to STOPPED...\n");
    dev->PEv2.PulseEngineStateSetup = PK_PEState_peSTOPPED;
    PK_PEv2_PulseEngineStateSet(dev);
}

// Buffered motion and PWM synchronisation
void example7(sPoKeysDevice * dev)
{
    const int pts = 10*1000;
    float tmp[pts];
    uint8_t buffer[pts];
    float diff;
    int bufferInd = 0;

    // Create some motion...
    for (int t = 0; t < pts; t++)
    {
        buffer[t] = 20*(t % 6);
    }


    // Check if pulse engine is enabled...
    if (dev->PEv2.PulseEngineEnabled != 3)
    {
        printf("Run example1 first");
        return;
    }

    // Setup PWM
    dev->PWM.PWMperiod = 2500; // 10 kHz
    dev->PWM.PWMenabledChannels[2] = 1;
    PK_PWMConfigurationSet(dev);

    // Enabled synched mode...
    // Source axis: axis 1 (y)
    // Destination PWM channel: 2 (Pin 20)
    PK_PEv2_SyncedPWMSetup(dev, 1, 1, 2);


    // Configure axis 1 for buffer mode...
    printf("Changing to buffer mode...\n");
    dev->PEv2.AxesConfig[0] = PK_AC_ENABLED;
    dev->PEv2.param1 = 0;
    PK_PEv2_AxisConfigurationSet(dev);

    // Configure axis 2 for buffer mode...
    dev->PEv2.AxesConfig[1] = PK_AC_ENABLED;
    dev->PEv2.param1 = 1;
    PK_PEv2_AxisConfigurationSet(dev);


    // Clear buffer
    PK_PEv2_BufferClear(dev);

    // Switch to running mode...
    printf("Changing state to RUNNING...\n");
    // Change mode to 'RUNNING'
    dev->PEv2.PulseEngineStateSetup = PK_PEState_peRUNNING;
    PK_PEv2_PulseEngineStateSet(dev);

    printf("Checking state...\n");
    // Check the state...
    PK_PEv2_StatusGet(dev);
    if (dev->PEv2.PulseEngineState != PK_PEState_peRUNNING)
    {
        printf("Pulse engine not in running state. Quiting!\n");
        return;
    }

    dev->PEv2.motionBufferEntriesAccepted = 0;
    dev->PEv2.newMotionBufferEntries = 0;

    int maxEntries = 18; // floor(56 bytes / number of axes enabled) = floor(56 / 3)

    int tmps = 0;
    // Fill buffer till it is empty...
    while (bufferInd < pts)
    {
        PK_PEv2_StatusGet(dev);
        printf("\rPosition: %04d  \tBuffer filling progress: %d%%  ", dev->PEv2.CurrentPosition[0], 100*bufferInd/pts);

        for (int i = dev->PEv2.newMotionBufferEntries; i < maxEntries; i++)
        {
            if (bufferInd >= pts) break; // End of buffer?

            // Buffer must contains data for all enabled axes!!
            dev->PEv2.MotionBuffer[i*3] = buffer[bufferInd];  // Axis x
            dev->PEv2.MotionBuffer[i*3 + 1] = buffer[bufferInd];                // Axis y
            dev->PEv2.MotionBuffer[i*3 + 2] = 0;                // Axis z

            dev->PEv2.newMotionBufferEntries = i + 1;
            bufferInd++;

            tmps = (tmps + 1) % 2;
        }

        // Send buffer to device
        PK_PEv2_BufferFill(dev);

        // Check how many entries have been accepted
        for (int i = dev->PEv2.motionBufferEntriesAccepted; i < maxEntries; i++)
        {
            memcpy(&dev->PEv2.MotionBuffer[(i - dev->PEv2.motionBufferEntriesAccepted) * 3], &dev->PEv2.MotionBuffer[i*3], 3);
        }

        dev->PEv2.newMotionBufferEntries -= dev->PEv2.motionBufferEntriesAccepted;
        dev->PEv2.motionBufferEntriesAccepted = 0;
    }

    // Wait for the buffer to empty... (max. 128 ms)
    Sleep(130);

    // Switch to STOPPED
    printf("Changing state to STOPPED...\n");
    dev->PEv2.PulseEngineStateSetup = PK_PEState_peSTOPPED;
    PK_PEv2_PulseEngineStateSet(dev);

    return;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    while(1)
    {

        printf("-------- DEMO -------------\n");
        printf("1 - Axis setup and motion\n");
        printf("2 - Buffered motion\n");
        printf("3 - Homing test\n");
        printf("4 - Probing test\n");
        printf("5 - PoKeysCNCaddon IO test\n");
        printf("6 - MPG jogging test\n");
        printf("x - Exit\n\nPress 1-5 to continue...\n");

        //int selected = std::cin.get();

        int selected = 13;

        while (selected == 13)
        {
            selected = _getch();

            switch (selected)
            {
                case 'x':
                    return 0;
                    break;
            }
        }

        if (PK_EnumerateUSBDevices() == 0)
        {
            printf("No devices\n");
            return 0;
        }

        //sPoKeysDevice * dev = PK_ConnectToDevice(0);
        sPoKeysDevice * dev = PK_ConnectToDeviceWSerial(45000, 1000);

        if (dev != 0)
        {
            printf("Connected to device...\n");

            // Get pulse engine status
            PK_PEv2_StatusGet(dev);

            // Get axis 1 configuration...
            dev->PEv2.param1 = 0;
            PK_PEv2_AxisConfigurationGet(dev);

            switch (selected)
            {
                case '1':
                    example1(dev);
                    break;
                case '2':
                    example2(dev);
                    break;
                case '3':
                    example3(dev);
                    break;
                case '4':
                    example4(dev);
                    break;
                case '5':
                    example5(dev);
                    break;
                case '6':
                    example6(dev);
                    break;

                case '7':
                    example7(dev);
                    break;
            }

            PK_DisconnectDevice(dev);
        } else
        {
            printf("Can not connect to device...\n");
        }


        printf("Finished!\n\n\n\n");
    }

    return 0;
}
