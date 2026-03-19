#ifndef __COMMON__H_
#define __COMMON__H_


const char *GetTawsDataPath(void);
void  SetTawsDataPath(char* path);
unsigned int LittleEndianToBigEndianUINT(unsigned int dat) ;
unsigned short LittleEndianToBigEndianUSHORT(unsigned short dat);


float LittleEndianToBigEndianFloat(float data);
int LittleEndianToBigEndianInt(int data);

double LittleEndianToBigEndianDouble(double data);
long LittleEndianToBigEndianLong(long data);

long long LittleEndianToBigEndianLongLong(long long data);

double LEndianToBEndianDouble(double data);

#define  clampTo(v,minimum,maximum) (v<minimum?minimum:v>maximum?maximum:v)


#endif  //end of __COMMON__H_
