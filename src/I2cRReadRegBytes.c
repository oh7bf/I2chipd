#include "i2chipd.h"

// read given number of bytes from register at i2c address with repeated start
// return 1 in success and negative in case of failure, set global 'i2cerr' 
// to negative in more serious failure
int I2cRReadRegBytes(int address, unsigned char reg, unsigned char *buffer, int nbytes)
{
  int fd, rd;
  int cnt = 0, result = 0;
  int i = 0;
  char message[500] = "";
  char hex[2] = ""; 

// from Lauri Pirttiaho
  struct i2c_msg rdwr_msgs[2] =
  {
    {  // Start address
      .addr = address,
      .flags = 0, // write
      .len = 1,
      .buf = &reg
    },
    { // Read buffer
      .addr = address,
      .flags = I2C_M_RD, // read
      .len = nbytes,
      .buf = buffer
    }
  };

  struct i2c_rdwr_ioctl_data rdwr_data =
  {
    .msgs = rdwr_msgs,
    .nmsgs = 2
  };

  if( (fd = open(i2cdev, O_RDWR) ) < 0 )
  {
    syslog(LOG_ERR|LOG_DAEMON, "Failed to open i2c port");
    return -1;
  }
  else
  {
    rd = flock(fd, LOCK_EX|LOCK_NB);

    cnt = I2LOCK_MAX;
    while( ( rd == 1 ) && ( cnt > 0 ) ) // try again if port locking failed
    {
      sleep(1);
      rd = flock(fd, LOCK_EX|LOCK_NB);
      cnt--;
    }

    if( rd )
    {
      syslog(LOG_ERR|LOG_DAEMON, "Failed to open i2c port");
      close( fd );
      return -2;
    }
    else
    {
      if( ioctl(fd, I2C_SLAVE, address) < 0 )
      {
        syslog(LOG_ERR|LOG_DAEMON, "Unable to get bus access to talk to slave");
        close( fd );
        i2cerr = -1;
        return -3;
      }
      else
      {
        sprintf(message, "I2C[%02X] repeated start read register [%02X]", address, reg);
        syslog(LOG_DEBUG, "%s", message);
        result = ioctl(fd, I2C_RDWR, &rdwr_data);
        if( result < 0 )
        {
          sprintf(message, "rdwr ioctl error: %d", errno);
          syslog(LOG_ERR|LOG_DAEMON, "%s", message);
          close( fd );
          i2cerr = -2;
          return -4;
        }
        else
        {
          sprintf(message, "I2C received [");
          for( i = 0; i <nbytes; i++ )
          {
            if( i == 0 ) sprintf( hex, "%02X", buffer[ i ] );
            else sprintf( hex, " %02X", buffer[ i ] );            
            strncat( message, hex, 3 );
          }
          strncat( message, "]", 1);
          syslog(LOG_DEBUG, "%s", message);
        }
      }
    }
  }

  close( fd );
  return 1;
}

