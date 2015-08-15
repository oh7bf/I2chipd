/**************************************************************************
 * 
 * Read chips with I2C and write data to a file and database. 
 *       
 * Copyright (C) 2015 Jaakko Koivuniemi.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************
 *
 * Sat Jul  4 11:36:06 CEST 2015
 * Edit: Sat Aug 15 16:13:17 CEST 2015
 *
 * Jaakko Koivuniemi
 **/

#include "i2chipd.h"
#include "I2cReadBytes.h"
#include "I2cRReadRegBytes.h"
#include "I2cWriteCommand.h"
#include "I2cWriteCommandRead3.h"
#include "InitBMP180.h"
#include "InitHtu21d.h"
#include "InitMag3110.h"
#include "InitMpl3115a2.h"
#include "InitTmp102_1.h"
#include "InitTmp102_2.h"
#include "InitTmp102_3.h"
#include "InitTmp102_4.h"
#include "InsertMySQL.h"
#include "InsertSQLite.h"
#include "ReadBMP180.h"
#include "ReadConfig.h"
#include "ReadHtu21d.h"
#include "ReadMag3110.h"
#include "ReadMpl3115a2.h"
#include "ReadTmp102_1.h"
#include "ReadTmp102_2.h"
#include "ReadTmp102_3.h"
#include "ReadTmp102_4.h"
#include "ReadMySQLTime.h"
#include "ReadSQLiteTime.h"

const int version=20150815; // program version
const char *i2cdev="/dev/i2c-1";
int i2cerr=0; // error flag for i2c communication 
const char *confile="/etc/i2chipd_config";
const char pidfile[200]="/var/run/i2chipd.pid";
int loglev=5; // log level
char message[200]="";

int  dbsqlite=0; 
char *dbfile=0;
int dbmysql=0; 
char *dbhost=0;
char *dbuser=0;
char *dbpswd=0;
char *database=0;
int dbport=3306;

struct bmp180 bmp180c;
struct htu21d htu21dc;
struct mag3110 mag3110c;
struct mpl3115a2 mpl3115a2c;
struct tmp102 tmp102c1, tmp102c2, tmp102c3, tmp102c4;

int cont=1; /* main loop flag */

void stop(int sig)
{
  sprintf(message, "signal %d catched, stop", sig);
  syslog(LOG_NOTICE|LOG_DAEMON, "%s", message);
  cont=0;
}

void terminate(int sig)
{
  sprintf(message, "signal %d catched", sig);
  syslog(LOG_NOTICE|LOG_DAEMON, "%s", message);

  sleep(1);
  syslog(LOG_NOTICE|LOG_DAEMON, "stop");

  cont=0;
}

void hup(int sig)
{
  sprintf(message,"signal %d catched",sig);
  syslog(LOG_NOTICE|LOG_DAEMON, "%s", message);
}


