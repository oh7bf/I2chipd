#include "i2chipd.h"
#include "I2cWriteCommandRead3.h"
#include "InsertMySQL.h"
#include "InsertSQLite.h"
#include "WriteFile.h"

void ReadHtu21d()
{
  const char hdatafile[200]="/var/lib/i2chipd/htu21d_humidity";
  const char tdatafile[200]="/var/lib/i2chipd/htu21d_temperature";
  const char query[ SQLITEQUERY_SIZE ]="insert into htu21d (name,temperature,humidity) values (?,?,?)";
  const char mquery[ SQLITEQUERY_SIZE ]="insert into htu21d values(default,default,?,?,?)";

  unsigned short H=0, T=0;
  unsigned int D=0;
  double rhum=-100;
  double temp=-100;
  char message[200]="";

  float data[ SQLITE_FLOATS ];

  D = I2cWriteCommandRead3(HTU21D_ADDRESS, HTU21D_TRIG_HUM_MEAS, 16000);
  if( D == UINT_MAX )
  {
    syslog(LOG_INFO|LOG_DAEMON, "HTU21D humidity reading failed");
  }
  else
  {
    H = (unsigned short)(D>>8);
    H &= 0xFFFC;
    rhum = -6+125*((double)H)/65536.0;
    sprintf(message, "HTU21D humidity %-5.1f %%", rhum);
    syslog(LOG_DEBUG, "%s", message);
    WriteFile(hdatafile, rhum);

    D = I2cWriteCommandRead3(HTU21D_ADDRESS, HTU21D_TRIG_TEMP_MEAS, 50000);
    if( D == UINT_MAX )
    {
      syslog(LOG_INFO|LOG_DAEMON, "HTU21D temperature reading failed");
    }
    else
    {
      T = (unsigned short)(D>>8);
      T &= 0xFFFC;
      temp = -46.85+175.72*((double)T)/65536.0;
      sprintf(message, "HTU21D temperature %-+6.3f C", temp);
      syslog(LOG_DEBUG, "%s", message);
      WriteFile(tdatafile, temp);

      sprintf(message, "HTU21D T=%-+6.3f C RH=%-5.1f %% ", temp, rhum);
      syslog(LOG_INFO|LOG_DAEMON, message);

      data[0] = (float)temp;
      data[1] = (float)rhum; 

      if( dbsqlite == 1 ) InsertSQLite(dbfile, query, "rh1", 2, data);
      if( dbmysql == 1 ) InsertMySQL(dbhost, dbuser, dbpswd, database, dbport, mquery, "rh1", 2, data);
    }
  }
}

