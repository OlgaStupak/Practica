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
   
   printf("Type in a short string to send to the kernel module:\n");
   scanf("%[^\n]%*c", stringToSend);                
   //printf("Writing message to the device [%s].\n", stringToSend);
   ret = write(fd, stringToSend, strlen(stringToSend));
   if (ret < 0){
      perror("Failed to write the message to the device.");
      return errno;
   }

   printf("End of the program\n");
   return 0;
}
