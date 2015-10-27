#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h> 
#include <asm/uaccess.h> 

#define MAJOR_NUM 1024 //主设备号

int global_var = 0; //"globalvar"设备的全局变量
EXPORT_SYMBOL(global_var);

static struct class *globalvar_class;
static struct device *globalvar_dev;
static ssize_t globalvar_read(struct file *, char *, size_t, loff_t*);
static ssize_t globalvar_write(struct file *, const char *, size_t, loff_t*);

static ssize_t globalvar_read(struct file *filp, char *buf, size_t len, loff_t *off)
{  
      if (copy_to_user(buf, &global_var, sizeof(int))){ //将global_var从内核空间复制到用户空间
            return - EFAULT;
      } 
      printk("globalvar_read enter ,the data is %d\n",global_var);
      return sizeof(int);
}

static ssize_t globalvar_write(struct file *filp, const char *buf, size_t len, loff_t *off)
{
      if (copy_from_user(&global_var, buf, sizeof(int))){//将用户空间的数据复制到内核空间的global_var
            return - EFAULT;
      } 
      printk("globalvar_write enter ,the data is %d\n",global_var);
      return sizeof(int);
}

//初始化字符设备驱动的file_operations结构体
struct file_operations globalvar_fops =
{
      //read: globalvar_read, write: globalvar_write,
      .write  = globalvar_write,
      .read =globalvar_read,
};

static int __init globalvar_init(void){
      int ret;
      
      ret = register_chrdev(MAJOR_NUM, "globalvar", &globalvar_fops); //注册设备驱动
      if (ret){
            printk("globalvar register failure");
      }
      else{
            printk("globalvar register success");
      }
      globalvar_class = class_create(THIS_MODULE, "globalvar");
      globalvar_dev = device_create(globalvar_class, NULL, MKDEV(MAJOR_NUM, 0), NULL, "globalvar");
      return ret;
}
//注销设备驱动
static void __exit globalvar_exit(void)
{
       unregister_chrdev(MAJOR_NUM, "globalvar");
}
module_init(globalvar_init);
module_exit(globalvar_exit);
MODULE_LICENSE("GPL");
