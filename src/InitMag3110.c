#include "i2chipd.h"
#include "I2cReadRegByte.h"
#include "I2cReadRegVector.h"
#include "I2cWriteRegister.h"
#include "I2cWriteRegVector.h"

int InitMag3110()
{
  short xoffset=0,yoffset=0,zoffset=0;
  char message[200]="";

  if( I2cReadRegByte(MAG3110_ADDRESS, MAG3110_WHO_AM_I) != 0xC4 ) 
  {
    syslog(LOG_ERR|LOG_DAEMON, "%s", "MAG3110 device id reading failed");
    return -1;
  }
  else
  {
    if( I2cWriteRegister(MAG3110_ADDRESS, MAG3110_CTRL_REG1, mag3110c.ctrlreg1) != 1 )
    {
      syslog(LOG_ERR|LOG_DAEMON, "%s", "MAG3110 writing CTRL_REG1 failed");
      return -2;
    }
    else
    {
      if( I2cWriteRegister(MAG3110_ADDRESS, MAG3110_CTRL_REG2, mag3110c.ctrlreg2) != 1 )

      {
        syslog(LOG_ERR|LOG_DAEMON, "%s", "MAG3110 writing CTRL_REG2 failed");
        return -3;
      }
      else
      {
        if( I2cWriteRegVector(MAG3110_ADDRESS, MAG3110_OFF_X_MSB, &mag3110c.offx, &mag3110c.offy, &mag3110c.offz) != 1)

        {
          syslog(LOG_ERR|LOG_DAEMON, "%s", "MAG3110 writing offset failed");
          return -4;
        }

        if( I2cReadRegVector(MAG3110_ADDRESS, MAG3110_OFF_X_MSB, &xoffset, &yoffset, &zoffset) == 1)
        {
           sprintf(message, "MAG3110 offsetx=%d offsety=%d offsetz=%d", xoffset, yoffset, zoffset);
          syslog(LOG_INFO|LOG_DAEMON, "%s", message);
        }
        else
        {
          syslog(LOG_ERR|LOG_DAEMON, "%s", "MAG3110 offset reading failed");
          return -5;
        }
      }
    }
  }

  return 0;
}
