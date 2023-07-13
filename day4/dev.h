#ifndef DEV_H 
#define DEV_H
#include <linux/ioctl.h> 
#define MAGIC_NUM 0xF1 
#define IOCTL_SET_MSG _IO(MAGIC_NUM, 0) //_IO
#define IOCTL_GET_MSG _IOR(MAGIC_NUM, 1, int *) //получение сообщения драйвера устройства
#define DEVICE_FILE_NAME "orochy" 
#define DEVICE_PATH "/dev/orochy" 
#endif DEV_H
