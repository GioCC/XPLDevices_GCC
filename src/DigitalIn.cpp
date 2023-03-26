#include "DigitalIn.h"


#ifdef ARDUINO_ARCH_AVR
// Helper function for quick pin out setting (AVR)
// Used for multiplexer selector for faster access
inline void directOut(uint8_t port, uint8_t pinMsk, uint8_t val)
{
    volatile uint8_t *out = portOutputRegister(port);
    uint8_t oldSREG = SREG;
    cli();
    (val == LOW ? (*out &= ~pinMsk) : (*out |= pinMsk));
    SREG = oldSREG;
}

inline void directOut(uint8_t pin, uint8_t val)
{
    volatile uint8_t *out = portOutputRegister(digitalPinToPort(pin));
    uint8_t mask = digitalPinToBitMask(pin);
    uint8_t oldSREG = SREG;
    cli();
    (val == LOW ? (*out &= ~mask) : (*out |= mask));
    SREG = oldSREG;
}

inline bool directIn(uint8_t pin)
{
    volatile uint8_t *inReg = portInputRegister(digitalPinToPort(pin));
    uint8_t mask = digitalPinToBitMask(pin);
    return ((*inReg & mask) == 0);     // Positive logic, will be negated later
}

#else
inline void directOut(uint8_t pin, uint8_t val) { digitalWrite(pin, val); } 
inline bool directIn(uint8_t pin) { digitalRead(pin) }
#endif



// constructor
DigitalIn_::DigitalIn_()
{
  _nExpanders = 0;
  for (uint8_t nExp = 0; nExp < MUX_MAX_NUMBER; nExp++)
  {
    _pin[nExp] = NOT_USED;
  }
  _s0 = _s1 = _s2 = _s3 = NOT_USED;
  #ifdef ARDUINO_ARCH_AVR
  _s0port = _s1port = _s2port = _s3port = NOT_USED;
  _s0mask = _s1mask = _s2mask = _s3mask = 0;
  #endif
}

// configure 74HC4067 adress pins S0-S3
void DigitalIn_::setMux(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3)
{
  _s0 = s0;
  _s1 = s1;
  _s2 = s2;
  _s3 = s3;
  pinMode(_s0, OUTPUT);
  pinMode(_s1, OUTPUT);
  pinMode(_s2, OUTPUT);
  pinMode(_s3, OUTPUT);

  #ifdef ARDUINO_ARCH_AVR
  _s0port = digitalPinToPort(_s0);
  _s1port = digitalPinToPort(_s1);
  _s2port = digitalPinToPort(_s2);
  _s3port = digitalPinToPort(_s3);
  _s0mask = digitalPinToBitMask(_s0);
  _s1mask = digitalPinToBitMask(_s1);
  _s2mask = digitalPinToBitMask(_s2);
  _s3mask = digitalPinToBitMask(_s3);
  #endif
}

void DigitalIn_::setMuxChannel(uint8_t ch)
{
  if(_s0 == NOT_USED) return;     // raw check, but still something
#ifdef ARDUINO_ARCH_AVR
  // could use the same directOut() calls without the #ifdef, but here we can factorize the operations
  uint8_t oldSREG = SREG;
  volatile uint8_t* preg;
  cli();
  preg = portOutputRegister(_s3port); (ch & 0x08) ? (*preg |= _s3mask) : (*preg &= ~_s3mask);
  preg = portOutputRegister(_s2port); (ch & 0x04) ? (*preg |= _s2mask) : (*preg &= ~_s2mask);
  preg = portOutputRegister(_s3port); (ch & 0x02) ? (*preg |= _s1mask) : (*preg &= ~_s1mask);
  preg = portOutputRegister(_s0port); (ch & 0x01) ? (*preg |= _s0mask) : (*preg &= ~_s0mask);
  SREG = oldSREG;
  delayMicroseconds(1);     // Allow signals to settle
#else
  directOut(_s3, (ch & 0x08));
  directOut(_s2, (ch & 0x04));
  directOut(_s1, (ch & 0x02));
  directOut(_s0, (ch & 0x01));
#endif
}

// Add a 74HC4067
bool DigitalIn_::addMux(uint8_t pin)
{
  if (_nExpanders >= MUX_MAX_NUMBER)
  {
    return false;
  }
  _pin[_nExpanders++] = pin;
  pinMode(pin, INPUT);
  return true;
}

#if MCP_MAX_NUMBER > 0
// Add a MCP23017
bool DigitalIn_::addMCP(uint8_t adress)
{
  if (_nExpanders >= MCP_MAX_NUMBER)
  {
    return false;
  }
  if (!_mcp[_nExpanders].begin_I2C(adress, &Wire))
  {
    return false;
  }
  for (int i = 0; i < 16; i++)
  {
    // TODO: register write iodir = 0xffff, ipol = 0xffff, gppu = 0xffff
    _mcp[_nExpanders].pinMode(i, INPUT_PULLUP);
  }
  _pin[_nExpanders++] = MCP_PIN;
  return true;
}
#endif

// Gets specific input from expander channel; expander number according to initialization order 
bool DigitalIn_::getBit(uint8_t expander, uint8_t channel, bool direct)
{
  bool res;
  
  // Unintuitive evaluation order favors multiplexers, 
  // which also have to perform the selector setting
  if (expander != NOT_USED)
  {
    if(direct && !isMCP(expander)) {
      setMuxChannel(channel);
      res = !directIn(_pin[expander]);
    } else {
      res = bitRead(_data[expander], channel);
    }
  } else {
      res = !directIn(channel);
  }
  return res;
}

// read all inputs together -> base for board specific optimization by using byte read
void DigitalIn_::handle()
{
  // only if Mux Pins present
#if MCP_MAX_NUMBER > 0  
  if (_nExpanders > _numMCP)
#else
  if (_nExpanders > 0)
#endif
  {
    for (uint8_t channel = 0; channel < 16; channel++)
    {
      setMuxChannel(channel);
      for (uint8_t expander = 0; expander < _nExpanders; expander++)
      {
        if (_pin[expander] == MCP_PIN) continue;
        bitWrite(_data[expander], channel, !directIn(_pin[expander]));
      }
    }
  }
#if MCP_MAX_NUMBER > 0
  int mcp = 0;
  for (uint8_t expander = 0; expander < _nExpanders; expander++)
  {
    if (_pin[expander] != MCP_PIN) continue;
    _data[expander] = ~_mcp[mcp++].readGPIOAB();
  }
#endif
}

DigitalIn_ DigitalIn;
