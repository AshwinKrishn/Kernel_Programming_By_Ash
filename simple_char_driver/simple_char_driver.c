/*
* File Name   :    simple_char_driver.c			 	
* Author Name :    Ashwin.K		 				
* E-Mail Addr :    ashwinstringed@gmail.com 				
* DATE        :    2019-02-08 
* Purpose     :    A simple char driver  
*/

#include <linux/module.h>   // Kernel module based definitions
#include <linux/init.h>     // Intialization and exit funtionalities macro defines import
#include <linux/fs.h>       // File system operaions
#include <linux/uaccess.h>  // User to kernel interactions
#include <linux/cdev.h>	    // char device includes
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>    // kmalloc requiers this

static dev_t dev;	    // used for creating major and minor numeber
static char *  buffer;
static int buffer_size = 256;
static struct class * class_in_dev;    // class for creating /dev/class entry
static struct cdev ash_device;	     // to inform udev to add entries in /dev/class

/*

*/
static ssize_t simple_chardrv_read(struct file * fil,char __user * user_buffer , size_t size,loff_t * offset){
	if(size <= buffer_size){
		copy_to_user(user_buffer,buffer,size);	
		printk(KERN_INFO" Reading from Ash Module\n");
		return size;
	}
	return -1;
}

static ssize_t simple_chardrv_write(struct file * fil,const char  __user * user_buffer , size_t size , loff_t * offset){

	if(size <= buffer_size){
		copy_from_user(buffer,user_buffer,size);	
		printk(KERN_INFO" Writing to Ash Module\n");
		return size;
	}
	return -1;
}

static int simple_chardrv_open(struct inode * node , struct file * files){

	printk(KERN_INFO" Ash Module opened\n");
	return 0;
}

static int simple_chardrv_release(struct inode * node , struct file * files){

	printk(KERN_INFO" Ash Module closed \n");
	return 0;
}


static void __exit simple_chrdrv_exit(void){

	/*
	  While exiting it's our duty to love the kernel and
	  clean up our device and release all the resources
	  we requested and kernel was nice enough to provide us.
	*/
	cdev_del(&ash_device);
	device_destroy(class_in_dev,dev);
        class_destroy(class_in_dev);
        unregister_chrdev_region(dev,3);
	
	printk(KERN_INFO "Ash module removed\n" );
}

static struct file_operations my_file_ops= {
		.open = simple_chardrv_open,
		.release = simple_chardrv_release,
		.write = simple_chardrv_write,
		.read  = simple_chardrv_read,

};
static int __init simple_chrdrv_init(void){
	int ret = 0;
	struct device *dev_ret;
	kmalloc(buffer_size , GFP_KERNEL );	
	printk(KERN_INFO" Ash Module inserted\n ");
	/* 
	   This step is for allocating the major numbers dynamically and
	   informing the range of minor numbers to be assosiated with the 
	   major number
	*/	
	ret = alloc_chrdev_region(&dev,0,3,"Ash_Char");
	if(ret != 0 ){
		printk(KERN_INFO "allocation failed");
	}
	/*
	   This is for creating a device class in /sys/class 
	   Classes of devices are similar to classes of car
	   SUV, Sedan , Hatchback etc...
	   Similarly Char,Block , Net devices etc...
	*/
	class_in_dev = class_create(THIS_MODULE , "ash_char");
	if( IS_ERR(class_in_dev)){
		unregister_chrdev_region(dev,3);
		return -1;
	}
	/*
	  The device_create API is used for creating the instances in /dev/<your_device>
	  Only one device is created per call. Incase of creating multiple devices , loop 
	  through the count and create multiple instances
	*/
	if(IS_ERR(dev_ret = device_create(class_in_dev,NULL,dev,NULL,"ash_chrdrv"))){
		class_destroy(class_in_dev);
	        unregister_chrdev_region(dev, 3);
       		return PTR_ERR(dev_ret);


	}
	/*
	  The cdev_init funtion takes a cdev type instance 
	  and populates the structure with vital information
	  to make it eligible to be added as a char device in 
	  kernel. 
	  We provide the file operation strucutre , that fit
	  into the kernel and gets called when the corresponding
	   syscall is issued from user space.  
	*/
	cdev_init(&ash_device,&my_file_ops);	// initializa the cdev instance	
	if((ret = cdev_add(&ash_device,dev,3)) < 0){
		printk(KERN_INFO "cdev_add failed with error code %d \n",ret);
		device_destroy(class_in_dev,dev);
		class_destroy(class_in_dev);
		unregister_chrdev_region(dev,3);
		return ret;
	}
	
	printk(KERN_INFO "Major number is %d and minor is %d\n",MAJOR(dev),MINOR(dev));
	return ret;
}
module_init(simple_chrdrv_init);	// Inform kernel this is entry funtion
module_exit(simple_chrdrv_exit);	// Inform kernel this is exit routine

MODULE_LICENSE("GPL");		// If not specified , then you wont get suport from community
MODULE_AUTHOR("Ashwin Savepo <ashwinstringed@gmail.com>"); //  Your name
MODULE_DESCRIPTION("A simple character driver for practice");	// This will be showed when asked for modinfo
