//////////////////////////////////////////////////
// Name        Ttl.c
// Author      zhaofei
// Version
// Copyright   Your copyright notice
// Description Test the testko-driver with ioctl ()
//-----------

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>

typedef struct{
     unsigned long dwMethodName;
     unsigned long dwData;
} testko_acpi_t;


// The following declarer must be same with the definiation in the testko-deriver, c
#define TESTKO_MAGIC 'B'
#define TESTKO_SET_WWAN_STATUS _IOWR(TESTKO_MAGIC, 0, testko_acpi_t)

int main()
{
     long lr;
     //unsigned long It ;
     long fd = open("/dev/Testko", O_RDWR);
     testko_acpi_t output;
     //It = '00NS';
     if ( 0!= fd ) {

          lr = ioctl(fd, TESTKO_SET_WWAN_STATUS, &output);
          if(!lr)
          {
               printf ("Successful ioctrl !\n");
          }
          else
          {
               printf ("Fail ioctrl !\n");
          }


     }
     return 0;
}
