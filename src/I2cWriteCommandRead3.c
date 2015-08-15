#include "i2chipd.h"

// write command byte to given i2c address and read three bytes of data
// in case of failure return UINT_MAX, in more serious failure set
// global integer 'i2cerr' to negative
unsigned int I2cWriteCommandRead3(int address, unsigned char cmd, int delay)
{
  int fd, rd;
  int cnt = 0;
  unsigned char buf[10];
  unsigned int D = 0;
  char message[200] = "";

  if( (fd=open(i2cdev, O_RDWR)) < 0 ) 
  {
    syslog(LOG_ERR|LOG_DAEMON, "Failed to open i2c port");
    return UINT_MAX;
  }

  rd = flock(fd, LOCK_EX|LOCK_NB);
  cnt = I2LOCK_MAX;
  while( (rd==1) && (cnt>0) ) // try again if port locking failed
  {
    sleep( 1 );
    rd = flock(fd, LOCK_EX|LOCK_NB);
    cnt--;
  }

  if( rd )
  {
    syslog(LOG_ERR|LOG_DAEMON, "Failed to lock i2c port");
    close( fd );
    return UINT_MAX;
  }

  buf[ 0 ] = cmd;
  if( ioctl(fd, I2C_SLAVE, address) < 0 ) 
  {
    syslog(LOG_ERR|LOG_DAEMON, "Unable to get bus access to talk to slave");
    close( fd );
    i2cerr = -1;
    return UINT_MAX;
  }

  sprintf(message, "I2C[%02X] write command [%02X]", address, buf[0]);
  syslog(LOG_DEBUG, "%s", message);

  if( (write(fd, buf, 1)) != 1 ) 
  {
    syslog(LOG_ERR|LOG_DAEMON, "Error writing to i2c slave");
    close( fd );
    i2cerr = -2;
    return UINT_MAX;
  }
  else
  {
    usleep( delay );
    if( read(fd, buf, 3) != 3 ) 
    {
      syslog(LOG_ERR|LOG_DAEMON, "Unable to read from slave");
      close( fd );
      i2cerr = -3;
      return UINT_MAX;
    }
    else 
    {
      D = ((unsigned int)buf[0])<<16;
      D |= ((unsigned int)buf[1])<<8;
      D |= (unsigned int)buf[2];
      sprintf(message, "I2C received [%02X %02X %02X] (%d)", buf[0],buf[1],buf[2], D);
      syslog(LOG_DEBUG, "%s", message);
    }
  }
  close( fd );

  return D;
}


