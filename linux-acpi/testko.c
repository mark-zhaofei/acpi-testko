#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/acpi.h>
#include <acpi/acpi_drivers.h>
#include <acpi/acpiosxf.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/string.h>

#define TESTKO_MAJOR 77
#define TESTKO_MINOR 2
#define TESTKO_NAME "Testko"

#define T493_WWAN_ACPI_NAME "\\_SB_.PCI0.RP07"
//"\\_SB.PCI0.RP07.PXSX"
#define T493_WWAN_ACPI_FULL_NAME "\\_SB.PCI0.RP07.PXSX"  //\_SB_.PCI0.RP07.PXSX
#define ACPI_DEVICE_PATH "\\_SB_.PCI0.RP07.PXSX"//"\\_SB.PCI0"
//#define ACPI_DEVICE_PATH ACPI_NS_ROOT_PATH
#define ACPI_PATHNAME_MAX 255

int T493_WWAN_ACPI_FOUND = 0;

MODULE_DESCRIPTION("MY KERNEL");
MODULE_AUTHOR("zhaofei");
MODULE_LICENSE("GPL");

#define TESTKO_MAGIC 'B'
#define TESTKO_SET_WWAN_STATUS _IOWR(TESTKO_MAGIC,0,testko_acpi_t)
//#define TESTKO_CONTROL_SN00 '00NS'

typedef struct {
  struct acpi_buffer *in;
  struct acpi_buffer *out;
}priv_data_t;


static int testko_open(struct inode *i, struct file *f)
{
  f->private_data = kmalloc(sizeof(priv_data_t),GFP_KERNEL);
  if(NULL == f->private_data)
  {
    return -ENOMEM;
  }
  memset(f->private_data,0,sizeof(priv_data_t));
  return 0;
}

static int testko_release(struct inode *i, struct file *f)
{
  kfree(f->private_data);
  return 0;
}

static ssize_t testko_read(struct file *f,char __user *buf,size_t len,loff_t *off)
{
  return 0;
}

static ssize_t testko_write(struct file *f, const char __user *buf, size_t len,loff_t *off)
{
  return 0;
}

typedef struct  {
  unsigned long dwMethodName;
  unsigned long dwData;
  /* data */
}testko_acpi_t;



/******ioctrl*******/
static long testko_ioctrl(struct file *f,unsigned int cmd, unsigned long arg)
{
  if(TESTKO_SET_WWAN_STATUS == cmd)
  {

	   acpi_handle chandle = NULL;
	   struct acpi_buffer path_buf;
	   char path[ACPI_PATHNAME_MAX];
	   acpi_object_type type;
	   acpi_status status;
     //acpi_status method_status;
	   acpi_handle parent_handle;
	   struct acpi_buffer buffer;
	   //struct acpi_device_info *dev_info;
	   unsigned long long current_status = 0;


	   buffer.length = ACPI_ALLOCATE_BUFFER;
	   buffer.pointer = NULL;

	   path_buf.length = sizeof(path);
	   memset(path, 0, sizeof(path));

	   status = acpi_get_handle(ACPI_ROOT_OBJECT, ACPI_DEVICE_PATH, &parent_handle); /*get device handle*/
     if (ACPI_SUCCESS(status))
	   {
		    printk(KERN_ERR "TestKo driver: ioctrl acpi_get_handle successful \n");
	   }

	   path_buf.pointer = path;

	   status = acpi_get_name(parent_handle, 0, &path_buf);

     if(ACPI_FAILURE(status))
     {
       printk(KERN_INFO "testko driver: acpi_get_name failed!!! status=%d,[%d]",status,__LINE__);

     }
     else
     {
       printk(KERN_INFO "testko driver: ioctrl acpi name is %s[%d]\n", (char*)path_buf.pointer,__LINE__);

     }

         /*Test the device using “_RST" method*/
         status = acpi_evaluate_integer(parent_handle,"_RST",NULL,&current_status);
         if(ACPI_SUCCESS(status))
         {
            printk(KERN_INFO "testko driver call _RST success !:%s current status is %x[%d]\n",
                  (char*)path_buf.pointer,
                  (unsigned int)current_status),
                  __LINE__;
         }
         else
         {
               printk(KERN_ERR "testko driver call _RST failed![%d]\n",__LINE__);
         }


  }
     printk(KERN_ERR "testko driver: ioctrl cmd : Ox%x.[%d]\n", cmd,__LINE__);
     return 0;

}


static struct file_operations testko_fops = {
.read = testko_read,
.write = testko_write,
.open = testko_open,
.release = testko_release,
.unlocked_ioctl = testko_ioctrl,
.owner = THIS_MODULE,
};
struct cdev *testko_cdev;
struct class *testko_class;

static int __init testko_init_module(void)
{
     int err ;
     dev_t devon;
     printk(KERN_ERR "testko_init_module[%d]!\n",__LINE__);
     devon = MKDEV(TESTKO_MAJOR, TESTKO_MINOR);
     printk(KERN_ERR "testko_init_module[%d]!\n",__LINE__);
     testko_cdev = cdev_alloc();
     printk(KERN_ERR "testko_init_module[%d]!\n",__LINE__);
     if (NULL == testko_cdev) {
        printk(KERN_ERR "cdev_alloc failed!\n");
        return -1;
     }
     printk(KERN_ERR "testko_init_module[%d]!\n",__LINE__);
     cdev_init (testko_cdev, &testko_fops);
     printk(KERN_ERR "testko_init_module[%d]!\n",__LINE__);
     testko_cdev->owner= THIS_MODULE;
     printk(KERN_ERR "testko_init_module[%d]!\n",__LINE__);
     err = cdev_add(testko_cdev, devon, 1);
     printk(KERN_ERR "testko_init_module[%d]!\n",__LINE__);
     if (0 != err) {
		     printk(KERN_ERR "testko device register failed!\n");
     }

     /*create owner class*/
     printk(KERN_ERR "testko_init_module[%d]!\n",__LINE__);
     testko_class = class_create(THIS_MODULE, "testko_class");
     printk(KERN_ERR "testko_init_module[%d]!\n",__LINE__);
     if (IS_ERR(testko_class)) {
          printk(KERN_ERR "testko device create class fdiled!\n");
		      return -1;

     }
     /*Create the class node for the test app accessing this driver*/
     printk(KERN_ERR "testko_init_module[%d]!\n",__LINE__);
     device_create(testko_class, NULL, MKDEV(TESTKO_MAJOR, TESTKO_MINOR),NULL,TESTKO_NAME);
     printk(KERN_ERR "testko_init_module[%d]!\n",__LINE__);
     printk( KERN_ERR "Module TestKo init\n");
     return 0;
}

static void __exit testko_exit_module(void)
{

     cdev_del(testko_cdev);
     device_destroy (testko_class, MKDEV(TESTKO_MAJOR, TESTKO_MINOR));
     class_destroy(testko_class);

     printk( KERN_ERR "Module TestKo exit\n");

}

module_init(testko_init_module);
module_exit (testko_exit_module);
