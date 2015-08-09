#include "i2chipd.h"
#include "I2cReadRegByte.h"
#include "I2cReadRegVector.h"
#include "I2cWriteRegister.h"
#include "InsertMySQL.h"
#include "InsertSQLite.h"
#include "WriteFile.h"

void ReadMag3110()
{
  const char mdatafilex[200]="/var/lib/i2chipd/mag3110_Bx";
  const char mdatafiley[200]="/var/lib/i2chipd/mag3110_By";
  const char mdatafilez[200]="/var/lib/i2chipd/mag3110_Bz";
  const char tdatafile[200]="/var/lib/i2chipd/mag3110_temperature";
  const char query[ SQLITEQUERY_SIZE ]="insert into mag3110 (name,Bx,By,Bz,temperature) values (?,?,?,?,?)";
  const char mquery[ SQLITEQUERY_SIZE ]="insert into mag3110 values(default,default,?,?,?,?,?)";

  char message[200]="";

  short Bx=0, By=0, Bz=0; // magnetic field [0.1 uT]
  int temp; // temperature

  float data[ SQLITE_FLOATS ];

  if( I2cWriteRegister(MAG3110_ADDRESS, MAG3110_CTRL_REG1, mag3110c.ctrlreg1|0x02) == 1 )
  {
    sleep( mag3110c.tdelay );

    if( I2cReadRegVector(MAG3110_ADDRESS, MAG3110_OUT_X_MSB, &Bx, &By, &Bz) == 1)
    {
      sprintf(message, "MAG3110 Bx=%d By=%d Bz=%d", Bx, By, Bz);
      syslog(LOG_INFO|LOG_DAEMON, "%s", message);

      WriteFile(mdatafilex, (double)Bx);
      WriteFile(mdatafiley, (double)By);
      WriteFile(mdatafilez, (double)Bz);

      temp = I2cReadRegByte(MAG3110_ADDRESS, MAG3110_DIE_TEMP);
      if( temp >= 0 )
      {
        temp = ( (signed char)(temp) - mag3110c.toffset );
        sprintf(message, "MAG3110 T=%+d C", temp);
        syslog(LOG_INFO|LOG_DAEMON, "%s", message);
        WriteFile(tdatafile, (double)temp);

        data[0] = (float)Bx;
        data[1] = (float)By;
        data[2] = (float)Bz;
        data[3] = (float)temp; 

        if( dbsqlite == 1 ) InsertSQLite(dbfile, query, "B1", 4, data);
        if( dbmysql == 1 ) InsertMySQL(dbhost, dbuser, dbpswd, database, dbport, mquery, "B1", 4, data);
      }
      else
      {
        syslog(LOG_INFO|LOG_DAEMON, "MAG3110 temperature reading failed");
      }
    }
    else
    {
      syslog(LOG_INFO|LOG_DAEMON, "MAG3110 field reading failed");
    }
  }
  else
  {
    syslog(LOG_INFO|LOG_DAEMON, "MAG3110 trigger failed");
  }
}
