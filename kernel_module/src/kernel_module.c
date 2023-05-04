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
#define SYKT_QEMU_EXIT_VAL (0x00007777)
 


#define SYKT_GPIO_ADDR_SPACE (baseptr) 
#define SYKT_GPIO_CTRL_ADDR (SYKT_GPIO_ADDR_SPACE+0x0000037F0)
#define SYKT_GPIO_ARG1_ADDR (SYKT_GPIO_ADDR_SPACE+0x00000388)
#define SYKT_GPIO_ARG2_ADDR (SYKT_GPIO_ADDR_SPACE+0x00000830)
#define SYKT_GPIO_RESULT_ADDR (SYKT_GPIO_ADDR_SPACE+0x00000390)
#define SYKT_GPIO_ONES_ADDR (SYKT_GPIO_ADDR_SPACE+0x00000398)
#define SYKT_GPIO_STATUS_ADDR (SYKT_GPIO_ADDR_SPACE+0x000003A0)

#define DONE_BIT (0x00000001)

void __iomem *baseptr;
static struct kobject *kobj_ref;
static int gpf2ctrl;
static int gpf2we1;
static int gpf2we2;
static int gpf2ones;
static int gpf2wy;
static int gpf2status;
// ================= funkcje do komunikacji ===========================
// odczyt argumentu arg1 i zapis na odpowiednie miejsce w pamięci // potrzebuje do zapisu tylko 2 znaczeńczyli tylko argumetu 1 i argumentu 2
static ssize_t gpf2we1_store(struct kobject *kobj,struct kobj_attribute *attr,const char *buf, size_t count)
{
sscanf(buf,"%x",&gpf2we1);
writel(gpf2we1, SYKT_GPIO_ARG1_ADDR);
return count;
}
// odczyt argumentu arg2 i zapis na odpowiednie miejsce w pamięci
static ssize_t gpf2we2_store(struct kobject *kobj,struct kobj_attribute *attr,const char *buf, size_t count)
{
sscanf(buf,"%x",&gpf2we2);
writel(gpf2we2, SYKT_GPIO_ARG2_ADDR);
return count;
}



// odczyt wyniku z modułu
static ssize_t gpf2wy_show(struct kobject *kobj,struct kobj_attribute *attr, char *buf)
{
gpf2wy = readl(SYKT_GPIO_RESULT_ADDR);
return sprintf(buf, "%x", gpf2wy);
}


static ssize_t gpf2status_store(struct kobject *kobj,struct kobj_attribute *attr,const char *buf, size_t count)   //  ten fragment trzeba będzie pewnie poprawić bo on wczytuje status  od użytkownika
{
sscanf(buf,"%x",&gpf2ctrl);     //czemu tu jest tak a nie inaczej?
writel(gpf2ctrl, SYKT_GPIO_CTRL_ADDR);
return count;
}

static ssize_t gpf2ones_show(struct kobject *kobj,struct kobj_attribute *attr, char *buf)
{
    gpf2ones = readl(SYKT_GPIO_ONES_ADDR);
    return sprintf(buf, "%x", gpf2ones);
}

// odczyt statusu (czy moduł skończył działanie)
static ssize_t gpf2status_show(struct kobject *kobj,struct kobj_attribute *attr, char *buf)

{
gpf2status = readl(SYKT_GPIO_STATUS_ADDR);
return sprintf(buf, "%x", gpf2status);
}
// makra do komunikacji

static struct kobj_attribute gpf2we1_attr = __ATTR_WO(gpf2we1);
static struct kobj_attribute gpf2we2_attr = __ATTR_WO(gpf2we2);
static struct kobj_attribute gpf2wy_attr = __ATTR_RO(gpf2wy);
static struct kobj_attribute gpf2ones_attr = __ATTR_RO(gpf2ones);
static struct kobj_attribute gpf2status_attr = __ATTR_RW(gpf2status);

// ===================================================================



//////////////////////////////////////////////
int my_init_module(void){
printk(KERN_INFO "Init my sykt module.\n");
baseptr=ioremap(SYKT_MEM_BASE_ADDR, SYKT_MEM_SIZE);
kobj_ref = kobject_create_and_add("sykt",kernel_kobj);

if (sysfs_create_file(kobj_ref, &gpf2we1_attr.attr))
{
    printk(KERN_INFO "Cannot create sysfs file......\n");
}
if (sysfs_create_file(kobj_ref, &gpf2we2_attr.attr))
{
    printk(KERN_INFO "Cannot create sysfs file......\n");
    sysfs_remove_file(kernel_kobj, &gpf2we1_attr.attr);
}
if (sysfs_create_file(kobj_ref, &gpf2wy_attr.attr))
{
    printk(KERN_INFO "Cannot create sysfs file......\n");
    sysfs_remove_file(kernel_kobj, &gpf2we1_attr.attr);
    sysfs_remove_file(kernel_kobj, &gpf2we2_attr.attr);
}
if (sysfs_create_file(kobj_ref, &gpf2ones_attr.attr))
{
    printk(KERN_INFO "Cannot create sysfs file......\n");
    sysfs_remove_file(kernel_kobj, &gpf2we1_attr.attr);
    sysfs_remove_file(kernel_kobj, &gpf2we2_attr.attr);
    sysfs_remove_file(kernel_kobj, &gpf2wy_attr.attr);
}
if (sysfs_create_file(kobj_ref, &gpf2status_attr.attr))
{
    printk(KERN_INFO "Cannot create sysfs file......\n");
    sysfs_remove_file(kernel_kobj, &gpf2we1_attr.attr);
    sysfs_remove_file(kernel_kobj, &gpf2we2_attr.attr);
    sysfs_remove_file(kernel_kobj, &gpf2wy_attr.attr);
    sysfs_remove_file(kernel_kobj, &gpf2ones_attr.attr);
}

return 0;

////////////////////////////////////////
}
void my_cleanup_module(void)
{
printk(KERN_INFO "Cleanup my sykt module.\n");
writel(SYKT_EXIT | ((SYKT_EXIT_CODE) << 16), baseptr);
kobject_put(kobj_ref);
sysfs_remove_file(kernel_kobj, &gpf2we1_attr.attr);
sysfs_remove_file(kernel_kobj, &gpf2we2_attr.attr);
sysfs_remove_file(kernel_kobj, &gpf2wy_attr.attr);
sysfs_remove_file(kernel_kobj, &gpf2ones_attr.attr);
sysfs_remove_file(kernel_kobj, &gpf2status_attr.attr);
iounmap(baseptr);
}

module_init(my_init_module)
module_exit(my_cleanup_module)



