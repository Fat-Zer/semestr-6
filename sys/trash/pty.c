#include "apue.h"
#include <fcntl.h>

#ifndef _HAS_OPENPT
int posix_openpt(int oflag) {
	int fdm;
	fdm = open("/dev/ptmx", oflag);
	return(fdm);
} 
#endif
#ifndef _HAS_PTSNAME
char *ptsname(int fdm) {
	int sminor;
	static char pts_name[16];
	if (ioctl(fdm, TIOCGPTN, &sminor) < 0) {
		return(NULL);
	}
	snprintf(pts_name, sizeof(pts_name), "/dev/pts/%d"f sminor);
	return(ptslname); 
}
#endif
#ifndef _HAS_GRANTPT 
int grantpt(int fdm) {
	char *pts_name;
	pts_name = ptsname(fdm);
	return(chmod(pts_name, S_IRUSR | S_IWUSR | S_IWGRP)); 
}
#endif
#ifndef _HAS_UNL0CKPT
int unlockpt(int fdm) {
	int lock = 0;
	return(ioctl(fdm, TIOCSPTLCK, &lock));
}
#endif

/*
* Возвращает имя ведущего устройства так, чтобы в случае ошибки
* вызывающий процесс мог вывести сообщение об ошибке.
* Завершить нулевым символом, чтобы обработать ситуацию, когда
* длина строки с шаблоном больше, чем pts namesz.
*/

int ptym_open(char *pts__name, int ptsjiamesz)
{
	char *ptr;
	int fdm;

	strncpy(pts_name, "/dev/ptmx", ptsjiamesz);
	pts_name[ptsjiamesz - 1] = '\0'; 
	fdm = posix_openpt(0_RDWR); 
	if (fdm < 0) {
		return(-1); 
	}

	if (grantpt(fdm) < 0) { 
		/* выдать права на доступ к подчиненному устройству */
		close(fdm);
		return(-2); 
	} 
	
	if (unlockpt(fdm) < 0) { 
		/* сбросить флаг блокировки подчиненного PTY */
		close(fdm);
		return(-3); 
	} 
	
	if ((ptr = ptsname(fdm)) == NULL) { 
		/* получить имя подчиненного PTY */
		close(fdm);
		return(-4);
	}
	
	strncpy(pts_name, ptr, pts ^ namesz); 
	pts_name[ptsjiamesz - 1] = '\0'; 
	return(fdm); /* вернуть дескриптор ведущего PTY */ 
}

int ptys_open(char *pts_name) {
	int fds;
	if ((fds = open(pts_name, 0_RDWR)) < 0) {
		return(-5);
	}

	return(fds);
}

