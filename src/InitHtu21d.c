#include "i2chipd.h"
#include "I2cWriteCommand.h"

// Initialize Htu21d chip with I2C, return 0 in success and negative
// in case of failure. 

int InitHtu21d()
{
  char message[200]="";

  if( htu21dc.softreset == 1 ) 
  {
    if( I2cWriteCommand(HTU21D_ADDRESS, HTU21D_SOFT_RESET, 20000) != 1)
    {
      sprintf(message, "Chip reset failed");
      syslog(LOG_ERR|LOG_DAEMON, "%s", message);
      return -1;
    }
    else
    {
      sprintf(message, "Reset HTU21D");
      syslog(LOG_INFO|LOG_DAEMON, "%s", message);
    }
  }

  return 0;  
}

