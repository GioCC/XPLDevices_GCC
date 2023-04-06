#ifndef Button_h
#define Button_h
#include <Arduino.h>
#include <DigitalIn.h>

/// @brief Class for a simple pushbutton with debouncing and XPLDirect command handling.
/// Supports start and end of commands so XPlane can show the current Button status.
class Button
{
public:

    using callback = int(*)(int);

    /// @brief Constructor, set Expander and Channel number
    /// @param nExp Expander number (from DigitalIn initialization order)
    /// @param nChannel Channel on the IO expander (0-15)
    Button(uint8_t nExp, uint8_t nChannel);

    /// @brief Constructor, set digital input without Expander
    /// @param pin Arduino pin number
    explicit Button(uint8_t pin)
        : Button(NOT_USED, pin){};

    /// @brief Set callback functions for button events.
    ///        Callback functions have prototype: int(*)(int) 
    /// @param cb_onPush Callback for "push" event
    /// @param cb_onRelease Callback for "release" event
    static 
    void setCallbacks(callback cb_onPush, callback cb_onRelease);

    /// @brief Handle realtime. Read input and evaluate any transitions.
    //virtual 
    void update(void);

    /// @brief Handle realtime and process XPLDirect commands
    //virtual 
    void process(void) { update(); trigger(); };

    /// @brief Evaluate and reset transition if button pressed down
    /// @return true: Button was pressed. Transition detected.
    bool pressed(void) { return _transition == transPressed ? (_transition = transNone, true) : false; };

    /// @brief Evaluate and reset transition if button released
    /// @return true: Button was released. Transition detected.
    bool released(void) { return _transition == transReleased ? (_transition = transNone, true) : false; };

    /// @brief Evaluate status of Button
    /// @return true: Button is currently held down
    bool engaged(void) { return _state > 0; };

    /// @brief Set XPLDirect command for Button events
    /// @param cmdPush Command handle as returned by XPLDirect::registerCommand()
    void setCommand(int cmdPush);

    // /// @brief Set XPLDirect command for Button events
    // /// @param cmdNamePush Command name to register
    // void setCommand(XPString_t cmdNamePush);

    /// @brief Get XPLDirect command associated with Button
    /// @return Handle of the command
    int  getCommand(void) { return _cmdPush; };

    /// @brief Process all transitions and active transitions to XPLDirect
    void trigger(void);

protected:

    enum {
        transNone,
        transPressed,
        transReleased
    };

    static callback onPush;
    static callback onRelease;

    // static void (*onPush)(uint8_t cmdHandle);
    // static void (*onRelease)(uint8_t cmdHandle);

    uint8_t _nExp;
    uint8_t _pin;
    uint8_t _state;
    uint8_t _transition;
    int     _cmdPush;
};

/// @brief Class for a simple pushbutton with debouncing and XPLDirect command handling,
/// supports start and end of commands so XPlane can show the current Button status.
/// When button is held down cyclic new pressed events are generated for auto repeat function.
class RepeatButton : public Button
{
public:
    /// @brief Constructor, set Expander and Channel number
    /// @param nExp Expander number (from initialization order)
    /// @param nChannel Channel on the IO expander (0-15)
    /// @param delay Cyclic delay for repeat function
    RepeatButton(uint8_t nExp, uint8_t nChannel, uint32_t delay);

    /// @brief Constructor, set digital input without Expander
    /// @param pin Arduino pin number
    /// @param delay Cyclic delay for repeat function
    RepeatButton(uint8_t pin, uint32_t delay)
        : RepeatButton(NOT_USED, pin, delay){};

    /// @brief Handle realtime. Read input and evaluate any transitions.
    // virtual 
    void update(void); // override;

    /// @brief Handle realtime and process XPLDirect commands
    // virtual 
    void process() //override
        { update(); trigger(); };

protected:
    uint32_t _delay;
    uint32_t _timer;
};

#endif
