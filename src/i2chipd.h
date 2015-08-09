#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>
#include <sqlite3.h>
#include <my_global.h>
#include <mysql.h>
#include <limits.h>

#ifndef I2CHIPD_H_INCLUDED
#define I2CHIPD_H_INCLUDED

#define I2LOCK_MAX 10

#define BMP180_ADDRESS 0x77
#define BMP180_AC1 0xAA
#define BMP180_AC2 0xAC
#define BMP180_AC3 0xAE
#define BMP180_AC4 0xB0
#define BMP180_AC5 0xB2
#define BMP180_AC6 0xB4
#define BMP180_B1 0xB6
#define BMP180_B2 0xB8
#define BMP180_MB 0xBA
#define BMP180_MC 0xBC
#define BMP180_MD 0xBE
#define BMP180_OUT 0xF6
#define BMP180_CTRL_MEAS 0xF4
#define BMP180_SOFT_RESET 0xE0
#define BMP180_ID 0xD0

#define HTU21D_ADDRESS 0x40
#define HTU21D_WRITE_USER_REG 0xE6
#define HTU21D_READ_USER_REG 0xE7
#define HTU21D_TRIG_TEMP_MEAS 0xF3
#define HTU21D_TRIG_HUM_MEAS 0xF5
#define HTU21D_SOFT_RESET 0xFE 

#define MAG3110_ADDRESS 0x0E
#define MAG3110_DR_STATUS 0x00
#define MAG3110_OUT_X_MSB 0x01
#define MAG3110_OUT_X_LSB 0x02
#define MAG3110_OUT_Y_MSB 0x03
#define MAG3110_OUT_Y_LSB 0x04
#define MAG3110_OUT_Z_MSB 0x05
#define MAG3110_OUT_Z_LSB 0x06
#define MAG3110_WHO_AM_I 0x07
#define MAG3110_SYSMOD 0x08
#define MAG3110_OFF_X_MSB 0x09
#define MAG3110_OFF_X_LSB 0x0A
#define MAG3110_OFF_Y_MSB 0x0B
#define MAG3110_OFF_Y_LSB 0x0C
#define MAG3110_OFF_Z_MSB 0x0D
#define MAG3110_OFF_Z_LSB 0x0E
#define MAG3110_DIE_TEMP 0x0F
#define MAG3110_CTRL_REG1 0x10
#define MAG3110_CTRL_REG2 0x11

#define MPL3115A2_ADDRESS 0x60
#define MPL3115A2_STATUS 0x00
#define MPL3115A2_OUT_P_MSB 0x01
#define MPL3115A2_OUT_P_CSB 0x02
#define MPL3115A2_OUT_P_LSB 0x03
#define MPL3115A2_OUT_T_MSB 0x04
#define MPL3115A2_OUT_T_LSB 0x05
#define MPL3115A2_DR_STATUS 0x06
#define MPL3115A2_OUT_P_DELTA_MSB 0x07
#define MPL3115A2_OUT_P_DELTA_CSB 0x08
#define MPL3115A2_OUT_P_DELTA_LSB 0x09
#define MPL3115A2_OUT_T_DELTA_MSB 0x0A
#define MPL3115A2_OUT_T_DELTA_LSB 0x0B
#define MPL3115A2_WHO_AM_I 0x0C
#define MPL3115A2_F_STATUS 0x0D

#define TMP102_ADDRESS1 0x48
#define TMP102_ADDRESS2 0x49
#define TMP102_ADDRESS3 0x4A
#define TMP102_ADDRESS4 0x4B
#define TMP102_TEMP_REG 0x00
#define TMP102_CONFIG_REG 0x01
#define TMP102_TEMP_LOW_REG 0x02
#define TMP102_TEMP_HIGH_REG 0x03

#define HOSTNAME_SIZE 63
#define SENSORNAME_SIZE 20
#define SQLITEFILENAME_SIZE 200
#define SQLITEQUERY_SIZE 200
#define SQLITE_FLOATS 10 
#define DBHOSTNAME_SIZE 200
#define DBUSER_SIZE 200
#define DBPSWD_SIZE 200
#define DBNAME_SIZE 200
#define MYSQLQUERY_SIZE 200

// BMP180 calibration data
struct bmp180 {
       int interval; // reading interval [s]
       int next; // next time to read [s]
       short AC1; 
       short AC2; 
       short AC3; 
       short B1; 
       short B2; 
       short MB; 
       short MC; 
       short MD;
       unsigned short AC4; 
       unsigned short AC5; 
       unsigned short AC6;
};

// HTU21D settings
struct htu21d {
       int interval; // reading interval [s]
       int next; // next time to read [s]
       int softreset; // soft reset flag
       int drop; // in case of i2c error remove from loop
};

// MAG3110 settings
struct mag3110 {
       int interval; // reading interval [s]
       int next; // next time to read [s]
       unsigned char ctrlreg1; // control register 1
       unsigned char ctrlreg2; // control register 2
       short offx; // user offset x
       short offy; // user offset y
       short offz; // user offset z
       int tdelay; // delay before reading after trigger
       int tempint; // temperature reading interval [s]
       int toffset; // temperature offset
};

// MPL3115A2 settings
struct mpl3115a2 {
       int interval; // reading interval [s]
       int next; // next time to read [s]
       int altint; // altitude reading interval [s]
       unsigned short refpres; // reference pressure
};

// TMP102 settings
struct tmp102 {
       int interval; // reading interval [s]
       int next; // next time to read [s]
};

extern struct bmp180 bmp180c;
extern struct htu21d htu21dc;
extern struct mag3110 mag3110c;
extern struct mpl3115a2 mpl3115a2c;
extern struct tmp102 tmp102c1, tmp102c2, tmp102c3, tmp102c4;

extern const char *i2cdev; // i2c device
extern int loglev; // log level
extern int i2cerr; // error in i2c communication

extern const char *confile; // configuration file

extern int dbsqlite; // 1=store data to local SQLite database
extern char *dbfile; // SQLite database file

extern int dbmysql; // 1=store data to MySQL database
extern char *dbhost; // MySQL hostname
extern char *dbuser; // MySQL username
extern char *dbpswd; // MySQL password
extern char *database; // MySQL database
extern int dbport; // MySQL database port number

#endif

