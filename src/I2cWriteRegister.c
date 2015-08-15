#include "i2chipd.h"

// write byte to register address, return 1 in success and negative in case
// of failure, set global 'i2cerr' to negative in more serious failure
int I2cWriteRegister(int address, unsigned char reg, unsigned char value)
{
  int fd, rd;
  int cnt = 0;
  unsigned char buf[10];
  char message[200] = "";

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
    rd=flock(fd, LOCK_EX|LOCK_NB);
    cnt--;
  }

  if(rd)
  {
    syslog(LOG_ERR|LOG_DAEMON, "Failed to lock i2c port");
    close(fd);
    return -2;
  }

  buf[0] = reg;
  buf[1] = value;
  if(ioctl(fd, I2C_SLAVE, address) < 0) 
  {
    syslog(LOG_ERR|LOG_DAEMON, "Unable to get bus access to talk to slave");
    close(fd);
    i2cerr = -1;
    return -3;
  }

  sprintf(message, "I2C[%02X] write [%02X] to register [%02X]", address, buf[1], buf[0]);
  syslog(LOG_DEBUG, "%s", message);

  if( ( write(fd, buf, 2) ) != 2) 
  {
    syslog(LOG_ERR|LOG_DAEMON, "Error writing to i2c slave");
    close(fd);
    i2cerr = -2;
    return -4;
  }

  close(fd);

  return 1;
}

