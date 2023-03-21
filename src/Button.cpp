#include <XPLDirect.h>
#include "Button.h"

#ifndef DEBOUNCE_DELAY
#define DEBOUNCE_DELAY 20
#endif

// Buttons
Button::Button(uint8_t nExp, uint8_t pin)
{
  _nExp = nExp;
  _pin = pin;
  _state = 0;
  _transition = 0;
  _cmdPush = -1;
  pinMode(_pin, INPUT_PULLUP);
}

// use additional bit for input masking
void Button::_handle(bool input)
{
  if (DigitalIn.getBit(_nExp, _pin) && input)
  {
    if (_state == 0)
    {
      _state = DEBOUNCE_DELAY;
      _transition = transPressed;
    }
  }
  else if (_state > 0)
  {
    if (--_state == 0)
    {
      _transition = transReleased;
    }
  }
}

void Button::processCommand()
{
  if (pressed())
  {
    XP.commandStart(_cmdPush);
  }
  if (released())
  {
    XP.commandEnd(_cmdPush);
  }
}

RepeatButton::RepeatButton(uint8_t nExp, uint8_t pin, uint32_t delay) : Button(nExp, pin)
{
  _delay = delay;
  _timer = 0;
}

void RepeatButton::_handle(bool input)
{
  if (DigitalIn.getBit(_nExp, _pin) && input)
  {
    if (_state == 0)
    {
      _state = DEBOUNCE_DELAY;
      _transition = transPressed;
      _timer = millis() + _delay;
    }
    else if (_delay > 0 && (millis() >= _timer))
    {
      _state = DEBOUNCE_DELAY;
      _transition = transPressed;
      _timer += _delay;
    }
  }
  else if (_state > 0)
  {
    if (--_state == 0)
    {
      _transition = transReleased;
    }
  }
}