#include "i2chipd.h"

void ReadConfig()
{
  FILE *cfile;
  char *line=NULL;
  char par[20];
  float value;
  size_t len;
  ssize_t read;

  char message[200]="";

  cfile = fopen(confile, "r");
  if(NULL != cfile)
  {
    syslog(LOG_INFO|LOG_DAEMON, "Read configuration file");

    while( (read=getline(&line, &len, cfile)) != -1)
    {
       if(sscanf(line, "%s %f", par, &value) != EOF) 
       {

          if(strncmp(par, "LOGLEVEL", 8) == 0)
          {
             loglev = (int)value;
             sprintf(message, "Log level set to %d", (int)value);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
             setlogmask(LOG_UPTO (loglev));
          }

          if(strncmp(par, "DBSQLITE", 8) == 0)
          {
            if(sscanf(line, "%s %s", par, dbfile) != EOF)  
            {
              dbsqlite = 1;
              sprintf(message, "Store data to database %s", dbfile);
              syslog(LOG_INFO|LOG_DAEMON, "%s", message);
            }
          }

          if(strncmp(par, "DBMYSQLHOST", 11) == 0)
          {
            if(sscanf(line, "%s %s", par, dbhost) != EOF)  
            {
              dbmysql = 1;
              sprintf(message, "MySQL host %s", dbhost);
              syslog(LOG_INFO|LOG_DAEMON, "%s", message);
            }
          }

          if(strncmp(par, "DBMYSQLUSER", 11) == 0)
          {
            if(sscanf(line, "%s %s", par, dbuser) != EOF)  
            {
              dbmysql = 1;
              sprintf(message, "MySQL user %s", dbuser);
              syslog(LOG_INFO|LOG_DAEMON, "%s", message);
            }
          }

          if(strncmp(par, "DBMYSQLPSWD", 11) == 0)
          {
            if(sscanf(line, "%s %s", par, dbpswd) != EOF)  
            {
              dbmysql = 1;
              syslog(LOG_INFO|LOG_DAEMON, "MySQL password set");
            }
          }

          if(strncmp(par, "DBMYSQLDB", 9) == 0)
          {
            if(sscanf(line, "%s %s", par, database) != EOF)  
            {
              dbmysql = 1;
              sprintf(message, "MySQL database %s", database);
              syslog(LOG_INFO|LOG_DAEMON, "%s", message);
            }
          }

          if(strncmp(par, "DBMYSQLPORT", 11) == 0)
          {
            dbport = (int)value;
            syslog(LOG_INFO|LOG_DAEMON, "MySQL host port %d", dbport);
          }

          if(strncmp(par, "BMP180_INT", 10) == 0)
          {
             bmp180c.interval = (int)value;
             sprintf(message, "BMP180 reading interval set to %d s", (int)value);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
          }

          if(strncmp(par, "HTU21D_INT", 10) == 0)
          {
             htu21dc.interval = (int)value;
             sprintf(message,"HTU21D reading interval set to %d s",(int)value);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
          }
          if(strncmp(par, "HTU21D_SOFTRESET", 16) == 0)
          {
             htu21dc.softreset = (int)value;
             if( htu21dc.softreset == 1 )
               syslog(LOG_INFO|LOG_DAEMON, "HTU21D soft reset at start");
          }
          if(strncmp(par, "HTU21D_DROP", 11) == 0)
          {
             htu21dc.drop = (int)value;
             if( htu21dc.drop == 1 )
               syslog(LOG_INFO|LOG_DAEMON, "HTU21D drop if i2c failure");
          }

          if(strncmp(par, "MAG3110_INT", 11) == 0)
          {
             mag3110c.interval = (int)value;
             sprintf(message,"MAG3110 reading interval set to %d s",(int)value);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
          }
          if(strncmp(par, "MAG3110_CTRL_REG1", 17) == 0)
          {
             mag3110c.ctrlreg1 = (unsigned char)value;
             sprintf(message, "MAG3110 control register 1 set to %02x", mag3110c.ctrlreg1);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
          }
          if(strncmp(par, "MAG3110_CTRL_REG2", 17) == 0)
          {
             mag3110c.ctrlreg2 = (unsigned char)value;
             sprintf(message, "MAG3110 control register 2 set to %02x", mag3110c.ctrlreg2);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
          }
          if(strncmp(par, "MAG3110_OFF_X", 13) == 0)
          {
             mag3110c.offx = (short)value;
             sprintf(message, "MAG3110 user offset x set to %d", mag3110c.offx);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
          }
          if(strncmp(par, "MAG3110_OFF_Y", 13) == 0)
          {
             mag3110c.offy = (short)value;
             sprintf(message, "MAG3110 user offset y set to %d", mag3110c.offy);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
          }
          if(strncmp(par, "MAG3110_OFF_Z", 13) == 0)
          {
             mag3110c.offz = (short)value;
             sprintf(message, "MAG3110 user offset z set to %d", mag3110c.offz);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
          }
          if(strncmp(par, "MAG3110_TDELAY", 16) == 0)
          {
             mag3110c.tdelay = (int)value;
             sprintf(message, "MAG3110 delay before reading set to %d", mag3110c.tdelay);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
          }
          if(strncmp(par, "MAG3110_TEMPINT", 15) == 0)
          {
             mag3110c.tempint = (int)value;
             sprintf(message, "MAG3110 temperature reading interval set to %d", mag3110c.tempint);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
          }
          if(strncmp(par, "MAG3110_TOFFSET", 15) == 0)
          {
             mag3110c.toffset = (int)value;
             sprintf(message, "MAG3110 temperature offset set to %d", mag3110c.toffset);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
          }

          if(strncmp(par, "MPL3115A2_INT", 13) == 0)
          {
             mpl3115a2c.interval = (int)value;
             sprintf(message, "MPL3115A2 reading interval set to %d", mpl3115a2c.interval);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
          }
          if(strncmp(par, "MPL3115A2_ALT_INT", 17) == 0)
          {
             mpl3115a2c.altint = (int)value;
             sprintf(message, "MPL3115A2 altitude reading interval set to %d", mpl3115a2c.altint);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
          }
          if(strncmp(par, "MPL3115A2_REFPRES", 17) == 0)
          {
             mpl3115a2c.refpres = (unsigned short)value;
             sprintf(message, "MPL3115A2 reference pressure set to %d", mpl3115a2c.refpres);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
          }

          if(strncmp(par, "TMP102_1_INT", 12) == 0)
          {
             tmp102c1.interval = (int)value;
             sprintf(message, "TMP102 1 read interval set to %d", tmp102c1.interval);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
          }
          if(strncmp(par, "TMP102_2_INT", 12) == 0)
          {
             tmp102c2.interval = (int)value;
             sprintf(message, "TMP102 2 read interval set to %d", tmp102c2.interval);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
          }
          if(strncmp(par, "TMP102_3_INT", 12) == 0)
          {
             tmp102c3.interval = (int)value;
             sprintf(message, "TMP102 3 read interval set to %d", tmp102c3.interval);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
          }
          if(strncmp(par, "TMP102_4_INT", 12) == 0)
          {
             tmp102c4.interval = (int)value;
             sprintf(message, "TMP102 4 read interval set to %d", tmp102c4.interval);
             syslog(LOG_INFO|LOG_DAEMON, "%s", message);
          }

       }
    }
    fclose(cfile);
  }
  else
  {
    sprintf(message, "Could not open %s", confile);
    syslog(LOG_ERR|LOG_DAEMON, "%s", message);
  }
}

