#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <asm/errno.h>
#include <asm/io.h>

MODULE_INFO(intree, "Y");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aleksander Pruszkowski");
MODULE_DESCRIPTION("Simple kernel module for SYKT/SYKOM lecture");
MODULE_VERSION("0.01");

#define SYKT_MEM_BASE_ADDR (0x00100000)
#define SYKT_MEM_SIZE (0x8000)
#define SYKT_EXIT (0x3333)
#define SYKT_EXIT_CODE (0x7F)

#define SYKT_QEMU_CTRL_ADDR (0x00100000)
#define SYKT_QEMU_EXIT_VAL (0x00005555)

#define SYKT_GPIO_ADDR_SPACE (baseptr)
#define SYKT_GPIO_CTRL_ADDR (SYKT_GPIO_ADDR_SPACE+0x00000810)
#define SYKT_GPIO_ARG1_ADDR (SYKT_GPIO_ADDR_SPACE+0x00000820)
#define SYKT_GPIO_ARG2_ADDR (SYKT_GPIO_ADDR_SPACE+0x00000830)
#define SYKT_GPIO_RESULT_ADDR (SYKT_GPIO_ADDR_SPACE+0x00000840)
#define SYKT_GPIO_STATUS_ADDR (SYKT_GPIO_ADDR_SPACE+0x00000850)

#define DONE_BIT (0x00000001)

void __iomem *baseptr;
static struct kobject *kobj_ref;

static int gpf2ctrlValue;
static int gpf2we1Value;
static int gpf2we2Value;
static int gpf2wyValue;
static int gpf2statusValue;

// ================= funkcje do komunikacji ==========================================================
// odczyt argumentu arg1 i zapis na odpowiednie miejsce w pamięci
static ssize_t gpf2we1_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
        sscanf(buf,"%x",&gpf2we1Value);
		writel(gpf2we1Value, SYKT_GPIO_ARG1_ADDR);
        return count;
}
// odczyt argumentu arg2 i zapis na odpowiednie miejsce w pamięci
static ssize_t gpf2we2_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
        sscanf(buf,"%x",&gpf2we2Value);
		writel(gpf2we2Value, SYKT_GPIO_ARG2_ADDR);
        return count;
}
// odczyt wyniku z modułu
static ssize_t gpf2wy_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	gpf2wyValue = readl(SYKT_GPIO_RESULT_ADDR);
    return sprintf(buf, "%x", gpf2wyValue);
}
// odczyt statusu (by zacząć pracę modułu)
static ssize_t gpf2status_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
        sscanf(buf,"%x",&gpf2ctrlValue);
		writel(gpf2ctrlValue, SYKT_GPIO_CTRL_ADDR);
        return count;
}
// odczyt statusu (czy moduł skończył działanie)
static ssize_t gpf2status_show(struct kobject *kobj, 
                struct kobj_attribute *attr, char *buf)
{
	gpf2statusValue = readl(SYKT_GPIO_STATUS_ADDR);
    return sprintf(buf, "%x", gpf2statusValue);
}

// makra do komunikacji
static struct kobj_attribute gpf2we1_attr = __ATTR_WO(gpf2we1);
static struct kobj_attribute gpf2we2_attr = __ATTR_WO(gpf2we2);
static struct kobj_attribute gpf2wy_attr = __ATTR_RO(gpf2wy);
static struct kobj_attribute gpf2status_attr = __ATTR_RW(gpf2status);

// ============================================================================================
int my_init_module(void){
	printk(KERN_INFO "Init my sykt module.\n");
	baseptr=ioremap(SYKT_MEM_BASE_ADDR, SYKT_MEM_SIZE);
	kobj_ref = kobject_create_and_add("sykt",kernel_kobj);

	if(sysfs_create_file(kobj_ref,&gpf2we1_attr.attr)){
    	printk(KERN_INFO"Cannot create sysfs file......\n");
	}

	if(sysfs_create_file(kobj_ref,&gpf2we2_attr.attr)){
    	printk(KERN_INFO"Cannot create sysfs file......\n");
		sysfs_remove_file(kernel_kobj, &gpf2we1_attr.attr);
	}

	if(sysfs_create_file(kobj_ref,&gpf2wy_attr.attr)){
    	printk(KERN_INFO"Cannot create sysfs file......\n");
		sysfs_remove_file(kernel_kobj, &gpf2we1_attr.attr);
		sysfs_remove_file(kernel_kobj, &gpf2we2_attr.attr);
	}

	if(sysfs_create_file(kobj_ref,&gpf2status_attr.attr)){
    	printk(KERN_INFO"Cannot create sysfs file......\n");
		sysfs_remove_file(kernel_kobj, &gpf2we2_attr.attr);
		sysfs_remove_file(kernel_kobj, &gpf2wy_attr.attr);
		sysfs_remove_file(kernel_kobj, &gpf2status_attr.attr);
	}
	return 0;
}

void my_cleanup_module(void){
	printk(KERN_INFO "Cleanup my sykt module.\n");
	writel(SYKT_EXIT | ((SYKT_EXIT_CODE)<<16), baseptr);
	kobject_put(kobj_ref); 
	sysfs_remove_file(kernel_kobj, &gpf2we1_attr.attr);
	sysfs_remove_file(kernel_kobj, &gpf2we2_attr.attr);
	sysfs_remove_file(kernel_kobj, &gpf2wy_attr.attr);
	sysfs_remove_file(kernel_kobj, &gpf2status_attr.attr);
	iounmap(baseptr);
}

module_init(my_init_module)
module_exit(my_cleanup_module)