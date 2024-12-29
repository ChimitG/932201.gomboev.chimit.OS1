#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timekeeping.h>
#include <linux/rtc.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
#define PROCFS_NAME "tsulab"
static struct proc_dir_entry *our_proc_file =NULL;
static ssize_t procfile_read(struct file *file_pointer, char __user *buffer, size_t buffer_length, loff_t *offset){
  struct timespec64 ts;
  struct rtc_time tm;
  size_t inf;
  char in[256];
  int all, passed, year, month, days, hours, minutes, seconds, perc;
  ktime_get_real_ts64(&ts);
  rtc_time64_to_tm(ts.tv_sec, &tm);
  year = tm.tm_year +1900;
  month = tm.tm_mon;
  days = tm.tm_mday;
  hours = tm.tm_hour + 7;
  minutes = tm.tm_min;
  seconds = tm.tm_sec;
  int days_in_month[]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (year%4==0 && (year %100 != 0 || year %400==0)){
    days_in_month[1] = 29;
  }
  if (hours >= 24){
    hours -=24;
    days+=1;
    if (days>days_in_month[month]){
      days-=days_in_month[month];
      month+=1;
      if (month>11){
        month-=11;
        year+=1;
      }
    }
  }
  all = days_in_month[tm.tm_mon] *24 *60 *60;
  passed = days*24 *60 *60 + hours *60 *60 + minutes *60 +seconds;
  perc = passed *100 /all;
  inf = snprintf(in,sizeof(in),
                  "Percentage of passed month: %d%%/n"
                  "Time passed: %d days %d hours %d Minutes %d Seconds passed",
                  perc, days, hours, minutes, seconds);
  if (*offset >= inf) return 0;
  if (buffer_length == 0) return -EINVAL;
  if (buffer_length <inf) return -EINVAL;
  if (copy_to_user(buffer, in, inf)){
    return -EFAULT;
  }
  *offset+= inf;
  return inf;
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops proc_file_fops = {
  .proc_read = procfile_read,
};
#else
static const struct file_operations proc_file_fops = {
  .proc = procfile_read,
};
#endif
static int __init OS4_init(void) {
  our_proc_file = proc_create(PROCFS_NAME, 0644, NULL, &proc_file_fops);
  return 0;
}
static void __exit OS4_exit(void) {
  proc_remove(our_proc_file);
  pr_info("All information for UTC+7!\n");
}
module_init(OS4_init);
module_exit(OS4_exit);
