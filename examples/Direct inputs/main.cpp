#include <Arduino.h>
#include <XPLDevices.h>

// The XPLDirect library is automatically installed by PlatformIO with XPLDevices
// Optional defines for XPLDirect can be set in platformio.ini
// This sample contains all the important defines. Modify or remove as needed

// A simple Pushbutton on Arduino pin 2
Button  btnStart(2);

// An Encoder with push functionality. 3&4 are the encoder pins, 5 the push pin.
// configured for an Encoder with 4 counts per mechanical notch, which is the standard
Encoder encHeading(3, 4, Encoder::enc4Pulse);
Button  btnHeading(5);

// A simple On/Off switch on pin 6
Switch  swStrobe(6);

// A Variable to be connected to a DataRef
long    strobe;

// Arduino setup function, called once
void    setup()
{
    // setup interface
    Serial.begin(XPLDIRECT_BAUDRATE);
    XPLDirect::begin("Sample");

    // Register Command for the Button
    SET_CMD1(btnStart, F("sim/starters/engage_starter_1"));

    // Register Commands for Encoder Up/Down/Push function.
    SET_CMD2(encHeading, F("sim/autopilot/heading_up"), F("sim/autopilot/heading_down"));
    // Std way (no macro):
    // int cmd1, cmd2;
    // cmd1 = XPLDirect::registerCommand(F("sim/autopilot/heading_up"));
    // cmd2 = XPLDirect::registerCommand(F("sim/autopilot/heading_down"));
    // encHeading.setCommand(cmd1, cmd2);

    SET_CMD1(btnHeading, F("sim/autopilot/heading_sync"));

    // Register Commands for Switch On and Off transitions. Commands are sent when Switch is moved
    SET_CMD2(swStrobe, F("sim/lights/strobe_lights_on"), F("sim/lights/strobe_lights_off"));

    // Register a DataRef for the strobe light. Read only from XP, 100ms minimum Cycle time, no divider
    XPLDirect::registerDataRef(F("sim/cockpit/electrical/strobe_lights_on"), XPL_READ, 100, 0, &strobe);
}

// Arduino loop function, called cyclic
void loop()
{
    // Handle XPlane interface
    XPLDirect::xloop();

    // handle all devices and automatically process commandsin background
    btnStart.process();
    encHeading.process();
    swStrobe.process();

    // Show the status of the Strobe on the internal LED
    digitalWrite(LED_BUILTIN, (strobe > 0));
}