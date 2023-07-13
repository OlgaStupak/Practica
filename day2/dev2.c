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

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

/*
static struct {
    uint8_t *buffer;
    size_t   buffer_size;
    size_t   head;
    size_t   tail;
    size_t   bytes_avail;
} queue_t; 

bool put(queue_t *q, uint8_t *data, size_t size) {
    if(q->buffer_size - q->bytes_avail < size){
        return false;
    }
    for(size_t i = 0; i < size; i++){
        q->buffer[(q->tail + i) % q->buffer_size] = data[i];
    }
    q->tail = (q->tail + size) % q->buffer_size;
    q->bytes_avail += size;
    return true;
}

bool get(queue_t *q, uint8_t *data, size_t size) {
    if(q->bytes_avail < size){
        return false;
    }
    for(size_t i = 0; i < size; i++){
        data[i] = q->buffer[(q->head + i) % q->buffer_size];
    }
    q->head = (q->head + size) % q->buffer_size;
    q->bytes_avail -= size;
    return true;
}
*/
static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

static int dev_init(void){
   printk(KERN_INFO "Initializing the EBBChar LKM\n");

   Major = register_chrdev(0, DEVICE_NAME, &fops);
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
   int error_count = 0;
   error_count = copy_to_user(buffer, message, size_of_message);
 
   if (error_count==0){      
      printk(KERN_INFO "Sent %d characters to the user\n", size_of_message);
      return (size_of_message=0);  
   }
   else {
      printk(KERN_INFO "Failed to send %d characters to the user\n", error_count);
      return -EFAULT;          
   }
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   sprintf(message, "%s(%zu letters)", buffer, len);   
   size_of_message = strlen(message);                
   printk(KERN_INFO "Received %zu characters from the user\n", len);
   return len;
}

static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "Device successfully closed\n");
   return 0;
}

module_init(dev_init);
module_exit(dev_exit);
