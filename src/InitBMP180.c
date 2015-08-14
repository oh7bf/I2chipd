#include "i2chipd.h"
#include "I2cRReadRegBytes.h"

// Initialize BMP180 chip with I2C, return 0 in success and negative
// in case of failure. This routine reads calibration data from
// BMP180.

int InitBMP180()
{
  unsigned char buffer[30];
  char message[200]="";

  if( I2cRReadRegBytes(BMP180_ADDRESS, BMP180_AC1, buffer, 22) == 1)
  {
    bmp180c.AC1 = ((short)buffer[0])<<8;
    bmp180c.AC1 |= (short)buffer[1];
    sprintf(message, "BMP180 AC1 = %d", bmp180c.AC1);
    syslog(LOG_NOTICE|LOG_DAEMON, message);

    bmp180c.AC2 = ((short)buffer[2])<<8;
    bmp180c.AC2 |= (short)buffer[3];
    sprintf(message, "AC2 = %d", bmp180c.AC2);
    syslog(LOG_NOTICE|LOG_DAEMON, message);

    bmp180c.AC3 = ((short)buffer[4])<<8;
    bmp180c.AC3 |= (short)buffer[5];
    sprintf(message, "AC3 = %d", bmp180c.AC3);
    syslog(LOG_NOTICE|LOG_DAEMON, message);

    bmp180c.AC4 = ((unsigned short)buffer[6])<<8;
    bmp180c.AC4 |= (unsigned short)buffer[7];
    sprintf(message, "AC4 = %d", bmp180c.AC4);
    syslog(LOG_NOTICE|LOG_DAEMON, message);

    bmp180c.AC5 = ((unsigned short)buffer[8])<<8;
    bmp180c.AC5 |= (unsigned short)buffer[9];
    sprintf(message, "AC5 = %d", bmp180c.AC5);
    syslog(LOG_NOTICE|LOG_DAEMON, message);

    bmp180c.AC6 = ((unsigned short)buffer[10])<<8;
    bmp180c.AC6 |= (unsigned short)buffer[11];
    sprintf(message, "AC6 = %d", bmp180c.AC6);
    syslog(LOG_NOTICE|LOG_DAEMON, message);

    bmp180c.B1 = ((short)buffer[12])<<8;
    bmp180c.B1 |= (short)buffer[13];
    sprintf(message, "B1 = %d", bmp180c.B1);
    syslog(LOG_NOTICE|LOG_DAEMON, message);

    bmp180c.B2 = ((short)buffer[14])<<8;
    bmp180c.B2 |= (short)buffer[15];
    sprintf(message, "B2 = %d", bmp180c.B2);
    syslog(LOG_NOTICE|LOG_DAEMON, message);

    bmp180c.MB = ((short)buffer[16])<<8;
    bmp180c.MB |= (short)buffer[17];
    sprintf(message, "MB = %d", bmp180c.MB);
    syslog(LOG_NOTICE|LOG_DAEMON, message);

    bmp180c.MC = ((short)buffer[18])<<8;
    bmp180c.MC |= (short)buffer[19];
    sprintf(message, "MC = %d", bmp180c.MC);
    syslog(LOG_NOTICE|LOG_DAEMON, message);

    bmp180c.MD = ((short)buffer[20])<<8;
    bmp180c.MD |= (short)buffer[21];
    sprintf(message, "MD = %d", bmp180c.MD);
    syslog(LOG_NOTICE|LOG_DAEMON, message);

  }
  else
  {
    syslog(LOG_ERR|LOG_DAEMON, "failed to read AC1...MD");
    return -1;
  }

  return 0;  
}

