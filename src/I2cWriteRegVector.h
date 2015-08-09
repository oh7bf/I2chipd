#ifndef I2CWRITEREGVECTOR_H_INCLUDED
#define I2CWRITEREGVECTOR_H_INCLUDED
int I2cWriteRegVector(int address, unsigned char reg, short *xval, short *yval, short *zval);
#endif

