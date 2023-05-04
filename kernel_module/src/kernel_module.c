#include <linux/kernel.h>       //piszemy czesc kernela
#include <linux/module.h>       //tutaj jest to modul
#include <linux/proc_fs.h>      //ktory ma obslugiwac PROCFS
#include <linux/uaccess.h>
#include <asm/uaccess.h>        //i posluguje sie: copy_from_user() i copy_to_user()
#include <asm/io.h>             //a calosc komunikowac sie bedzie z peryferiami: writel() i readl()

#define BUF_SIZE            100

#define MEM_BASE_ADDR       0x00100000
#define MEM_SIZE            16
#define FINISHER_FAIL       (0x3333)

#define OFFSET_FINISHER		(0x0000)    //FINISHER


// czujnik 1 odczyt
#define OFFSET_ss95el1		(0x230)

// czujnik 1 zapis
#define OFFSET_ss95el2		(0x234)

// czujik 2 odczyt
#define OFFSET_ss95el3		(0x238)

// czujnik 2 zapis
#define OFFSET_ss95el4		(0x23C)

// int odczyt
#define OFFSET_ss95el5	    (0x240)

// int zapis
#define OFFSET_ss95el6	    (0x244)



void __iomem *ptr_finisher;                //FINISHER

void __iomem *ptr_ss95el1_read;
void __iomem *ptr_ss95el3_read;
void __iomem *ptr_ss95el2_write;
void __iomem *ptr_ss95el4_write;
void __iomem *ptr_ss95el5_read;
void __iomem *ptr_ss95el6_write;

// funkcja do odczytu z czujnika 1
static ssize_t read_ss95el1(struct file *file, char __user *ubuf, size_t count, loff_t *offset)
{
    char buf[BUF_SIZE];
    int len=0;

    if(*offset>0 || count<BUF_SIZE){
        return 0;                           //czyli nic nie udalo sie skopiowac a nie jest to blad
    }

    len=snprintf(buf, BUF_SIZE, "%x\n", readl(ptr_ss95el1_read)); //odczyt danych z elementu peryferyjnego poprzez wskazanie baseptr_2
                                            //(w gpioemu.v jest tu podawana stala wartosc)
                                            //oraz konwersja odczytanej wartoosci do postaci tekstowej zgodnie z notacja HEX

    if(copy_to_user(ubuf, buf, len)){       //kopiowanie danych przestrzeni kernela do przestrzeni uzytkownika
        return -EFAULT;    
    }

    *offset = len;
    return len;
}

// funkcja do odczytu z czujnika 2
static ssize_t read_ss95el3(struct file *file, char __user *ubuf, size_t count, loff_t *offset)
{
    char buf[BUF_SIZE];
    int len=0;
    if(*offset>0 || count<BUF_SIZE){
        return 0;
    }
    len=snprintf(buf, BUF_SIZE, "%x\n", readl(ptr_ss95el3_read));
    if(copy_to_user(ubuf, buf, len)){
        return -EFAULT;    
    }
    *offset = len;
    return len;
}


// funkcja do odczytu z inta
static ssize_t read_ss95el5(struct file *file, char __user *ubuf, size_t count, loff_t *offset)
{
    char buf[BUF_SIZE];
    int len=0;
    if(*offset>0 || count<BUF_SIZE){
        return 0;
    }
    len=snprintf(buf, BUF_SIZE, "%x\n", readl(ptr_ss95el5_read));
    if(copy_to_user(ubuf, buf, len)){
        return -EFAULT;    
    }
    *offset = len;
    return len;
}


// funkcja do zapisu na czujnik 1
static ssize_t write_ss95el2(struct file *file, const char *ubuf, size_t count, loff_t *offset)
{
    unsigned int chbuf = 0;
    char buf[BUF_SIZE];
    if(*offset>0 || count>BUF_SIZE){
        return 0;
    }
    if(copy_from_user(buf, ubuf, count)){
        return -EFAULT;
    }
    buf[count] = '\0';
    sscanf(buf,"%x\n",&chbuf);
   
    writel(chbuf, ptr_ss95el2_write);

    return count;
}

// funkcja do zapisu na czujnik 2
static ssize_t write_ss95el4(struct file *file, const char *ubuf, size_t count, loff_t *offset)
{
    unsigned int chbuf = 0;
    char buf[BUF_SIZE];
    if(*offset>0 || count>BUF_SIZE){
        return 0;
    }
    if(copy_from_user(buf, ubuf, count)){
        return -EFAULT;
    }
    buf[count] = '\0';
    sscanf(buf,"%x\n",&chbuf);
  
    writel(chbuf, ptr_ss95el4_write);

    return count;
}


