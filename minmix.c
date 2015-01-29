/*
 * minmix - a minimal oss mixer
 *
 * Copyright (C) 2010-2015 Ali Gholami Rudi <ali at rudi dot ir>
 *
 * This program is released under the Modified BSD license.
 */
#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

#define LEN(a)			(sizeof(a) / sizeof((a)[0]))
#define DEFCTL			"pcm"

static int afd;
static char *vnames[] = SOUND_DEVICE_NAMES;

static int vol_id(char *name)
{
	int i;
	if (isdigit(name[0]))
		return atoi(name);
	for (i = 0; i < LEN(vnames); i++)
		if (!strcmp(name, vnames[i]))
			return i;
	return -1;
}

static int vol_get(int id)
{
	int v = 0;
	ioctl(afd, MIXER_READ(id), &v);
	return v & 0xff;
}

static void vol_set(int id, int vol)
{
	vol = vol | (vol << 8);
	ioctl(afd, MIXER_WRITE(id), &vol);
}

static void vol_set_kwd(char *kwd)
{
	char name[128];
	char *val = strchr(kwd, '=') + 1;
	strcpy(name, kwd);
	*strchr(name, '=') = '\0';
	vol_set(vol_id(name), atoi(val));
}

static char *usage =
	"usage: minmix [options]\n\n"
	"examples:\n"
	"   minmix                   default control (" DEFCTL ") value\n"
	"   minmix 40                set default control value\n"
	"   minmix /                 list all controls and their values\n"
	"   minmix pcm vol           show the value of specified controls\n"
	"   minmix pcm=40 bass=50    set the value of specified controls\n"
	"   minmix vol=70 vol        set and show the values at the same time\n";

int main(int argc, char *argv[])
{
	int i, j;
	afd = open("/dev/mixer", O_RDWR);
	if (afd < 0) {
		fprintf(stderr, "cannot open /dev/mixer\n");
		return 1;
	}
	if (argc < 2)
		printf("%d\n", vol_get(vol_id(DEFCTL)));
	for (i = 1; i < argc; i++) {
		if (!strcmp("-h", argv[i]))
			printf(usage);
		if (!strcmp("-v", argv[i]))
			printf("minmix-0.3\n");
		if (strchr(argv[i], '=')) {
			vol_set_kwd(argv[i]);
			continue;
		}
		if (isalpha(argv[i][0]))
			printf("%d\n", vol_get(vol_id(argv[i])));
		if (isdigit(argv[i][0]))
			vol_set(vol_id(DEFCTL), atoi(argv[i]));
		if (!strcmp("/", argv[i])) {
			int devmask;
			ioctl(afd, SOUND_MIXER_READ_DEVMASK, &devmask);
			for (j = 0; j < LEN(vnames); j++)
				if (devmask & (1 << j))
					printf("%s\t%d\n", vnames[j], vol_get(j));
		}
	}
	close(afd);
	return 0;
}
