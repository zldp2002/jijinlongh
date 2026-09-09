#include <progress.h>

static double gettime(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

static const char * format_eta(double remaining)
{
	static char result[6] = "";
	int seconds = remaining + 0.5;
	if(seconds >= 0 && seconds < 6000)
	{
		snprintf(result, sizeof(result), "%02d:%02d", seconds / 60, seconds % 60);
		return result;
	}
	return "--:--";
}

static char * ssize(char * buf, double size)
{
	const char * unit[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
	int count = 0;

	while((size > 1000) && (count < 8))
	{
		size /= 1000;
		count++;
	}
	sprintf(buf, "%5.3f %s", size, unit[count]);
	return buf;
}

void progress_start(struct progress_t * p, size_t total)
{
	if(p && (total > 0))
	{
		p->total = total;
		p->done = 0;
		p->start = gettime();
	}
}

void progress_update(struct progress_t * p, size_t bytes)
{
	char buf1[32], buf2[32];

	if(p)
	{
		p->done += bytes;
		double ratio = p->total > 0 ? (double)p->done / (double)p->total : 0.0;
		double speed = (double)p->done / (gettime() - p->start);
		double eta = speed > 0 ? (p->total - p->done) / speed : 0;
		int i, pos = 48 * ratio;
		printf("\r%3.0f%% [", ratio * 100);
		for(i = 0; i < pos; i++)
			putchar('=');
		for(i = pos; i < 48; i++)
			putchar(' ');
		if(p->done < p->total)
			printf("] %s/s, ETA %s", ssize(buf1, speed), format_eta(eta));
		else
			printf("] %s, %s/s        ", ssize(buf1, p->done), ssize(buf2, speed));
		fflush(stdout);
	}
}

void progress_stop(struct progress_t * p)
{
	if(p)
		printf("\r\n");
}
