// #include <XPLDirect.h>
#include "Button.h"

#ifndef DEBOUNCE_DELAY
    #define DEBOUNCE_DELAY 20
#endif

 
Button::callback Button::onPush    = nullptr;
Button::callback Button::onRelease = nullptr;

void Button::setCallbacks(callback cb_onPush, callback cb_onRelease)
{ 
    onPush = cb_onPush; 
    onRelease = cb_onRelease; 
}


// Buttons
Button::Button(uint8_t nExp, uint8_t pin):
_nExp(nExp), _pin(pin), _state {0}, _transition {transNone}, _cmdPush {-1}, _autoTrigger{false}
{
    if(nExp == NOT_USED) pinMode(_pin, INPUT_PULLUP);
}

void Button::setCommand(int cmdPush)
{
    _cmdPush = cmdPush;
}

void Button::update(void)
{
    if (DigitalIn.getBit(_nExp, _pin)) {
        if (_state == 0) {
            _state      = DEBOUNCE_DELAY;
            _transition = transPressed;
            if(_autoTrigger) {
                if(onPush != nullptr) onPush(_cmdPush);
            }
        }
    } else if (_state > 0) {
        if (--_state == 0) {
            _transition = transReleased;
            if(_autoTrigger) {
                if(onRelease != nullptr) onRelease(_cmdPush);
            }
        }
    }
}

void Button::trigger(void)
{
    // Uncomment the "if" according to desired behaviour
    // if(! _autoTrigger) {
        if (pressed()) {
            if(onPush != nullptr) onPush(_cmdPush); //XPLDirect::commandStart(_cmdPush);
        } else
        if (released()) {
            if(onRelease != nullptr) onRelease(_cmdPush); //XPLDirect::commandEnd(_cmdPush);
        }
    // }
}

RepeatButton::RepeatButton(uint8_t nExp, uint8_t pin, uint32_t delay)
    : Button(nExp, pin)
{
    _delay = delay;
    _timer = 0;
}

void RepeatButton::update(void)
{
    if (DigitalIn.getBit(_nExp, _pin)) {
        if (_state == 0) {
            _state      = DEBOUNCE_DELAY;
            _transition = transPressed;
            _timer      = millis() + _delay;
        } else if (_delay > 0 && (millis() >= _timer)) {
            _state      = DEBOUNCE_DELAY;
            _transition = transPressed;
            _timer += _delay;
        }
    } else if (_state > 0) {
        if (--_state == 0) {
            _transition = transReleased;
        }
    }
}