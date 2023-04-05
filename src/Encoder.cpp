#include <XPLDirect.h>
#include "Encoder.h"

#ifndef DEBOUNCE_DELAY
    #define DEBOUNCE_DELAY 20
#endif

Encoder::callback Encoder::onUp    = nullptr;
Encoder::callback Encoder::onDown  = nullptr;
// Encoder::callback Encoder::onFastUp    = nullptr;
// Encoder::callback Encoder::onFastDown  = nullptr;

void Encoder::setCallbacks(callback cb_onUp, callback cb_onDown
    // , callback cb_onFastUp, callback cb_onFastDown
)
{ 
    onUp = cb_onUp; 
    onDown = cb_onDown; 
    // onFastUp = cb_onFastUp; 
    // onFastDown = cb_onFastDown;
}


// Encoder with button functionality on MUX
// Encoder::Encoder(uint8_t nExp, uint8_t pin1, uint8_t pin2, uint8_t pin3, EncPulse_t pulses)
Encoder::Encoder(uint8_t nExp, uint8_t pin1, uint8_t pin2, EncPulse_t pulses)
{
    _nExp       = nExp;
    _pin1       = pin1;
    _pin2       = pin2;
    // _pin3       = pin3;
    _pulses     = pulses;
    _count      = 0;
    _state      = 0;
    // _transition = transNone;
    _cmdUp      = -1;
    _cmdDown    = -1;
    // _cmdPush    = -1;
    pinMode(_pin1, INPUT_PULLUP);
    pinMode(_pin2, INPUT_PULLUP);
    // if (_pin3 != NOT_USED) {
    //     pinMode(_pin3, INPUT_PULLUP);
    // }
}

// real time handling
void Encoder::update()
{
    uint8_t newState = 0;
    // collect new state
    // If MCU direct pins, getBit() performs pin read, not cache read
    // For _nExp values corresponding to I/O expanders: resort to cache read
    // For actual multiplexers: avoid cache read, access multiplexer now
    newState |= (DigitalIn.getBit(_nExp, _pin2, true) ? 0x02 : 0x00);
    newState |= (DigitalIn.getBit(_nExp, _pin1, true) ? 0x01 : 0x00);
    _state = ((_state & 0x03) << 2) | newState;

    // evaluate state change
    if (_state == 1 || _state == 7 || _state == 8 || _state == 14) {
        _count++;
    } else if (_state == 2 || _state == 4 || _state == 11 || _state == 13) {
        _count--;
    } else if (_state == 3 || _state == 12) {
        _count += 2;
    } else if (_state == 6 || _state == 9) {
        _count -= 2;
    }

    // optional button functionality
    // if (_pin3 != NOT_USED) {
    //     if (DigitalIn.getBit(_nExp, _pin3)) {
    //         if (_debounce == 0) {
    //             _debounce   = DEBOUNCE_DELAY;
    //             _transition = transPressed;
    //         }
    //     } else if (_debounce > 0) {
    //         if (--_debounce == 0) {
    //             _transition = transReleased;
    //         }
    //     }
    // }
}

// void Encoder::setCommand(int cmdUp, int cmdDown) //, int cmdPush)
// {
//     _cmdUp   = cmdUp;
//     _cmdDown = cmdDown;
//     // _cmdPush = cmdPush;
// }

// void Encoder::setCommand(XPString_t *cmdNameUp, XPString_t *cmdNameDown, XPString_t *cmdNamePush)
// {
//     _cmdUp   = XPLDirect::registerCommand(cmdNameUp);
//     _cmdDown = XPLDirect::registerCommand(cmdNameDown);
//     // _cmdPush = XPLDirect::registerCommand(cmdNamePush);
// }

void Encoder::setCommand(int cmdUp, int cmdDown)
{
    _cmdUp   = cmdUp;
    _cmdDown = cmdDown;
    // _cmdPush = -1;
}

// void Encoder::setCommand(XPString_t *cmdNameUp, XPString_t *cmdNameDown)
// {
//     _cmdUp   = XPLDirect::registerCommand(cmdNameUp);
//     _cmdDown = XPLDirect::registerCommand(cmdNameDown);
//     // _cmdPush = -1;
// }

int Encoder::getCommand(EncCmd_t cmd)
{
    if (cmd == encCmdUp) { return _cmdUp; }
    else 
    if (cmd == encCmdDown) { return _cmdDown; }
    else 
    // if (cmd == encCmdPush) { return _cmdPush; }
    // else 
    return -1;
}

void Encoder::trigger()
{
    if (up()) {
        if(onUp != nullptr) onUp(_cmdUp); // XPLDirect::commandTrigger(_cmdUp);
    } else
    if (down()) {
        if(onDown != nullptr) onDown(_cmdDown); // XPLDirect::commandTrigger(_cmdDown);
    }
    // if (_cmdPush >= 0) {
    //     if (pressed()) {
    //         XPLDirect::commandStart(_cmdPush);
    //     }
    //     if (released()) {
    //         XPLDirect::commandEnd(_cmdPush);
    //     }
    // }
}
