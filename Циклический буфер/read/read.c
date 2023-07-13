#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
 
#define BUFFER_LENGTH 256               
static char receive[BUFFER_LENGTH];    
 
int main(){
   int ret, fd;
   char stringToSend[BUFFER_LENGTH];
   fd = open("/dev/orochy", O_RDWR);       
   if (fd < 0){
      perror("Failed to open the device...");
      return errno;
   }

   printf("Reading from the device...\n");
   ret = read(fd, receive, BUFFER_LENGTH);       
   if (ret < 0){
      perror("Failed to read the message from the device.");
      return errno;
   }
   
   printf("End of the program\n");
   return 0;
}
