#ifndef Encoder_h
#define Encoder_h
#include <Arduino.h>
#include <DigitalIn.h>

//! TODO: Encoder on MUX

/// @brief Class for simple rotary encoders.
/// Without button (handled separately). 
/// The number of counts per mechanical notch can be
/// configured for the triggering of up/down events.
class Encoder
{
public:

    enum EncCmd_t {
        encCmdUp,
        encCmdDown,
        encCmdFastUp,
        encCmdFastDown,
        // encCmdPush
    };

    enum EncPulse_t {
        enc1Pulse = 1,
        enc2Pulse = 2,
        enc4Pulse = 4
    };

    using callback = int(*)(int);

    /// @brief Constructor. Sets connected pins and number of counts per notch.
    /// @param nExp expander number (from initialization order)
    /// @param pin1 pin for Encoder A track
    /// @param pin2 pin for Encoder B track
    /// @param pulses Number of counts per mechanical notch
    Encoder(uint8_t nExp, uint8_t pin1, uint8_t pin2, EncPulse_t pulses = enc1Pulse);

    /// @brief Constructor. Sets connected pins and number of counts per notch.
    /// @param pin1 pin for Encoder A track
    /// @param pin2 pin for Encoder B track
    /// @param pulses Number of counts per mechanical notch
    Encoder(uint8_t pin1, uint8_t pin2, EncPulse_t pulses = enc1Pulse)
        : Encoder(NOT_USED, pin1, pin2, pulses) {}

    /// @brief Set callback functions for encoder events.
    ///        Callback functions have prototype: int(*)(int) 
    /// @param cb_onUp   Callback for "up" event
    /// @param cb_onDown Callback for "down" event
    // /// @param cb_onFastUp   Callback for fast "up" event
    // /// @param cb_onFastDown Callback for fast "down" event
    static 
    void setCallbacks(callback cb_onUp, callback cb_onDown
        // , callback cb_onFastUp, callback cb_onFastDown
    );

    /// @brief Set auto trigger behaviour (default is off).
    /// @param onOff True to activate
    void setAutoTrigger(bool onOff) { _autoTrigger = onOff;  };

    /// @brief Handle realtime. Read input and evaluate any transitions.
    void update();

    /// @brief Handle realtime and process XPLDirect commands.
    void process() { update(); trigger(); };

    /// @brief Read current Encoder count.
    /// @return Remaining Encoder count.
    int16_t pos() { return _count; };

    /// @brief Evaluate Encoder up one notch (positive turn)
    /// @param reset  Consume ONE event if true, otherwise just report
    /// @return true: up event available and transition reset.
    bool    up(bool reset = true) { return _count >= _pulses ? ((reset ? _count -= _pulses : 0), true) : false; };

    /// @brief Evaluate Encoder up down notch (negative turn)
    /// @param reset  Consume ONE event if true, otherwise just report
    /// @return true: up event available and transition reset.
    bool    down(bool reset = true) { return _count <= -_pulses ? ((reset ? _count += _pulses : 0), true) : false; };

    /// @brief Set XPLDirect commands for Encoder events without push function
    /// @param cmdUp Command handle for positive turn as returned by XPLDirect::registerCommand()
    /// @param cmdDown Command handle for negative turn as returned by XPLDirect::registerCommand()
    void    setCommand(int cmdUp, int cmdDown);

    // /// @brief Set XPLDirect commands for Encoder events
    // /// @param cmdNameUp Command for positive turn
    // /// @param cmdNameDown Command for negative turn
    // void    setCommand(XPString_t *cmdNameUp, XPString_t *cmdNameDown);

    /// @brief Get XPLDirect command assiciated with the selected event
    /// @param cmd Event to read out (encCmdUp, encCmdDown, encCmdPush)
    /// @return Handle of the command, -1 = no command
    int     getCommand(EncCmd_t cmd);

    /// @brief Check for Encoder events and process XPLDirect commands as appropriate
    ///        Beware: one call of trigger() only reports one encoder "click" event.
    ///        If there are more, trigger() must be called repeatedly.
    /// @return true if an events was found.
    bool    trigger(void);

private:
    static callback onUp;
    static callback onDown;
    // static callback onFastUp;
    // static callback onFastDown;

    uint8_t _nExp;
    uint8_t _pin1, _pin2;
    uint8_t _pulses;
    int8_t  _count;
    uint8_t _state;
    bool    _autoTrigger;
    int     _cmdUp;
    int     _cmdDown;
};

#endif
