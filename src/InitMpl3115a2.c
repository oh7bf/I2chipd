#include "i2chipd.h"
#include "I2cWriteRegister.h"
#include "I2cRReadRegBytes.h"

int InitMpl3115a2()
{
  unsigned char msb,lsb;

  if( mpl3115a2c.refpres > 0 )
  {
    msb = (unsigned char)(mpl3115a2c.refpres>>8);
    lsb = (unsigned char)(mpl3115a2c.refpres&0x00FF);
    if( I2cWriteRegister(MPL3115A2_ADDRESS, 0x14, msb) == 1 )
    {
      if( I2cWriteRegister(MPL3115A2_ADDRESS, 0x15, lsb) == 1 )
      {
        syslog(LOG_INFO|LOG_DAEMON, "%s", "MPL3115A2 set reference pressure");
      }
      else
      {
        syslog(LOG_ERR|LOG_DAEMON, "%s", "MPL3115A2 failed to write reference pressure");
        return -1;
      }
    }
    else
    {
      syslog(LOG_ERR|LOG_DAEMON, "%s", "MPL3115A2 failed to write reference pressure");
      return -1;
    }
  }

  return 0;
}
