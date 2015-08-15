#include "i2chipd.h"

// write command byte to i2c address and wait delay in microseconds 
// one is returned is success and negative number in case of failure,
// in case of more serious failure the global integer 'i2cerr' is set to zero 
int I2cWriteCommand(int address, unsigned char cmd, int delay)
{
  int fd, rd;
  int cnt = 0;
  unsigned char buf[ 10 ];
  char message[ 200 ] = "";

  if( ( fd = open(i2cdev, O_RDWR) ) < 0 ) 
  {
    syslog(LOG_ERR|LOG_DAEMON, "Failed to open i2c port");
    return -1;
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
    syslog(LOG_ERR|LOG_DAEMON, "Failed to open i2c port");
    close( fd );
    return -2;
  }

  buf[ 0 ] = cmd; 
  if(ioctl(fd, I2C_SLAVE, address) < 0) 
  {
    syslog(LOG_ERR|LOG_DAEMON, "Unable to get bus access to talk to slave");
    close( fd );
    i2cerr = -1;
    return -3;
  }

  sprintf(message, "I2C[%02X] write command [%02X]", address, buf[0]);
  syslog(LOG_DEBUG, "%s", message);

  if( (write(fd, buf, 1)) != 1 ) 
  {
    syslog(LOG_ERR|LOG_DAEMON, "Error writing to i2c slave");
    close( fd );
    i2cerr = -2;
    return -4;
  }

  usleep( delay );
  close( fd );

  return 1;
}

