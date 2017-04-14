#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/slab.h>
#include<linux/errno.h>
#include<linux/types.h>
#include<linux/fs.h>
#include<linux/proc_fs.h>
#include<asm/uaccess.h>
#include<linux/ioctl.h>

#define MAJOR_NUMBER 61/* forward declaration */
#define FILE_SIZE 4*1024*1024
#define SCULL_IOC_MAGIC 'k'
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC, 1)
#define SCULL_IOC_MAXNR 3

int onebyte_open(struct inode *inode, struct file *filep);
int onebyte_release(struct inode *inode, struct file *filep);
ssize_t onebyte_read(struct file *filep, char *buf, size_t
count, loff_t *f_pos);
ssize_t onebyte_write(struct file *filep, const char *buf,
size_t count, loff_t *f_pos);
static void onebyte_exit(void);
/* definition of file_operation structure */
struct file_operations onebyte_fops = {
read:
onebyte_read,
write:
onebyte_write,
open:
onebyte_open,
release: onebyte_release
};
char *onebyte_data = NULL;
int onebyte_open(struct inode *inode, struct file *filep)
{
return 0; // always successful
}
int onebyte_release(struct inode *inode, struct file *filep)
{
return 0; // always successful
}

static int size = 0;
ssize_t onebyte_read(struct file *filep, char *buf, size_t
count, loff_t *f_pos)
{
/*please complete the function on your own*/

	int bytes_read = 0;
	while(*onebyte_data && count){
		put_user(*(onebyte_data++),buf++);

		count --;
		bytes_read ++;
	}
	
	return bytes_read;
	
	
/*
	if(*f_pos>=4*1024*1024){
		return 0;
	}

	

	if(copy_to_user(buf,onebyte_data,count)){
		return -ENOSPC;
	}
	*f_pos += count;

	return count;*/
	
	/**f_pos += 1;
	if(*f_pos > 1){
		return 0;	
	}
	return 1;*/
	
}
ssize_t onebyte_write(struct file *filep, const char *buf,
size_t count, loff_t *f_pos)
{
/*please complete the function on your own*/
	
	
	if(*f_pos>=4*1024*1024){
		return -ENOSPC;
	}
	
	if(copy_from_user(onebyte_data, buf, count)){
		return -EFAULT;
	}

	*f_pos += count;
	siez += count;
	printk(KERN_ALERT "The file size is %d",*f_pos);
	return count;
/*
	int i;
		
	for(i = 0; i<count && i< FILE_SIZE;i++){
		get_user(*(onebyte_data+i),buf+i);
	}
	printk(KERN_ALERT "The file size is %d",i);
	return i;
	*/
}
static loff_t onebyte_llseek(struct file *filp, loff_t off, int whence){
	
	loff_t newpos;
	
	switch(whence)
	{
	case 0:
		newpos = off;
		break;
	case 1:
		newpos = filp->f_pos + off;
		break;
	case 2:
		newpos = size -1 + off;
		break;
	default:
		return -EINVAL;	
	}
	if(newpos < 0){
		return -EINVAL;
	}
	filp->f_pos = newpos;
	return newpos; 
}
static long ioctl_onebyte(struct file *filp, unsigned int cmd, unsigned long arg){
	int err = 0, tmp;
	int retval = 0;
	if(_IOC_TYPE(cmd)!=SCULL_IOC_MAGIC) return -ENOTTY;
	if(_IOC_NR(cmd)> SCULL_IOC_MAXNR) return -ENOTTY;
	if (_IOC_DIR(cmd) & _IOC_READ) 
    		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd)); 
    	else if (_IOC_DIR(cmd) & _IOC_WRITE) 
    		err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd)); 
    	if(err) return -ENOTTY;
    	switch(cmd) { 
            case SCULL_HELLO: 
        	printk(KERN_WARNING "hello here\n"); 
        	break;  
     	    default: 
    
 		return -ENOTTY; 
    	} 
    	return retval; 
	
}
static int onebyte_init(void)
{
int result;
// register the device
result = register_chrdev(MAJOR_NUMBER, "onebyte",
&onebyte_fops);
if (result < 0) {
return result;
}
// allocate one byte of memory for storage
// kmalloc is just like malloc, the second parameter is// the type of memory to be allocated.
// To release the memory allocated by kmalloc, use kfree.
onebyte_data = kmalloc(4*1024*1024, GFP_KERNEL);
if (!onebyte_data) {
onebyte_exit();
// cannot allocate memory
// return no memory error, negative signify a failure
return -ENOMEM;
}
// initialize the value to be X
*onebyte_data = 'X';
printk(KERN_ALERT "Onebyte device test1\n");

return 0;
}
static void onebyte_exit(void)
{
// if the pointer is pointing to something
if (onebyte_data) {
// free the memory and assign the pointer to NULL
kfree(onebyte_data);
onebyte_data = NULL;
}
// unregister the device
unregister_chrdev(MAJOR_NUMBER, "onebyte");

//printk(KERN_ALERT "Onebyte device module is unloaded\n");

}
MODULE_LICENSE("GPL");
module_init(onebyte_init);
module_exit(onebyte_exit);
