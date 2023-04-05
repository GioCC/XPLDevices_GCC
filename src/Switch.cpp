#include <XPLDirect.h>
#include "Switch.h"

#ifndef DEBOUNCE_DELAY
#define DEBOUNCE_DELAY 20
#endif

Switch::Switch(uint8_t nExp, uint8_t pin)
{
  _nExp = nExp;
  _pin = pin;
  _state = switchOff;
  _cmdOn = -1;
  _cmdOff = -1;
  pinMode(_pin, INPUT_PULLUP);
}

void Switch::update()
{
  if (_debounce > 0)
  {
    _debounce--;
  }
  else 
  {
    SwState_t input = (DigitalIn.getBit(_nExp, _pin) ? switchOn : switchOff);
    if (input != _state)
    {
      _debounce = DEBOUNCE_DELAY;
      _state = input;
      _transition = true;
    }
  }
}

void Switch::trigger()
{
  if (_transition)
  {
    int cmd = getCommand();
    if (cmd >= 0)
    {
      XPLDirect::commandTrigger(getCommand());
    }
    _transition = false;
  }
}

// Switch 2

Switch2::Switch2(uint8_t nExp, uint8_t pin1, uint8_t pin2)
{
  _nExp = nExp;
  _pin1 = pin1;
  _pin2 = pin2;
  _state = switchOff;
  _cmdOff = -1;
  _cmdOn1 = -1;
  _cmdOn2 = -1;
  if (_nExp == NOT_USED)
  {
    pinMode(_pin1, INPUT_PULLUP);
    pinMode(_pin2, INPUT_PULLUP);
  }
}

void Switch2::update()
{
  if (_debounce > 0)
  {
    _debounce--;
  }
  else
  {
    SwState_t input = switchOff;
    if (DigitalIn.getBit(_nExp, _pin1))
    {
      input = switchOn1;
    }
    else if (DigitalIn.getBit(_nExp, _pin2))
    {
      input = switchOn2;
    }
    if (input != _state)
    {
      _debounce = DEBOUNCE_DELAY;
      _lastState = _state;
      _state = input;
      _transition = true;
    }
  }
}

int Switch2::getCommand()
{
  int res = -1;
  // One-pos switch
  if (_cmdOn2 == -1)
  {
    if (_state == switchOn1)
    {
      res = _cmdOn1;
    } else
    if (_state == switchOff && _lastState == switchOn1)
    {
      res = _cmdOff;
    } else
    if (_state == switchOn2)
    {
      res = _cmdOff;
    } else
    if (_state == switchOff && _lastState == switchOn2)
    {
      res = _cmdOn1;
    }
  }
  else
  // Two-pos switch
  {
    if (_state == switchOn1)
    {
      res = _cmdOn1;
    } else
    if (_state == switchOff)
    {
      res = _cmdOff;
    } else
    if (_state == switchOn2)
    {
      res = _cmdOn2;
    }
  }
  return res;
}

void Switch2::trigger()
{
  if (_transition)
  {
    XPLDirect::commandTrigger(getCommand());
    _transition = false;
  }
}
