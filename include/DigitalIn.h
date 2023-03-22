#ifndef DigitalIn_h
#define DigitalIn_h
#include <Arduino.h>

/// @brief Maximum number of 74HC4067 multiplexers
#ifndef MUX_MAX_NUMBER
#define MUX_MAX_NUMBER 6
#endif

/// @brief Maximum number of MCP23017 multiplexers
#ifndef MCP_MAX_NUMBER
#define MCP_MAX_NUMBER 0
#endif

// Include i2c lib only when needed
#if MCP_MAX_NUMBER > 0
#include <Adafruit_MCP23X17.h>
#endif

#define NOT_USED 255
#define MCP_PIN  254

/// @brief Class to encapsulate digital inputs from 74HC4067 and MCP23017 input multiplexers,
/// used by all digital input devices. Scans all expander inputs into internal process data image.
class DigitalIn_
{
public:
  /// @brief Class constructor
  DigitalIn_();

  /// @brief Set selector pins for 74HC4067 multiplexers. All multiplexers share the same selector pins.
  /// @param s0 Selector pin s0
  /// @param s1 Selector pin s1
  /// @param s2 Selector pin s2
  /// @param s3 Selector pin s3
  void setMux(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3);
  
  /// @brief Add one 74HC4067 multiplexer
  /// @param pin Data pin the multiplexer is connected to
  /// @return true when successful, false when all multiplexers have been used up (increase MUX_MAX_NUMBER)
  bool addMux(uint8_t pin);

#if MCP_MAX_NUMBER > 0
  /// @brief Add one MCP23017 I2C IO expander
  /// @param address I2C address of the expander (valid: 0x20-0x28)
  /// @return true when successful, false when all expanders have been used up (increase MCP_MAX_NUMBER)
  bool addMCP(uint8_t address);

  bool isMCP(uint8_t index) { return (_pin[index] == MCP_PIN); }
#else
  bool isMCP(uint8_t index) { return false; }
#endif
  
  /// @brief Get the state of one input from either a direct input pin or an expander.
  /// @param nExp expander to read from. Use NOT_USED to access Arduino digital input pin
  /// @param nChannel expander channel (0-15), or Arduino pin number when nExp = NOT_USED
  /// @param direct poll actual input instead of cache (effective on multiplexers only)
  /// @return Status of the input (negative logic: true = GND, false = +5V)
  bool getBit(uint8_t nExp, uint8_t nChannel, bool direct = false);
  
  /// @brief Read all expander inputs into data cache; direct pins are not included (always read directly)
  void handle();
private:
  uint8_t _s0, _s1, _s2, _s3;
#ifdef ARDUINO_ARCH_AVR
  uint8_t _S0port, _S1port, _S2port, _S3port;
  uint8_t _S0mask, _S1mask, _S2mask, _S3mask;
#endif

  uint8_t _nExpanders;
  uint8_t _pin[MUX_MAX_NUMBER + MCP_MAX_NUMBER];
  int16_t _data[MUX_MAX_NUMBER + MCP_MAX_NUMBER];
#if MCP_MAX_NUMBER > 0
  uint8_t _numMCP;
  Adafruit_MCP23X17 _mcp[MCP_MAX_NUMBER];
#endif
  /// @brief Set multiplexer channel.
  /// @param ch Channel number (0..15)
  void setMuxChannel(uint8_t ch);
  
};

/// @brief Instance of the class for system wide use
extern DigitalIn_ DigitalIn;

#endif
