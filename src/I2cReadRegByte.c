#include "i2chipd.h"

// read byte from register address
// return one in success and negative in case of failure, global
// i2cerr is set to negative in more serious failure
int I2cReadRegByte(int address, unsigned char reg)
{
  int value = 0;
  int fd, rd;
  int cnt = 0;
  unsigned char buf[ 10 ];
  char message[ 200 ] = "";

  if( (fd = open(i2cdev, O_RDWR) ) < 0 ) 
  {
    syslog(LOG_ERR|LOG_DAEMON, "Failed to open i2c port");
    return -1;
  }

  rd = flock(fd, LOCK_EX|LOCK_NB);
  cnt = I2LOCK_MAX;
  while( (rd == 1) && (cnt > 0) ) // try again if port locking failed
  {
    sleep(1);
    rd = flock(fd, LOCK_EX|LOCK_NB);
    cnt--;
  }

  if( rd )
  {
    syslog(LOG_ERR|LOG_DAEMON, "Failed to lock i2c port");
    close( fd );
    return -2;
  }

  buf[0] = reg;
  if(ioctl(fd, I2C_SLAVE, address) < 0) 
  {
    syslog(LOG_ERR|LOG_DAEMON, "Unable to get bus access to talk to slave");
    i2cerr = -1;
    close( fd );
    return -3;
  }

  sprintf(message, "I2C[%02X] read register [%02X]", address, reg);
  syslog(LOG_DEBUG, "%s", message);

  if((write(fd, buf, 1)) != 1) 
  {
    syslog(LOG_ERR|LOG_DAEMON, "Error writing to i2c slave");
    i2cerr = -2;
    close( fd );
    return -4;
  }
  else
  {
    if(read(fd, buf,1) != 1) 
    {
      syslog(LOG_ERR|LOG_DAEMON, "Unable to read from slave");
      i2cerr = -3;
      close( fd );
      return -5;
    }
    else 
    {
      value = (int)buf[0]; 

      sprintf(message, "I2C received [%02X] (%d)", buf[0], value);
      syslog(LOG_DEBUG, "%s", message);
    }
  }

  close( fd );

  return value;
}

