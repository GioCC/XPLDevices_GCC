#ifndef Switch_h
#define Switch_h
#include <Arduino.h>
#include <DigitalIn.h>

/// @brief Class for a simple on/off switch with debouncing
class Switch
{
public:
    using callback = int(*)(int);

    /// @brief Constructor. Connect the switch to an expander channel (can also be used for a direct pin).
    /// @param nExp expander number (from DigitalIn initialization order)
    /// @param nChannel channel on the expander (0-15)
    Switch(uint8_t nExp, uint8_t nChannel);

    /// @brief Constructor, Connect the switch to a direct pin
    /// @param pin Arduino pin number
    Switch(uint8_t pin)
        : Switch(NOT_USED, pin){};

    /// @brief Set callback functions for button events.
    ///        Callback functions have prototype: int(*)(int) 
    /// @param cb_onChange Callback for "change" event
    static 
    void setCallbacks(callback cb_onChange);


    /// @brief Handle realtime. Read input and evaluate any transitions.
    void update();

    /// @brief Handle realtime and process XPLDirect commands
    void process() { update(); trigger(); };

    /// @brief Check whether Switch set to on
    /// @return true: Switch is on
    bool isOn() { return (_state == switchOn); };

    /// @brief Check whether Switch set to off
    /// @return true: Switch is off
    bool isOff() { return (_state == switchOff); };

    /// @brief Set XPLDirect commands for Switch events (command only for on position)
    /// @param cmdOn Command handle for Switch moved to on as returned by XPLDirect::registerCommand()
    void setCommand(int cmdOn) { _cmdOn  = cmdOn; _cmdOff = -1;
    }

    // /// @brief Set XPLDirect commands for Switch events (command only for on position)
    // /// @param cmdNameOn Command for Switch moved to on
    // void setCommand(XPString_t *cmdNameOn)
    // {
    //     _cmdOn  = XPLDirect::registerCommand(cmdNameOn);
    //     _cmdOff = -1;
    // }

    /// @brief Set XPLDirect commands for Switch events
    /// @param cmdOn Command handle for Switch moved to on as returned by XPLDirect::registerCommand()
    /// @param cmdOff Command handle for Switch moved to off as returned by XPLDirect::registerCommand()
    void setCommand(int cmdOn, int cmdOff) { _cmdOn  = cmdOn; _cmdOff = cmdOff; };

    // /// @brief Set XPLDirect commands for Switch events
    // /// @param cmdNameOn Command for Switch moved to on
    // /// @param cmdNameOff Command for Switch moved to off
    // void setCommand(XPString_t *cmdNameOn, XPString_t *cmdNameOff)
    // {
    //     _cmdOn  = XPLDirect::registerCommand(cmdNameOn);
    //     _cmdOff = XPLDirect::registerCommand(cmdNameOff);
    // }

    /// @brief Get XPLDirect command for last transition of Switch
    /// @return Handle of the last command
    int   getCommand() { return (_state == switchOn ? _cmdOn : _cmdOff); };

    /// @brief Process all transitions to XPLDirect
    void  trigger();

    /// @brief Check Status of Switch and translate to float value
    /// @param onValue Value to return when Switch is set to on
    /// @param offValue Value to return when Switch is set to off
    /// @return Returned value
    float value(float onValue, float offValue) { return isOn() ? onValue : offValue; };

private:
    enum SwState_t {
        switchOff,
        switchOn
    };

    static callback onChange;

    uint8_t _nExp;
    uint8_t _pin;
    uint8_t _debounce;
    uint8_t _state;
    bool    _transition;
    int     _cmdOff;
    int     _cmdOn;
};

/// @brief Class for an on/off/on switch with debouncing and XPLDirect command handling.
class Switch2
{
public:
    using callback = int(*)(int);

    /// @brief Constructor. Connect the switch to two expander channels.
    /// @param nExp expander number (from DigitalIn initialization order)
    /// @param nChannel1 channel on the expander (0-15) for "on1" switch terminal
    /// @param nChannel2 channel on the expander (0-15) for "on2" switch terminal
    Switch2(uint8_t nExp, uint8_t nChannel1, uint8_t nChannel2);

    /// @brief Constructor, Connect the switch to two direct pins
    /// @param pin1 on1 Arduino pin number
    /// @param pin2 on2 Arduino pin number
    Switch2(uint8_t pin1, uint8_t pin2)
        : Switch2(NOT_USED, pin1, pin2) {}

