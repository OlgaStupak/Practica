#include <linux/init.h>         
#include <linux/module.h>         
#include <linux/device.h>       
#include <linux/kernel.h>     
#include <linux/fs.h>            
#include <linux/uaccess.h>  
#include <linux/wait.h>       
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

static char flag = 'n';
//static DECLARE_WAIT_QUEUE_HEAD(wq);
static wait_queue_head_t wq_write;
static wait_queue_head_t wq_read;
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

   init_waitqueue_head(&wq_write);
   init_waitqueue_head(&wq_read);
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

	if((count_read+1 == count_write) //buffer empty
		&& (filep->f_flags & O_NONBLOCK))
		return -EAGAIN;
	if(wait_event_interruptible(wq_read, (count_read+1 != count_write)))
		return -ERESTARTSYS;
	int bytes_read = 0;
	while(bytes_read < len && (count_read+1 != count_write) )
	{
		put_user(cycleBuf[++count_read], buffer++);
		printk(KERN_INFO"read: readc %d, writec %d, buf[read] %c\n", count_read, count_write, cycleBuf[count_read]);
		bytes_read++;
		if(count_read == SIZE_BUF-1)
			count_read = -1;
	}
	wake_up_interruptible(&wq_write);
	
	printk(KERN_INFO"read: length %d, bytes %d\n", len, bytes_read);
	return bytes_read;
	
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   /*copy_from_user(message, buffer, size_of_message);
   message[size_of_message]=0;
   return size_of_message;*/
	
	if((count_read == count_write) //buffer if full
		&&(filep->f_flags & O_NONBLOCK))
		return -EAGAIN;
	if(wait_event_interruptible(wq_write, (count_read != count_write)))
		return -ERESTARTSYS;
	
	int bytes_written = 0;
	while( bytes_written < len && (count_write != count_read) )
	{
		printk(KERN_INFO"write1: readc %d, writec %d\n", count_read, count_write);
		get_user(cycleBuf[count_write++], buffer++);
		bytes_written++;
		if(count_write == SIZE_BUF)
			count_write = 0;
		printk(KERN_INFO"write2: readc %d, writec %d\n", count_read, count_write);
	}
	wake_up_interruptible(&wq_read);
	
	return bytes_written;
}

static int dev_release(struct inode *inodep, struct file *filep){
   numberOpens--;
   printk(KERN_INFO "Device successfully closed\n");
   return 0;
}

module_init(dev_init);
module_exit(dev_exit);
