#include "i2chipd.h"

// read data with from i2c address, length is the number of bytes to read
// return the data as int, in case of failure return: -1=open failed, 
// -2=lock failed, -3=bus access failed, -4=i2c slave reading failed
// global 'i2cerr' is set to negative in case of more serious failure
int I2cReadBytes(int address, int length)
{
  int rdata = 0;
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
    sleep( 1 );
    rd = flock(fd, LOCK_EX|LOCK_NB);
    cnt--;
  }

  if(rd)
  {
    syslog(LOG_ERR|LOG_DAEMON, "Failed to lock i2c port");
    close( fd );
    return -2;
  }

  if( ioctl(fd, I2C_SLAVE, address) < 0 ) 
  {
    syslog(LOG_ERR|LOG_DAEMON, "Unable to get bus access to talk to slave");
    i2cerr = -1;
    close( fd );
    return -3;
  }

  sprintf(message, "I2C[%02X] read", address);
  syslog(LOG_DEBUG, "%s", message);

  if( length == 1 )
  {
     if( read(fd, buf,1) != 1 ) 
     {
       syslog(LOG_ERR|LOG_DAEMON, "Unable to read from slave");
       i2cerr = -2;
       close(fd);
       return -4;
     }
     else 
     {
       rdata = buf[0];
       sprintf(message, "I2C receive [%02X] (%d)", buf[0], rdata);
       syslog(LOG_DEBUG, "%s", message); 
     }
  } 
  else if( length == 2 )
  {
     if( read(fd, buf,2) != 2 ) 
     {
       syslog(LOG_ERR|LOG_DAEMON, "Unable to read from slave");
       i2cerr = -2;
       close( fd );
       return -4;
     }
     else 
     {
       rdata = 256*buf[0] + buf[1];
       sprintf(message, "I2C receive [%02X %02X] (%d)", buf[0], buf[1], rdata);
       syslog(LOG_DEBUG, "%s", message);  
     }
  }
  else if( length == 4 )
  {
     if( read(fd, buf,4) != 4 ) 
     {
       syslog(LOG_ERR|LOG_DAEMON, "Unable to read from slave");
       i2cerr = -2;
       close(fd);
       return -4;
     }
     else 
     {
        rdata = 16777216*buf[0] + 65536*buf[1] + 256*buf[2] + buf[3];
        sprintf(message, "I2C receive [%02X %02X %02X %02X] (%d)", buf[0], buf[1], buf[2], buf[3], rdata);
        syslog(LOG_DEBUG, "%s", message);  
     }
  }

  close(fd);

  return rdata;
}


