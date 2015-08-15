#include "i2chipd.h"

// write vector of three shorts beginning with register address byte
// return one in success and negative in case of failure, global
// i2cerr is set to negative in more serious failure
int I2cWriteRegVector(int address, unsigned char reg, short *xval, short *yval, short *zval)
{
  int fd, rd;
  int cnt = 0;
  unsigned char buf[ 10 ];
  char message[ 200 ] = "";

  if( ( fd = open(i2cdev, O_RDWR) ) < 0) 
  {
    syslog(LOG_ERR|LOG_DAEMON, "Failed to open i2c port");
    return -1;
  }

  rd = flock(fd, LOCK_EX|LOCK_NB);
  cnt = I2LOCK_MAX;
  while( ( rd == 1 ) && ( cnt > 0 ) ) // try again if port locking failed
  {
    sleep(1);
    rd = flock(fd, LOCK_EX|LOCK_NB);
    cnt--;
  }
  if(rd)
  {
    syslog(LOG_ERR|LOG_DAEMON, "Failed to open i2c port");
    close(fd);
    return -2;
  }

  buf[0] = reg;
  buf[1] = (unsigned char)(*xval>>8);
  buf[2] = (unsigned char)(*xval&0x00FF);
  buf[3] = (unsigned char)(*yval>>8);
  buf[4] = (unsigned char)(*yval&0x00FF);
  buf[5] = (unsigned char)(*zval>>8);
  buf[6] = (unsigned char)(*zval&0x00FF);

  if( ioctl(fd, I2C_SLAVE, address) < 0 ) 
  {
    syslog(LOG_ERR|LOG_DAEMON, "Unable to get bus access to talk to slave");
    i2cerr = -1;
    close( fd );
    return -3;
  }

  sprintf(message, "I2C[%02X] write register [%02X] [%02X%02X %02X%02X %02X%02X]", address, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);
  syslog(LOG_DEBUG, "%s", message);

  if( (write(fd, buf, 7)) != 7) 
  {
    syslog(LOG_ERR|LOG_DAEMON, "Error writing to i2c slave");
    i2cerr = -2;
    close( fd );
    return -4;
  }

  close( fd );

  return 1;
}