// funkcja do zapisu na inta
static ssize_t write_ss95el6(struct file *file, const char *ubuf, size_t count, loff_t *offset)
{
    unsigned int chbuf = 0;
    char buf[BUF_SIZE];
    if(*offset>0 || count>BUF_SIZE){
        return 0;
    }
    if(copy_from_user(buf, ubuf, count)){
        return -EFAULT;
    }
    buf[count] = '\0';
    sscanf(buf,"%x\n",&chbuf);
    
        
    
    writel(chbuf, ptr_ss95el6_write);

    return count;
}




static struct file_operations fop_ss95el1_read = {
    .owner = THIS_MODULE,
    .read = read_ss95el1,
};
static struct file_operations fop_ss95el3_read = {
    .owner = THIS_MODULE,
    .read = read_ss95el3,
};
static struct file_operations fop_ss95el5_read = {
    .owner = THIS_MODULE,
    .read = read_ss95el5,
    
};


static struct file_operations fop_ss95el6_write = {
    .owner = THIS_MODULE,
    .write = write_ss95el6,
};
static struct file_operations fop_ss95el2_write = {
    .owner = THIS_MODULE,
    .write = write_ss95el2,
};
static struct file_operations fop_ss95el4_write = {
    .owner = THIS_MODULE,
    .write = write_ss95el4,
};



struct proc_dir_entry *parent;

static struct proc_dir_entry *ent_ss95el1_read;
static struct proc_dir_entry *ent_ss95el3_read;
static struct proc_dir_entry *ent_ss95el2_write;
static struct proc_dir_entry *ent_ss95el4_write;

static struct proc_dir_entry *ent_ss95el5_read;
static struct proc_dir_entry *ent_ss95el6_write;

static int procfs_init(void){
    printk(KERN_ALERT "Starting of the module...\n");

    parent = proc_mkdir("sykom", NULL);
    ent_ss95el1_read = proc_create("ss95el1", 0660, parent, &fop_ss95el1_read);
	ent_ss95el3_read = proc_create("ss95el3", 0660, parent, &fop_ss95el3_read);
	ent_ss95el5_read = proc_create("ss95el5", 0660, parent, &fop_ss95el5_read);

    
    ent_ss95el2_write = proc_create("ss95el2", 0660, parent, &fop_ss95el2_write);
	ent_ss95el4_write = proc_create("ss95el4", 0660, parent, &fop_ss95el4_write);
    ent_ss95el6_write = proc_create("ss95el6", 0660, parent, &fop_ss95el6_write);

    ptr_finisher = ioremap(MEM_BASE_ADDR+OFFSET_FINISHER, MEM_SIZE);
    
	  ptr_ss95el1_read = ioremap(MEM_BASE_ADDR+OFFSET_ss95el1, MEM_SIZE);
	  ptr_ss95el3_read = ioremap(MEM_BASE_ADDR+OFFSET_ss95el3, MEM_SIZE);
	  ptr_ss95el5_read = ioremap(MEM_BASE_ADDR+OFFSET_ss95el5, MEM_SIZE);
     
      ptr_ss95el6_write = ioremap(MEM_BASE_ADDR+OFFSET_ss95el6, MEM_SIZE);
	  ptr_ss95el2_write = ioremap(MEM_BASE_ADDR+OFFSET_ss95el2, MEM_SIZE);
	  ptr_ss95el4_write = ioremap(MEM_BASE_ADDR+OFFSET_ss95el4, MEM_SIZE);

    return 0;
}


static void procfs_cleanup(void){
    printk(KERN_WARNING "Stoping of the module...\n");

    proc_remove(ent_ss95el1_read);
	proc_remove(ent_ss95el3_read);
    proc_remove(ent_ss95el2_write);
	proc_remove(ent_ss95el4_write);
	proc_remove(ent_ss95el6_write);
    proc_remove(ent_ss95el5_read);
    remove_proc_entry("sykom", NULL);

    writel(FINISHER_FAIL | ((127)<<16), ptr_finisher);
}

module_init(procfs_init);
module_exit(procfs_cleanup);

MODULE_INFO(intree, "Y");
MODULE_LICENSE("based license");
MODULE_AUTHOR("Aleksander Pruszkowski");
MODULE_DESCRIPTION("PROCFS kernel module prepared for SYKOM lecture");
