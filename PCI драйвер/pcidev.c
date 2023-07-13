#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/wait.h>
#include <linux/ioctl.h>
#include "dev.h"
#include <linux/kthread.h>
#include <linux/delay.h>                
#include <linux/device.h>

#define  DEVICE_NAME "orochy"    
#define  CLASS_NAME  "oro"

MODULE_LICENSE("GPL");

static struct pci_device_id pci_driver_id_table[] = {
	{PCI_DEVICE(0x8086, 0x100e)},//vendor_id and device_id
	{0,}
};

MODULE_DEVICE_TABLE(pci, pci_driver_id_table);

static unsigned int port_addr;
static unsigned int dev_size;
static unsigned char* dev_virtual;
//static int Major;                                                
//static struct class* devClass  = NULL; 
//static struct device* devDevice = NULL; 

static int pci_driver_probe(struct pci_dev *, const struct pci_device_id *);
static void pci_driver_remove(struct pci_dev *);
static long dev_ioctl( struct file *, unsigned int cmd, unsigned long arg);

static struct pci_driver ethernet_pci_driver = {
	.name = DEVICE_NAME,
	.id_table = pci_driver_id_table,
	.probe = pci_driver_probe,
	.remove = pci_driver_remove
};

/*static struct file_operations fops =
{
   //.unlocked_ioctl = dev_ioctl 
};*/

static int __init pci_init_module(void) {
	/*Major = register_chrdev(0, DEVICE_NAME, &fops); //obtaining the device number
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
	printk(KERN_INFO "Device class created correctly\n"); */
	
	printk(KERN_INFO "Init\n");
	return pci_register_driver(&ethernet_pci_driver);
}

static void __exit pci_cleanup_module(void) {
	
	/*device_destroy(devClass, MKDEV(Major, 0));     
	class_unregister(devClass);                         
	class_destroy(devClass);                           
	unregister_chrdev(Major, DEVICE_NAME);*/
	printk(KERN_INFO "Exit\n");
	pci_unregister_driver(&ethernet_pci_driver);
}

/*static long dev_ioctl(struct file *filep,  unsigned int cmd, unsigned long arg)
{
   switch (cmd)
   {
   
   }
   return 0;
}*/

int pci_driver_probe(struct pci_dev *dev, const struct pci_device_id *id) {
	unsigned int i = 0;
	unsigned int data = 0;
	port_addr = pci_resource_start(dev,0);
	dev_size = pci_resource_len(dev,0);
	dev_virtual=ioremap(port_addr,dev_size);
	printk(KERN_INFO "%lu ", dev_size);
	while(i < dev_size)
        {
            data = ioread8(&dev_virtual[i]);
            if(data == 0x08)
        {
            pr_info("data: %02x:%02x:%02x:%02x:%02x:%02x\n", 
            (unsigned int)data,//
            (unsigned int)ioread8(&dev_virtual[i+1]),
            (unsigned int)ioread8(&dev_virtual[i+2]),
            (unsigned int)ioread8(&dev_virtual[i+3]),
            (unsigned int)ioread8(&dev_virtual[i+4]),
            (unsigned int)ioread8(&dev_virtual[i+5]));
            pr_info("i = %d\n", i);
            break;
        }
        i++;
        }
	
	printk(KERN_INFO "Loaded\n");
	return 0;
}

void pci_driver_remove(struct pci_dev *dev) {
	printk(KERN_INFO "Removed\n");
}

module_init(pci_init_module);
module_exit(pci_cleanup_module);
