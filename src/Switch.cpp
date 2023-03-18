#include <Arduino.h>
#include <XPLDirect.h>
#include "Switch.h"

#ifndef DEBOUNCE_DELAY
#define DEBOUNCE_DELAY 20
#endif

Switch::Switch(uint8_t mux, uint8_t pin)
{
  _mux = mux;
  _pin = pin;
  _state = switchOff;
  _cmdOff = -1;
  _cmdOn = -1;
  pinMode(_pin, INPUT_PULLUP);
}

bool Switch::handle()
{
  if (_debounce > 0)
  {
    _debounce--;
  }
  else 
  {
    SwState_t input = (DigitalIn.getBit(_mux, _pin) ? switchOn : switchOff);
    if (input != _state)
    {
      _debounce = DEBOUNCE_DELAY;
      _state = input;
      _transition = true;
      return true;
    }
  }
  return false;
}

void Switch::processCommand()
{
  if (_transition)
  {
    XP.commandTrigger(getCommand());
    _transition = false;
  }
}

Switch2::Switch2(uint8_t mux, uint8_t pin1, uint8_t pin2) : Switch(mux, pin1)
{
  _pin2 = pin2;
  _cmdOn2 = -1;
  if (_mux == NOT_USED)
  {
    pinMode(_pin2, INPUT_PULLUP);
  }
}

bool Switch2::handle()
{
  if (_debounce > 0)
  {
    _debounce--;
  }
  else
  {
    SwState_t input;
    input = (DigitalIn.getBit2(_mux, _pin2) ? switchOn2 : switchOff);
    input = (DigitalIn.getBit(_mux, _pin)   ? switchOn  : input);
    
    if (input != _state)
    {
      _debounce = DEBOUNCE_DELAY;
      _lastState = _state;
      _state = input;
      _transition = true;
      return true;
    }
  }
  return false;
}

int Switch2::getCommand()
{
  int res = -1;
  if (_state == switchOn)
  {
    res = _cmdOn;
  } 
  else if (_state == switchOff && _lastState == switchOn)
  {
    res = _cmdOff;
  }
  else if (_state == switchOff && _lastState == switchOn2)
  {
    res = _cmdOff2;
  }
  else if (_state == switchOn2)
  {
    res = _cmdOn2;
  }
  return res;
}
