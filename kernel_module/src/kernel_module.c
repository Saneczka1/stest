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
#define SYKT_GPIO_ARG2_ADDR (SYKT_GPIO_ADDR_SPACE+0x00000388)
#define SYKT_GPIO_ARG1_ADDR (SYKT_GPIO_ADDR_SPACE+0x0000037F0)
#define SYKT_GPIO_RESULT_ADDR (SYKT_GPIO_ADDR_SPACE+0x00000390)
#define SYKT_GPIO_ONES_ADDR (SYKT_GPIO_ADDR_SPACE+0x00000398)
#define SYKT_GPIO_STATUS_ADDR (SYKT_GPIO_ADDR_SPACE+0x000003A0)

#define DONE_BIT (0x00000001)

void __iomem *baseptr;
static struct kobject *kobj_ref;
static int ctrlValue;
static int raba1;
static int raba2;
static int rabw;
static int rabl;
static int rabb;
// ================= funkcje do komunikacji ===========================
// odczyt argumentu arg1 i zapis na odpowiednie miejsce w pamięci // potrzebuje do zapisu tylko 2 znaczeńczyli tylko argumetu 1 i argumentu 2
static ssize_t raba1_store(struct kobject *kobj,struct kobj_attribute *attr,const char *buf, size_t count)
{
sscanf(buf,"%x",&raba1);
writel(raba1, SYKT_GPIO_ARG1_ADDR);
return count;
}
// odczyt argumentu arg2 i zapis na odpowiednie miejsce w pamięci
static ssize_t raba2_store(struct kobject *kobj,struct kobj_attribute *attr,const char *buf, size_t count)
{
sscanf(buf,"%x",&raba2);
writel(raba2, SYKT_GPIO_ARG2_ADDR);
return count;
}



// odczyt wyniku z modułu
static ssize_t rabw_show(struct kobject *kobj,struct kobj_attribute *attr, char *buf)
{
rabw = readl(SYKT_GPIO_RESULT_ADDR);
return sprintf(buf, "%x", rabw);
}




static ssize_t rabl_show(struct kobject *kobj,struct kobj_attribute *attr, char *buf)
{
    rabl = readl(SYKT_GPIO_ONES_ADDR);
    return sprintf(buf, "%x", rabl);
}

// odczyt statusu (czy moduł skończył działanie)
static ssize_t rabb_show(struct kobject *kobj,struct kobj_attribute *attr, char *buf)

{
rabw = readl(SYKT_GPIO_STATUS_ADDR);
return sprintf(buf, "%x", rabw);
}
// makra do komunikacji

static struct kobj_attribute raba1_attr = __ATTR_WO(raba1);
static struct kobj_attribute raba2_attr = __ATTR_WO(raba2);
static struct kobj_attribute rabw_attr = __ATTR_RO(rabw);
static struct kobj_attribute rabl_attr = __ATTR_RO(rabl);
static struct kobj_attribute rabb_attr = __ATTR_RW(rabb);

// ===================================================================



//////////////////////////////////////////////
int my_init_module(void){
printk(KERN_INFO "Init my sykt module.\n");
baseptr=ioremap(SYKT_MEM_BASE_ADDR, SYKT_MEM_SIZE);
kobj_ref = kobject_create_and_add("sykt",kernel_kobj);

if (sysfs_create_file(kobj_ref, &raba1_attr.attr))
{
    printk(KERN_INFO "Cannot create sysfs file......\n");
}
if (sysfs_create_file(kobj_ref, &raba2_attr.attr))
{
    printk(KERN_INFO "Cannot create sysfs file......\n");
    sysfs_remove_file(kernel_kobj, &raba1_attr.attr);
}
if (sysfs_create_file(kobj_ref, &rabw_attr.attr))
{
    printk(KERN_INFO "Cannot create sysfs file......\n");
    sysfs_remove_file(kernel_kobj, &raba1_attr.attr);
    sysfs_remove_file(kernel_kobj, &raba2_attr.attr);
}
if (sysfs_create_file(kobj_ref, &rabl_attr.attr))
{
    printk(KERN_INFO "Cannot create sysfs file......\n");
    sysfs_remove_file(kernel_kobj, &raba1_attr.attr);
    sysfs_remove_file(kernel_kobj, &raba2_attr.attr);
    sysfs_remove_file(kernel_kobj, &rabw_attr.attr);
}
if (sysfs_create_file(kobj_ref, &rabb_attr.attr))
{
    printk(KERN_INFO "Cannot create sysfs file......\n");
    sysfs_remove_file(kernel_kobj, &raba1_attr.attr);
    sysfs_remove_file(kernel_kobj, &raba2_attr.attr);
    sysfs_remove_file(kernel_kobj, &rabw_attr.attr);
    sysfs_remove_file(kernel_kobj, &rabl_attr.attr);
}

return 0;

////////////////////////////////////////
}
void my_cleanup_module(void)
{
printk(KERN_INFO "Cleanup my sykt module.\n");
writel(SYKT_EXIT | ((SYKT_EXIT_CODE) << 16), baseptr);
kobject_put(kobj_ref);
sysfs_remove_file(kernel_kobj, &raba1_attr.attr);
sysfs_remove_file(kernel_kobj, &raba2_attr.attr);
sysfs_remove_file(kernel_kobj, &rabw_attr.attr);
sysfs_remove_file(kernel_kobj, &rabl_attr.attr);
sysfs_remove_file(kernel_kobj, &rabb_attr.attr);
iounmap(baseptr);
}

module_init(my_init_module)
module_exit(my_cleanup_module)


