#ifndef XPLDevices_h
#define XPLDevices_h

// Comment unused devices to spare memory
#define USE_BUTTONS
#define USE_ENCODERS
#define USE_SWITCHES
#define USE_SHIFTOUTS
#define USE_LEDSHIFTS
#define USE_ANALOGINS

#include <Timer.h>
#include <DigitalIn.h>
#include <XPLDirect_ns.h>

#ifdef USE_BUTTONS
    #include <Button.h>
#endif
#ifdef USE_ENCODERS
    #include <Encoder.h>
#endif
#ifdef USE_SWITCHES
    #include <Switch.h>
#endif
#ifdef USE_SHIFTOUTS
    #include <ShiftOut.h>
#endif
#ifdef USE_LEDSHIFTS
    #include <LedShift.h>
#endif
#ifdef USE_ANALOGINS
    #include <AnalogIn.h>
#endif

#define SET_CMD1(dev, cmd)          do { dev.setCommand(XPLDirect::registerCommand(cmd)); } while(0);
#define SET_CMD2(dev, cmd1, cmd2)   do { dev.setCommand(XPLDirect::registerCommand(cmd1), XPLDirect::registerCommand(cmd2)); } while(0);
#define SET_CMD3(dev, cmd1, cmd2, cmd3) \
   do { dev.setCommand(XPLDirect::registerCommand(cmd1), \
                       XPLDirect::registerCommand(cmd2), \
                       XPLDirect::registerCommand(cmd3) ); \
    } while(0);


namespace XPLDevices
{
    void Init(void)
    {

        XPLDirect::XPLDirect(&Serial);

#ifdef USE_BUTTONS
        Button::setCallbacks(XPLDirect::commandStart, XPLDirect::commandEnd);
#endif

#ifdef USE_ENCODERS
        Encoder::setCallbacks(XPLDirect::commandTrigger, XPLDirect::commandTrigger
                              // , XPLDirect::commandTrigger, XPLDirect::commandTrigger,
        );
#endif

#ifdef USE_SWITCHES
        Switch::setCallbacks(XPLDirect::commandTrigger);
        Switch2::setCallbacks(XPLDirect::commandTrigger);
#endif

#ifdef USE_SHIFTOUTS
#endif

#ifdef USE_LEDSHIFTS
#endif

#ifdef USE_ANALOGINS
#endif
    }

}

#endif
