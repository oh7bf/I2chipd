#include "i2chipd.h"
#include "I2cReadBytes.h"
#include "InsertMySQL.h"
#include "InsertSQLite.h"
#include "WriteFile.h"

void ReadTmp102_2()
{

  const char tdatafile[200]="/var/lib/i2chipd/tmp102_2_temperature";

  char message[200]="";

  int value;
  short temp;
  double temperature;

  const char query[ SQLITEQUERY_SIZE ] = "insert into tmp102 (name,temperature) values (?,?)";  
  const char mquery[ SQLITEQUERY_SIZE ] = "insert into tmp102 values(default,default,?,?)";
  float data[ SQLITE_FLOATS ];

  value = I2cReadBytes(TMP102_ADDRESS2, 2);
  if( value >= 0 )
  {
    temp = (short)(value);
    temp /= 16;
    temperature = (double)(temp*0.0625);

    sprintf(message, "TMP102_2 T=%+6.2f C", temperature);
    syslog(LOG_INFO|LOG_DAEMON, "%s", message);

    WriteFile( tdatafile, temp);

    data[0] = (float)temperature;
    if( dbsqlite == 1 ) InsertSQLite( dbfile, query, "T2", 1, data);
    if( dbmysql == 1 ) InsertMySQL(dbhost, dbuser, dbpswd, database, dbport, mquery, "T2", 1, data);
  }
  else
  {
    syslog(LOG_ERR|LOG_DAEMON, "TMP102_2 temperature reading failed");
  }
}
