#include <linux/init.h>         
#include <linux/module.h>         
#include <linux/device.h>       
#include <linux/kernel.h>     
#include <linux/fs.h>            
#include <linux/uaccess.h>         
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

static char cycleBuf[SIZE_BUF];
static int count_write = 1;
static int count_read = 0;
//static char tail = 0; 
//static char head = 0; 
//static char count = 0;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

static int dev_init(void){
   printk(KERN_INFO "Initializing the EBBChar LKM\n");

   Major = register_chrdev(0, DEVICE_NAME, &fops); //obtaining the device number
   if (Major<0){
      printk(KERN_ALERT "Registering char device failed with %d\n", Major);
      return Major;
   }
   printk(KERN_INFO "Registered correctly with major number %d\n", Major);

   devClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(devClass)){                
      unregister_chrdev(Major, DEVICE_NAME);//liberation the device number
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

static void dev_exit(void){
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

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   /*copy_to_user(buffer, message, size_of_message);
   return size_of_message;*/
   int bytes_read = 0;
	while(len && (count_read+1 != count_write) )
	{
		put_user(cycleBuf[++count_read], buffer++);
		len--;
		bytes_read++;
		if((count_read - SIZE_BUF) == 1)
			count_read = 0;
	}
	printk(KERN_INFO "length= %d, bytes_read= %d, write_c= %d, read_c= %d, userbufchar= %c", len, bytes_read, count_write, count_read, buffer);

	return bytes_read;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   /*copy_from_user(message, buffer, size_of_message);
   message[size_of_message]=0;
   return size_of_message;*/
   int bytes_written = 0;
   while(bytes_written < len && (count_write != count_read 
		|| (count_write == count_read && count_write == 0)) )
	{
		get_user(cycleBuf[count_write++], buffer++);
		bytes_written++;
		if((count_write - SIZE_BUF) == 0)
			count_write = 0;
		printk(KERN_INFO "length= %d, bytes_written=%d, write_c=%d, read_c= %d", len, bytes_written, count_write, count_read);
	}
	cycleBuf[count_write++] = '\0';
	printk(KERN_INFO "Buffer = %s \n", cycleBuf);
	return bytes_written;
}

static int dev_release(struct inode *inodep, struct file *filep){
   numberOpens--;
   printk(KERN_INFO "Device successfully closed\n");
   return 0;
}

module_init(dev_init);
module_exit(dev_exit);