int main()
{  
  int ok=0;

  setlogmask(LOG_UPTO (loglev));
  syslog(LOG_NOTICE|LOG_DAEMON, "i2chipd v. %d started",version); 

  signal(SIGINT,&stop); 
  signal(SIGKILL,&stop); 
  signal(SIGTERM,&terminate); 
  signal(SIGQUIT,&stop); 
  signal(SIGHUP,&hup); 

  dbfile = malloc(sizeof(char)*SQLITEFILENAME_SIZE);
  dbhost = malloc(sizeof(char)*DBHOSTNAME_SIZE);
  dbuser = malloc(sizeof(char)*DBUSER_SIZE);
  dbpswd = malloc(sizeof(char)*DBPSWD_SIZE);
  database = malloc(sizeof(char)*DBNAME_SIZE);

  bmp180c.interval = 0; // disable BMP180 reading
  htu21dc.interval = 0; // disable HTU21D reading
  htu21dc.drop = 0; // drop from loop if i2c error
  mag3110c.interval = 0; // disable MAG3110 reading
  mpl3115a2c.interval = 0; // disable MPL3115A2 reading
  tmp102c1.interval = 0; // disable TMP102 at 0x48 reading 
  tmp102c2.interval = 0; // disable TMP102 at 0x49 reading 
  tmp102c3.interval = 0; // disable TMP102 at 0x4A reading
  tmp102c4.interval = 0; // disable TMP102 at 0x4B reading 

  ReadConfig();

  pid_t pid, sid;
        
  pid=fork();
  if(pid<0) 
  {
    exit(EXIT_FAILURE);
  }

  if(pid>0) 
  {
    exit(EXIT_SUCCESS);
  }

  umask(0);

  /* Create a new SID for the child process */
  sid=setsid();
  if(sid<0) 
  {
    syslog(LOG_ERR|LOG_DAEMON, "failed to create child process"); 
    exit(EXIT_FAILURE);
  }
        
  if((chdir("/")) < 0) 
  {
    syslog(LOG_ERR|LOG_DAEMON, "failed to change to root directory"); 
    exit(EXIT_FAILURE);
  }
        
  /* Close out the standard file descriptors */
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  FILE *pidf;
  pidf=fopen(pidfile,"w");

  if(pidf==NULL)
  {
    sprintf(message,"Could not open PID lock file %s, exiting", pidfile);
    syslog(LOG_ERR|LOG_DAEMON, "%s", message);
    exit(EXIT_FAILURE);
  }

  if(flock(fileno(pidf),LOCK_EX||LOCK_NB)==-1)
  {
    sprintf(message,"Could not lock PID lock file %s, exiting", pidfile);
    syslog(LOG_ERR|LOG_DAEMON, "%s", message);
    exit(EXIT_FAILURE);
  }

  fprintf(pidf,"%d\n",getpid());
  fclose(pidf);

  if( bmp180c.interval > 0 )
  {
    if( InitBMP180() < 0 )
    {
      syslog(LOG_ERR|LOG_DAEMON, "BMP180 calibration data reading failed, drop chip from loop");
      bmp180c.interval = 0;
    }
  }

  if( htu21dc.interval > 0 )
  {
    if( InitHtu21d() < 0 )
    {
      syslog(LOG_ERR|LOG_DAEMON, "HTU21D init failed, drop chip from loop");
      htu21dc.interval = 0;
    }
  }

  if( mag3110c.interval > 0 )
  {
    if( InitMag3110() < 0 )
    {
      syslog(LOG_ERR|LOG_DAEMON, "MAG3110 init failed, drop chip from loop");
      mag3110c.interval = 0;
    }
  }

  if( mpl3115a2c.interval > 0 )
  {
    if( InitMpl3115a2() < 0 )
    {
      syslog(LOG_ERR|LOG_DAEMON, "MPL3115A2 init failed, drop chip from loop");
      mpl3115a2c.interval = 0;
    }
  }

  if( tmp102c1.interval > 0 )
  {
    if( InitTmp102_1() < 0 )
    {
      syslog(LOG_ERR|LOG_DAEMON, "TMP102_1 init failed, drop chip from loop");
      tmp102c1.interval = 0;
    }
  }

  if( tmp102c2.interval > 0 )
  {
    if( InitTmp102_2() < 0 )
    {
      syslog(LOG_ERR|LOG_DAEMON, "TMP102_2 init failed, drop chip from loop");
      tmp102c2.interval = 0;
    }
  }

  if( tmp102c3.interval > 0 )
  {
    if( InitTmp102_3() < 0 )
    {
      syslog(LOG_ERR|LOG_DAEMON, "TMP102_3 init failed, drop chip from loop");
      tmp102c3.interval = 0;
    }
  }

  if( tmp102c4.interval > 0 )
  {
    if( InitTmp102_4() < 0 )
    {
      syslog(LOG_ERR|LOG_DAEMON, "TMP102_4 init failed, drop chip from loop");
      tmp102c4.interval = 0;
    }
  }

  if( dbsqlite == 1 )
  {
    if( ReadSQLiteTime( dbfile ) == 0 ) 
    {
      syslog(LOG_ERR|LOG_DAEMON, "SQLite database read failed, drop database connection");
      dbsqlite=0; 
    }
  }

  if( dbmysql == 1 )
  {
    if( ReadMySQLTime(dbhost, dbuser, dbpswd, database, dbport) == 0 ) 
    {
      syslog(LOG_ERR|LOG_DAEMON, "MySQL database read failed, drop database connection");
      dbmysql=0; 
    }
  }

  int unxs=(int)time(NULL); // unix seconds
  bmp180c.next = unxs; // next time to read BMP180
  htu21dc.next = unxs; // next time to read HTU21D
  mag3110c.next = unxs; // next time to read MAG3110
  mpl3115a2c.next = unxs; // next time to read MPL3115A2
  tmp102c1.next = unxs; // next time to read TMP102 1
  tmp102c2.next = unxs; // next time to read TMP102 2
  tmp102c3.next = unxs; // next time to read TMP102 3
  tmp102c4.next = unxs; // next time to read TMP102 4

  while(cont==1)
  {
    unxs=(int)time(NULL); 

    if( bmp180c.interval > 0 )
    {
      if( ( unxs >= bmp180c.next )||( (bmp180c.next - unxs) > bmp180c.next) ) 
      {
        bmp180c.next = bmp180c.interval + unxs;
        i2cerr = 0;
        ReadBMP180();
        if( i2cerr < 0 )
        {
          syslog(LOG_ERR|LOG_DAEMON, "BMP180 reading failed, drop from loop");
          bmp180c.interval = 0;
        }
      }
    }

    if( htu21dc.interval > 0 )
    {
      if( ( unxs >= htu21dc.next )||( (htu21dc.next - unxs) > htu21dc.next) ) 
      {
        htu21dc.next = htu21dc.interval + unxs;
        i2cerr = 0;
        ReadHtu21d();
        if( i2cerr < 0 )
        {
          if( htu21dc.drop == 1 )
          {
            syslog(LOG_ERR|LOG_DAEMON, "HTU21D reading failed, drop from loop");
            htu21dc.interval = 0;
          }
        }
      }
    }

    if( mag3110c.interval > 0 )
    {
      if( ( unxs >= mag3110c.next )||( (mag3110c.next - unxs) > mag3110c.next) ) 
      {
        mag3110c.next = mag3110c.interval + unxs;
        i2cerr = 0;
        ReadMag3110();
        if( i2cerr < 0 )
        {
          syslog(LOG_ERR|LOG_DAEMON, "MAG3110 reading failed, drop from loop");
          mag3110c.interval = 0;
        }
      }
    }

    if( mpl3115a2c.interval > 0 )
    {
      if( ( unxs >= mpl3115a2c.next )||( (mpl3115a2c.next - unxs) > mpl3115a2c.next) ) 
      {
        mpl3115a2c.next = mpl3115a2c.interval + unxs;
        i2cerr = 0;
        ReadMpl3115a2();
        if( i2cerr < 0 )
        {
          syslog(LOG_ERR|LOG_DAEMON, "MPL3115A2 reading failed, drop from loop");
          mpl3115a2c.interval = 0;
        }
      }
    }

    if( tmp102c1.interval > 0 )
    {
      if( ( unxs >= tmp102c1.next )||( (tmp102c1.next - unxs) > tmp102c1.next) ) 
      {
        tmp102c1.next = tmp102c1.interval + unxs;
        i2cerr = 0;
        ReadTmp102_1();
        if( i2cerr < 0 )
        {
          syslog(LOG_ERR|LOG_DAEMON, "TMP102_1 reading failed, drop from loop");
          tmp102c1.interval = 0;
        }
      }
    }

    if( tmp102c2.interval > 0 )
    {
      if( ( unxs >= tmp102c2.next )||( (tmp102c2.next - unxs) > tmp102c2.next) ) 
      {
        tmp102c2.next = tmp102c2.interval + unxs;
        i2cerr = 0;
        ReadTmp102_2();
        if( i2cerr < 0 )
        {
          syslog(LOG_ERR|LOG_DAEMON, "TMP102_2 reading failed, drop from loop");
          tmp102c2.interval = 0;
        }
      }
    }

    if( tmp102c3.interval > 0 )
    {
      if( ( unxs >= tmp102c3.next )||( (tmp102c3.next - unxs) > tmp102c3.next) ) 
      {
        tmp102c3.next = tmp102c3.interval + unxs;
        i2cerr = 0;
        ReadTmp102_3();
        if( i2cerr < 0 )
        {
          syslog(LOG_ERR|LOG_DAEMON, "TMP102_3 reading failed, drop from loop");
          tmp102c3.interval = 0;
        }
      }
    }

    if( tmp102c4.interval > 0 )
    {
      if( ( unxs >= tmp102c4.next )||( (tmp102c4.next - unxs) > tmp102c4.next) ) 
      {
        tmp102c4.next = tmp102c4.interval + unxs;
        i2cerr = 0;
        ReadTmp102_4();
        if( i2cerr < 0 )
        {
          syslog(LOG_ERR|LOG_DAEMON, "TMP102_4 reading failed, drop from loop");
          tmp102c4.interval = 0;
        }
      }
    }

    sleep(1);
  }

  syslog(LOG_NOTICE|LOG_DAEMON, "remove PID file");
  ok=remove(pidfile);

  return ok;
}
