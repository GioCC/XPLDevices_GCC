#include "DigitalIn.h"

#ifdef ARDUINO_ARCH_AVR
// Helper function for quick pin out setting (AVR)
// Used for multiplexer selector for faster access
void directOut(uint8_t port, uint8_t pinMsk, uint8_t val)
{
    volatile uint8_t *out = portOutputRegister(port);
    uint8_t oldSREG = SREG;
    cli();
    (val == LOW ? (*out &= ~pinMsk) : (*out |= pinMsk));
    SREG = oldSREG;
}
#endif


// constructor
DigitalIn_::DigitalIn_()
{
  _nExpanders = 0;
  for (uint8_t mux = 0; mux < MUX_MAX_NUMBER; mux++)
  {
    _pin[mux] = NOT_USED;
  }
  _s0 = _s1 = _s2 = _s3 = NOT_USED;
  #ifdef ARDUINO_ARCH_AVR
  _S0port = _S1port = _S2port = _S3port = NOT_USED;
  _S0mask = _S1mask = _S2mask = _S3mask = 0;
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
  _S0port = digitalPinToPort(_s0);
  _S1port = digitalPinToPort(_s1);
  _S2port = digitalPinToPort(_s2);
  _S3port = digitalPinToPort(_s3);
  _S0mask = digitalPinToBitMask(_s0);
  _S1mask = digitalPinToBitMask(_s1);
  _S2mask = digitalPinToBitMask(_s2);
  _S3mask = digitalPinToBitMask(_s3);
  #endif
}

void DigitalIn_::setMuxChannel(uint8_t ch)
{
  if(_s0 == NOT_USED) return;     // raw check, but still something
  directOut(_S3port, _S3mask, (ch & 0x08));
  directOut(_S2port, _S2mask, (ch & 0x04));
  directOut(_S1port, _S1mask, (ch & 0x02));
  directOut(_S0port, _S0mask, (ch & 0x01));
#ifdef ARDUINO_ARCH_AVR
#else
  digitalWrite(_s3, (ch & 0x08));
  digitalWrite(_s2, (ch & 0x04));
  digitalWrite(_s1, (ch & 0x02));
  digitalWrite(_s0, (ch & 0x01));
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
  if (_numMCP >= MCP_MAX_NUMBER)
  {
    return false;
  }
  if (!_mcp[_numMCP].begin_I2C(adress, &Wire))
  {
    return false;
  }
  for (int i = 0; i < 16; i++)
  {
    // TODO: register write iodir = 0xffff, ipol = 0xffff, gppu = 0xffff
    _mcp[_numMCP].pinMode(i, INPUT_PULLUP);
  }
  _numMCP++;
  _pin[_nExpanders++] = MCP_PIN;
  return true;
}
#endif

// Gets specific pin from mux, number according to initialization order 
bool DigitalIn_::getBit(uint8_t mux, uint8_t pin, bool direct)
{
  bool res;
  
  // Unintuitive evaluation order favors multiplexers, 
  // which also have to perform the selector setting
  if (mux != NOT_USED)
  {
    if(direct && !isMCP(mux)) {
        setMuxChannel(pin);
        res = !digitalRead(_pin[mux]);
    } else {
        res = bitRead(_data[mux], pin);
    }
  } else {
    res = !digitalRead(pin);
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
    for (uint8_t muxch = 0; muxch < 16; muxch++)
    {
      setMuxChannel(muxch);
      for (uint8_t nExp = 0; nExp < _nExpanders; nExp++)
      {
        if (_pin[nExp] != MCP_PIN)
        {
          bitWrite(_data[nExp], muxch, !digitalRead(_pin[nExp]));
        }
      }
    }
  }
#if MCP_MAX_NUMBER > 0
  int mcp = 0;
  for (uint8_t nExp = 0; nExp < _nExpanders; nExp++)
  {
    if (_pin[nExp] == MCP_PIN)
    {
      _data[nExp] = ~_mcp[nExp++].readGPIOAB();
    }
  }
#endif
}

DigitalIn_ DigitalIn;
