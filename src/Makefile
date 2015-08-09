# -*- mode: makefile -*-
# 
# Makefile for compiling 'i2chipd' on Raspberry Pi. 
#
# Sat Jul  4 13:26:36 CEST 2015
# Edit: 
# Jaakko Koivuniemi

CXX           = gcc
CXXFLAGS      = -g -O -Wall -I/usr/include/mysql/ 
LD            = gcc
LDFLAGS       = -L/usr/lib/mysql -lmysqlclient -lsqlite3 -O

MODULES      = I2cReadBytes.o 
MODULES     += I2cReadRegByte.o 
MODULES     += I2cReadRegVector.o 
MODULES     += I2cRReadRegBytes.o
MODULES     += I2cWriteCommand.o
MODULES     += I2cWriteCommandRead3.o
MODULES     += I2cWriteRegister.o
MODULES     += I2cWriteRegVector.o
MODULES     += InitBMP180.o
MODULES     += InitHtu21d.o
MODULES     += InitMag3110.o
MODULES     += InitMpl3115a2.o
MODULES     += InitTmp102_1.o 
MODULES     += InitTmp102_2.o 
MODULES     += InitTmp102_3.o 
MODULES     += InitTmp102_4.o 
MODULES     += InsertMySQL.o 
MODULES     += InsertSQLite.o
MODULES     += ReadBMP180.o
MODULES     += ReadConfig.o
MODULES     += ReadHtu21d.o
MODULES     += ReadMag3110.o
MODULES     += ReadMpl3115a2.o
MODULES     += ReadMySQLTime.o
MODULES     += ReadSQLiteTime.o
MODULES     += ReadTmp102_1.o
MODULES     += ReadTmp102_2.o
MODULES     += ReadTmp102_3.o
MODULES     += ReadTmp102_4.o
MODULES     += WriteFile.o 
MODULES     += i2chipd.o

%.o : %.c
	$(CXX) $(CXXFLAGS) -c $<

all: i2chipd 

i2chipd: $(MODULES) 
	$(LD) $(LDFLAGS) $^ -o $@

clean:
	rm -f *.o

