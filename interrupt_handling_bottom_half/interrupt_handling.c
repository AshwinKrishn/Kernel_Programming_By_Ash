#include <linux/module.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>

static int count,device_id =100;
static irqreturn_t ash_handler(int value,void * data)
{
	count++;
	printk(KERN_INFO"Called %d times\n",count);
	return IRQ_NONE;	
}

static int __init ash_module_init(void)
{

	printk(KERN_INFO"Module initialized\n");

	if(request_irq(12,ash_handler,IRQF_SHARED,"Ashwin's interrupt",&device_id)){
		return -1;
	}
	pr_info("device_id = %d\n", device_id);
	printk (KERN_INFO "Successfully loading ISR handler\n");
	return 0;
}

static void __exit ash_module_exit(void)
{
	free_irq(12, &device_id);
	printk(KERN_INFO"module removed\n");
}

module_init(ash_module_init);
module_exit(ash_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ashwin Savepo <ashwinstringed@gmail.com>");
MODULE_DESCRIPTION("Catching touchpad interrupts");
