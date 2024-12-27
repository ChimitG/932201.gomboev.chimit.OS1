#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timekeeping.h>
#include <linux/rtc.h>

MODULE_LICENSE("GPL");
static int __init OS4_init(void) {
  struct timespec64 ts;
  struct rtc_time tm;
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
  pr_info("Percentage of passed month: %d%%/n", perc);
  pr_info("Days passed: %d", days);
  pr_info("Hours passed: %d", hours);
  pr_info("Minutes passed: %d", minutes);
  return 0;
}
static void __exit OS4_exit(void) {
pr_info("All information for UTC+7!\n");
}
module_init(OS4_init);
module_exit(OS4_exit);
