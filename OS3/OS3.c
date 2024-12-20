#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
MODULE_LICENSE("GPL");
static int __init OS3_init(void) {
pr_info("Welcome to the Tomsk State University\n");
return 0;
}
static void __exit OS3_exit(void) {
pr_info("Tomsk State University forever!\n");
}
module_init(OS3_init);
module_exit(OS3_exit);