    /// @brief Set callback functions for button events.
    ///        Callback functions have prototype: int(*)(int) 
    /// @param cb_onChange Callback for "change" event
    static 
    void setCallbacks(callback cb_onChange);

    /// @brief Handle realtime. Read inputs and evaluate any transitions.
    void update();

    /// @brief Handle realtime and process XPLDirect commands
    void process()
    {
        update();
        trigger();
    };

    /// @brief Check whether Switch set to off
    /// @return true: Switch is off
    bool isOff() { return _state == switchOff; };

    /// @brief Check whether Switch set to on1
    /// @return true: Switch is on1
    bool isOn1() { return _state == switchOn1; };

    /// @brief Check whether Switch set to on2
    /// @return true: Switch is on2
    bool isOn2() { return _state == switchOn2; };

    /// @brief Set XPLDirect commands for Switch events in cases only up/down commands are to be used
    /// @param cmdUp Command handle for Switch moved from on1 to off or from off to on2 on as returned by XPLDirect::registerCommand()
    /// @param cmdDown Command handle for Switch moved from on2 to off or from off to on1 on as returned by XPLDirect::registerCommand()
    void setCommand(int cmdUp, int cmdDown)
    {
        _cmdOn1 = cmdUp;
        _cmdOff = cmdDown;
        _cmdOn2 = -1;
    };

    /// @brief Set XPLDirect commands for Switch events in cases only up/down commands are to be used
    /// @param cmdNameUp Command for Switch moved from on1 to off or from off to on2 on
    /// @param cmdNameDown Command for Switch moved from on2 to off or from off to on1
    void setCommand(XPString_t *cmdNameUp, XPString_t *cmdNameDown)
    {
        _cmdOn1 = XPLDirect::registerCommand(cmdNameUp);
        _cmdOff = XPLDirect::registerCommand(cmdNameDown);
        _cmdOn2 = -1;
    }

    /// @brief Set XPLDirect commands for Switch events in cases separate events for on1/off/on2 are to be used
    /// @param cmdOn1 Command handle for Switch moved to on1 position as returned by XPLDirect::registerCommand()
    /// @param cmdOff Command handle for Switch moved to off position as returned by XPLDirect::registerCommand()
    /// @param cmdOn2 Command handle for Switch moved to on2 position as returned by XPLDirect::registerCommand()
    void setCommand(int cmdOn1, int cmdOff, int cmdOn2)
    {
        _cmdOn1 = cmdOn1;
        _cmdOff = cmdOff;
        _cmdOn2 = cmdOn2;
    }

    /// @brief Set XPLDirect commands for Switch events in cases separate events for on1/off/on2 are to be used
    /// @param cmdNameOn1 Command for Switch moved to on1 position
    /// @param cmdNameOff Command for Switch moved to off position
    /// @param cmdNameOn2 Command for Switch moved to on2 position
    void setCommand(XPString_t *cmdNameOn1, XPString_t *cmdNameOff, XPString_t *cmdNameOn2)
    {
        _cmdOn1 = XPLDirect::registerCommand(cmdNameOn1);
        _cmdOff = XPLDirect::registerCommand(cmdNameOff);
        _cmdOn2 = XPLDirect::registerCommand(cmdNameOn2);
    }

    /// @brief Get XPLDirect command for last transition of Switch
    /// @return Handle of the last command
    int   getCommand();

    /// @brief Process all transitions to XPLDirect
    void  trigger();

    /// @brief Check Status of Switch and translate to float value
    /// @param on1Value Value to return when Switch is set to on1
    /// @param offValue Value to return when Switch is set to off
    /// @param on2Value Value to return when Switch is set to on2
    /// @return Returned value
    float value(float on1Value, float offValue, float on2value) { return (isOn1() ? on1Value : isOn2() ? on2value
                                                                                                       : offValue); };

private:
    enum SwState_t {
        switchOff,
        switchOn1,
        switchOn2
    };

    static callback onChange;

    uint8_t _nExp;
    uint8_t _pin1;
    uint8_t _pin2;
    uint8_t _lastState;
    uint8_t _debounce;
    uint8_t _state;
    bool    _transition;
    int     _cmdOff;
    int     _cmdOn1;
    int     _cmdOn2;
};

#endif
