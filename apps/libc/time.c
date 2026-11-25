#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>

#include "lib.h"



unsigned int sleep(unsigned int seconds)
{
    struct timespec ts;
    ts.tv_nsec = 0;
    ts.tv_sec = seconds;
    do_syscall(SYSCALL_NANOSLEEP,&ts);
    return 0;
}

int usleep(useconds_t usec)
{
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = usec * 1000;
    do_syscall(SYSCALL_NANOSLEEP,&ts);
    return 0;
}

int gettimeofday(struct timeval *tv, void *tz)
{
    return do_syscall(SYSCALL_GETTIMEOFDAY,tv);
}
int settimeofday(const struct timeval *tv, const struct timezone *tz)
{
    return do_syscall(SYSCALL_SETTIMEOFDAY,(void *)tv);
}

time_t time(time_t *tloc)
{
    struct timeval tv;
    do_syscall(SYSCALL_GETTIMEOFDAY, &tv);
    if(tloc)
    {
        *tloc = tv.tv_sec;
    }
    return tv.tv_sec;
}

//stolen from musl
/* 2000-03-01 (mod 400 year, immediately after feb29 */
#define LEAPOCH (946684800LL + 86400*(31+29))

#define DAYS_PER_400Y (365*400 + 97)
#define DAYS_PER_100Y (365*100 + 24)
#define DAYS_PER_4Y   (365*4   + 1)

int __secs_to_tm(long long t, struct tm *tm)
{
	long long days, secs, years;
	int remdays, remsecs, remyears;
	int qc_cycles, c_cycles, q_cycles;
	int months;
	int wday, yday, leap;
	static const char days_in_month[] = {31,30,31,30,31,31,30,31,30,31,31,29};

	/* Reject time_t values whose year would overflow int */
	if (t < INT_MIN * 31622400LL || t > INT_MAX * 31622400LL)
		return -1;

	secs = t - LEAPOCH;
	days = secs / 86400;
	remsecs = secs % 86400;
	if (remsecs < 0) {
		remsecs += 86400;
		days--;
	}

	wday = (3+days)%7;
	if (wday < 0) wday += 7;

	qc_cycles = days / DAYS_PER_400Y;
	remdays = days % DAYS_PER_400Y;
	if (remdays < 0) {
		remdays += DAYS_PER_400Y;
		qc_cycles--;
	}

	c_cycles = remdays / DAYS_PER_100Y;
	if (c_cycles == 4) c_cycles--;
	remdays -= c_cycles * DAYS_PER_100Y;

	q_cycles = remdays / DAYS_PER_4Y;
	if (q_cycles == 25) q_cycles--;
	remdays -= q_cycles * DAYS_PER_4Y;

	remyears = remdays / 365;
	if (remyears == 4) remyears--;
	remdays -= remyears * 365;

	leap = !remyears && (q_cycles || !c_cycles);
	yday = remdays + 31 + 28 + leap;
	if (yday >= 365+leap) yday -= 365+leap;

	years = remyears + 4*q_cycles + 100*c_cycles + 400LL*qc_cycles;

	for (months=0; days_in_month[months] <= remdays; months++)
		remdays -= days_in_month[months];

	if (months >= 10) {
		months -= 12;
		years++;
	}

	if (years+100 > INT_MAX || years+100 < INT_MIN)
		return -1;

	tm->tm_year = years + 100;
	tm->tm_mon = months + 2;
	tm->tm_mday = remdays + 1;
	tm->tm_wday = wday;
	tm->tm_yday = yday;

	tm->tm_hour = remsecs / 3600;
	tm->tm_min = remsecs / 60 % 60;
	tm->tm_sec = remsecs % 60;

	return 0;
}

struct tm *gmtime(const time_t *timep)
{
    static struct tm tm_out;
    __secs_to_tm(*timep, &tm_out);
    return &tm_out;
}
