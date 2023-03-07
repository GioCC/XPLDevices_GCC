#include <XPLDevices.h>

// Interface
XPLDirect XP(&Serial);

void XPSetup(const char *devicename)
{
  Serial.begin(XPLDIRECT_BAUDRATE);
  XP.begin(&devicename);
}

void handleCommand(Button *btn)
{
  btn->handle();
  int cmdPush = btn->getCommand();
  if (btn->pressed())
  {
    XP.commandStart(cmdPush);
  }
  if (btn->released())
  {
    XP.commandEnd(cmdPush);
  }
}

void handleCommand(Button *btn, bool input)
{
  btn->handle(input);
  int cmdPush = btn->getCommand();
  if (btn->pressed())
  {
    XP.commandStart(cmdPush);
  }
  if (btn->released())
  {
    XP.commandEnd(cmdPush);
  }
}

void handleCommand(Encoder *enc)
{
  enc->handle();
  if (enc->up())
  {
    XP.commandTrigger(enc->getCommand(eEncCmdUp));
  }
  if (enc->down())
  {
    XP.commandTrigger(enc->getCommand(eEncCmdDown));
  }
  int cmdPush = enc->getCommand(eEncCmdPush);
  if (cmdPush >= 0)
  {
    if (enc->pressed())
    {
      XP.commandStart(cmdPush);
    }
    if (enc->released())
    {
      XP.commandEnd(cmdPush);
    }
  }
}

void handleCommand(Switch *sw)
{
  if (sw->handle())
  {
    XP.commandTrigger(sw->getCommand());
  }
}
