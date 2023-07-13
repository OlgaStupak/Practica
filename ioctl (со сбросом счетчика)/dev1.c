#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/wait.h>
#include <linux/ioctl.h>
#include "dev.h"
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/init.h>                 
#include <linux/device.h>       
   
#define  DEVICE_NAME "orochy"    
#define  CLASS_NAME  "oro"        
#define SIZE_BUF 256 
 
MODULE_LICENSE("GPL");                 
 
static int Major;                  
static char message[256] = {0};          
static short size_of_message;           
static int numberOpens = 0;              
static struct class* devClass  = NULL; 
static struct device* devDevice = NULL; 

static int counter = 0;
struct task_struct *ts;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static long dev_ioctl( struct file *, unsigned int cmd, unsigned long arg);

static struct file_operations fops =
{
   .open = dev_open,
   .release = dev_release,
   .unlocked_ioctl = dev_ioctl 
};

static int thread(void *data)
{
   while(1)
   {
      counter++;
      msleep(100);
      if (kthread_should_stop())
         break;
   }
   return counter;
}

static int dev_init(void){

   printk(KERN_INFO "Initializing the EBBChar LKM\n");
   
   ts = kthread_run(thread, NULL, "oro kthread");

   Major = register_chrdev(0, DEVICE_NAME, &fops); //obtaining the device number
   if (Major<0){
      printk(KERN_ALERT "Registering char device failed with %d\n", Major);
      return Major;
   }
   printk(KERN_INFO "Registered correctly with major number %d\n", Major);

   devClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(devClass)){                
      unregister_chrdev(Major, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(devClass);          
   }
   printk(KERN_INFO "Device class registered correctly\n");

   devDevice = device_create(devClass, NULL, MKDEV(Major, 0), NULL, DEVICE_NAME);
   if (IS_ERR(devDevice)){               
      class_destroy(devClass);           
      unregister_chrdev(Major, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(devDevice);
   }
   printk(KERN_INFO "Device class created correctly\n"); 
   return 0;
}   

static long dev_ioctl(struct file *filep,  unsigned int cmd, unsigned long arg)
{
   switch (cmd)
   {
      case IOCTL_SET_MSG:
         counter = 0;
         printk(KERN_INFO "Counter: %d \n", counter);
         break;
      case IOCTL_GET_MSG:
         copy_to_user((int*) arg, &counter, sizeof(int));
         printk(KERN_INFO "Counter: %d \n", &counter);
         break;
   }
   return counter;//?
}

static void dev_exit(void){
   kthread_stop(ts);
   device_destroy(devClass, MKDEV(Major, 0));     
   class_unregister(devClass);                         
   class_destroy(devClass);                           
   unregister_chrdev(Major, DEVICE_NAME);            
   printk(KERN_INFO "Goodbye from the LKM!\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   printk(KERN_INFO "Device has been opened %d time(s)\n", numberOpens);
   return 0;
}

static int dev_release(struct inode *inodep, struct file *filep){
   numberOpens--;
   printk(KERN_INFO "Device successfully closed\n");
   return 0;
}

module_init(dev_init);
module_exit(dev_exit);
