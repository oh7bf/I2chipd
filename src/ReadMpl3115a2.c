#include "i2chipd.h"
#include "I2cWriteRegister.h"
#include "I2cRReadRegBytes.h"
#include "InsertMySQL.h"
#include "InsertSQLite.h"
#include "WriteFile.h"

void ReadMpl3115a2()
{
  double temperature = 0; // temperature [C]
  double pressure = 0; // pressure [Pa]
  double altitude = 0; // altitude [m]

  const char adatafile[200] = "/var/lib/i2chipd/mpl3115a2_altitude";
  const char pdatafile[200] = "/var/lib/i2chipd/mpl3115a2_pressure";
  const char tdatafile[200] = "/var/lib/i2chipd/mpl3115a2_temperature";

  char message[200]="";

  short T = 0;
  int P = 0;
  unsigned int A = 0;
  unsigned char buffer[10];

  const char query[ SQLITEQUERY_SIZE ] = "insert into mpl3115a2 (name,temperature,pressure,altitude) values (?,?,?,?)";  
  const char mquery[ SQLITEQUERY_SIZE ] = "insert into mpl3115a2 values(default,default,?,?,?,?)";  

  float data[ SQLITE_FLOATS ];

  if( mpl3115a2c.interval > 0 )
  {
    if( I2cWriteRegister(MPL3115A2_ADDRESS, 0x26, 0x38) == 1 )
    {
      if( I2cWriteRegister(MPL3115A2_ADDRESS, 0x13, 0x07) == 1)
      {
        if( I2cWriteRegister(MPL3115A2_ADDRESS, 0x26, 0x39) == 1)
        {

          usleep(500000);

          if( I2cRReadRegBytes(MPL3115A2_ADDRESS, MPL3115A2_STATUS, buffer, 6) == 1 )
          {
            T = ((short)buffer[4])<<8;
            T |= (short)buffer[5];
            temperature = ((double)T)/256.0; 
            P = ((int)buffer[1])<<16;
            P |= ((int)buffer[2])<<8;
            P |= ((int)buffer[3]);
            pressure = ((double)P)/64.0;

            sprintf(message, "MPL3115A2 T=%-+6.3f P=%-9.2f", temperature, pressure);
            syslog(LOG_INFO|LOG_DAEMON, "%s", message);

            WriteFile( pdatafile, pressure);
            WriteFile( tdatafile, temperature); 

            data[0] = (float)temperature;
            data[1] = (float)pressure;
            data[2] = 0;

            if( dbsqlite == 1 ) InsertSQLite(dbfile, query, "p1", 3, data);
            if( dbmysql == 1 ) InsertMySQL(dbhost, dbuser, dbpswd, database, dbport, mquery, "p1", 3, data);
          }
          else
          {
            syslog(LOG_ERR|LOG_DAEMON, "MPL3115A2 temperature and pressure reading failed");
          }
        }
      }
    }
  }

  if( mpl3115a2c.altint > 0 )
  {
    if( I2cWriteRegister(MPL3115A2_ADDRESS, 0x26, 0xB8) == 1 )
    {
      if( I2cWriteRegister(MPL3115A2_ADDRESS, 0x13, 0x07) == 1)
      {
        if( I2cWriteRegister(MPL3115A2_ADDRESS, 0x26, 0xB9) == 1)
        {
          usleep(500000);

          if( I2cRReadRegBytes(MPL3115A2_ADDRESS, MPL3115A2_STATUS, buffer, 6) == 1 )
          {
            T = ((short)buffer[4])<<8;
            T |= (short)buffer[5];
            temperature = ((double)T)/256.0; 
            A = ((unsigned int)buffer[1])<<24;
            A |= ((unsigned int)buffer[2])<<16;
            A |= ((unsigned int)buffer[3])<<8;
            altitude = ((double)A)/65536.0;

            sprintf(message, "MPL3115A2 T=%-+6.3f A=%-9.2f", temperature, altitude);
            syslog(LOG_INFO|LOG_DAEMON, "%s", message);

            WriteFile( adatafile, pressure);
            WriteFile( tdatafile, temperature); 

            data[0] = (float)temperature;
            data[1] = 0;
            data[2] = (float)altitude;

            if(dbsqlite==1) InsertSQLite(dbfile, query, "a1", 3, data);
            if( dbmysql == 1 ) InsertMySQL(dbhost, dbuser, dbpswd, database, dbport, mquery, "a1", 3, data);
          }
          else
          {
            syslog(LOG_ERR|LOG_DAEMON, "MPL3115A2 temperature and altitude reading failed");
          }
        }
      }
    }
  }

}

