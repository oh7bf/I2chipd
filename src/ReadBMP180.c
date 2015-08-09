#include "i2chipd.h"
#include "I2cWriteRegister.h"
#include "I2cRReadRegBytes.h"
#include "InsertMySQL.h"
#include "InsertSQLite.h"
#include "WriteFile.h"

// Read pressure from BMP180 chip with I2C 

void ReadBMP180()
{

  const char adatafile[200]="/var/lib/i2chipd/bmp180_altitude";
  const char pdatafile[200]="/var/lib/i2chipd/bmp180_pressure";
  const char tdatafile[200]="/var/lib/i2chipd/bmp180_temperature";
  const char query[ SQLITEQUERY_SIZE ] = "insert into bmp180 (name,temperature,pressure,altitude) values (?,?,?,?)";
  const char mquery[ SQLITEQUERY_SIZE ] = "insert into bmp180 values(default,default,?,?,?,?)";


  long UT, UP, X1, X2, X3, B3, B5, B6, p;
  unsigned long B4, B7;
  double T = -100;
  float data[ SQLITE_FLOATS ];
  unsigned char buffer[10];

  char message[200]="";

  if( I2cWriteRegister(BMP180_ADDRESS, BMP180_CTRL_MEAS, 0x2E) == 1 )
  {
    usleep(4500);
    if( I2cRReadRegBytes(BMP180_ADDRESS, BMP180_OUT, buffer, 2) == 1 )
    {
      UT = ((long)buffer[0])<<8;
      UT |= (long)buffer[1]; 
      X1 = ((UT-bmp180c.AC6)*bmp180c.AC5)>>15;
      X2 = (((long)bmp180c.MC)<<11)/(X1+bmp180c.MD);
      B5 = X1 + X2;
      T = 0.1*((double)((B5+8)>>4));

      if( I2cWriteRegister(BMP180_ADDRESS, BMP180_CTRL_MEAS, 0xF4) == 1 )
      {
        usleep(25500);
        if( I2cRReadRegBytes(BMP180_ADDRESS, BMP180_OUT, buffer, 3) == 1 )
        {
          UP = ((long)buffer[0])<<16;
          UP |= ((long)buffer[1])<<8;
          UP |= ((long)buffer[2]);
          UP = UP>>5;
          B6 = B5 - 4000;
          X1 = (bmp180c.B2*((B6*B6)>>12))>>11;
          X2 = (bmp180c.AC2*B6)>>11;
          X3 = X1+X2;
          B3 = (((bmp180c.AC1*4+X3)<<3)+2)>>2;
          X1 = (bmp180c.AC3*B6)>>13;
          X2 = (bmp180c.B1*((B6*B6)>>12))>>16;
          X3 = ((X1+X2)+2)>>2;
          B4 = (bmp180c.AC4*((unsigned long)(X3+32768)))>>15;
          B7 = ((unsigned long)UP-B3)*(50000>>3);
          if( B7 < 0x80000000 ) p = (B7*2)/B4;
          else p = B7/B4*2;
          X1 = (p>>8)*(p>>8);
          X1 = (X1*3038)>>16;
          X2 = (-7357*p)>>16;
          p = p+(X1+X2+3791)/16;

          sprintf(message, "BMP180 T=%+6.2f C p=%ld Pa", T, p);
          syslog(LOG_INFO|LOG_DAEMON, message);

          WriteFile(tdatafile, T);
          WriteFile(pdatafile, p);
//          WriteFile(adatafile, a);

          data[0] = (float)T;
          data[1] = (float)p;
          data[2] = 0;

          if( dbsqlite == 1 ) InsertSQLite(dbfile, query, "p1", 3, data);
          if( dbmysql == 1 ) InsertMySQL(dbhost, dbuser, dbpswd, database, dbport, mquery, "p1", 3, data);
        }
      }
    }
  }
}

