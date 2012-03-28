#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <syslog.h>
#include <getopt.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define LOG(lvl, ...) do { \
		if(pd.log.syslog) { syslog(lvl, __VA_ARGS__); } \
		if( !pd.daemonized || (pd.daemonized && pd.log.fname)) \
			{ fprintf(stderr, "%s: ", loglvl2str(lvl)); \
			  fprintf(stderr,  __VA_ARGS__ ); \
			  fprintf(stderr, "\n"); } \
		if( !pd.flag.daemon && pd.log.fs) \
			{ fprintf( pd.log.fs, "%s: ", loglvl2str(lvl)); \
			  fprintf( pd.log.fs, __VA_ARGS__ ); \
			  fprintf(stderr, "\n"); } \
	} while(0)

#define DEFAULT_CONTROL_PIPE "/var/run/backlightd.pipe"
#define DEFAULT_PID_FILE "/var/run/backlightd.pid"

typedef int error_t;

enum exit_error_t {
	EERR_SUCCESS=0,
	EERR_BAD_KEY,
	EERR_GETOPT_FAILED,
	EERR_DAEMONIZE_FAILED,
	EERR_REOPEN_DESCRIPTERS,
	EERR_LOCK_PIDFILE };

static struct {
	const char* prog_name;
	bool daemonized;
	const char *pipe;
	FILE *pipe_fd;
	const char *pid_file;
	int pid_fd;
	struct {
		char *fname;
		FILE *fs;
		bool syslog;
	} log;
	struct {
		bool help;
		bool usage;
		bool daemon;
	} flag;
	struct {
		char* iface;
		const char* iface_name;
	} ligth;
} pd = { 
	.prog_name=0,
	.daemonized=0, 
	.pid_file=DEFAULT_PID_FILE,
	.pid_fd=-1,
	.pipe=DEFAULT_CONTROL_PIPE,
	.pipe_fd=-1,
	.log={.fname=0, .fs=0, .syslog=0}, 
	.flag={.help=0, .usage=0, .daemon=1},
	.ligth={.iface=0, .iface_name="intel_backlight"} };

void usage() {
	fprintf(stderr,"Usage: %s -uhDNS --log <log_file> --pipe <control_pipe> [brightness_drv]\n", 
			pd.prog_name);
}

void help() {
	usage();
	fprintf(stderr,
		"-u\t--usage\tshow usage message\n"
		"-h\t--help\tshow this help message\n"
		"-D\t--daemon\t\n"
		"-N\t--no-daemon\t\n"
		"-S\t--syslog\t\n"
		"-L\t--log <log file>\t\n"
		"-p\t--pipe <control_pipe>\t\n");
}

const char * loglvl2str(int lvl);
void start_backlightd();
error_t daemonize();

int main(int argc, char** argv) {
	
	pd.prog_name=argv[0];

	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"help",     no_argument,		0,  'h' },
			{"usage",    no_argument,		0,  'u' },
			{"daemon",   no_argument,		0,  'D' },
			{"no-daemon",no_argument,		0,  'N' },
			{"syslog",   no_argument,		0,  's' },
			{"log",      required_argument, 0,  'L' },
			{"pipe",     required_argument,	0,  'p' },
			{0,			0,					0,  0 }};
		const char *opt_string="uhDNsL:p:";
		int c;
		c = getopt_long(argc, argv, opt_string,
						long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'h':
			pd.flag.help=1;
			break;
		case 'u':
			pd.flag.usage=1;
			break;
		case 'D':
			pd.flag.daemon=1;
			break;
		case 'N':
			pd.flag.daemon=0;
			break;
		case 's':
			pd.log.syslog=1;
			break;
		case 'L':
			pd.log.fname=optarg;
			break;
		case 'p':
			pd.pipe=optarg;
			break;
		case '?':
			usage();
			exit(EERR_BAD_KEY);

		default:
			fprintf(stderr, "?? getopt returned character code 0%o ??\n", c);
			usage();
			exit(EERR_GETOPT_FAILED);
		}
	}
	if(argc - optind > 1) {
		usage();
		exit(3);
	} else if (argc - optind != 1) {
		pd.ligth.iface_name = argv[optind];
	}
	
	if(pd.flag.help) {
		help();
	} else if(pd.flag.usage) {
		usage();
	} else {
		start_backlightd();
	}
	
	return 0;
}

void start_backlightd() {
	if(pd.log.syslog) {
		openlog(pd.prog_name, 0, pd.flag.daemon ? LOG_DAEMON : LOG_USER);
	}

	if(!pd.flag.daemon && pd.log.fname) {
		pd.log.fs = fopen(pd.log.fname, "w");
		if(!pd.log.fs) {
			LOG(LOG_ERR, "open file %s failed with error %d:%s\n",
					pd.log.fname, errno, strerror(errno));
		}
	}

	umask(0);
	pd.pid_fd = lock_pid_file(pd.pid_file);
	if(pd.pid_fd == -1) {
		LOG(LOG_CRIT, "locking pidfile failed: seems the daemon allready running.");
		exit(EERR_LOCK_PIDFILE);
	}

	pd.pipe_fd = open_ctl_pipe(pd.pipe);
	if(pd.pipe_fd == -1) {
		LOG(LOG_CRIT, "creation of the control pipe failed.");
		exit(EERR_LOCK_PIDFILE);
	}

	if(pd.flag.daemon) {
		if(daemonize() == -1) {
			LOG(LOG_INFO, "Daemonized successfull.");
			pd.daemonized=1;
			if(reopen_stdio() != -1) {
				LOG(LOG_INFO, "Reopened stdio descripters successfully.");
			} else {
				LOG(LOG_CRIT, "reopen stdio descripters failed.");
				exit(EERR_REOPEN_DESCRIPTERS);
			}
		} else {
			LOG(LOG_CRIT, "Daemonized failed. Exiting.");
			exit(EERR_DAEMONIZE_FAILED);
		}
	}

}

const char * loglvl2str(int lvl) {
	switch(lvl) {
		case LOG_EMERG:   return "EMRG";
		case LOG_ALERT:   return "ALRT";
		case LOG_CRIT:    return "CRIT";
		case LOG_ERR:     return "ERRO";
		case LOG_WARNING: return "WARN";
		case LOG_NOTICE:  return "NOTI";
		case LOG_INFO:    return "INFO";
		case LOG_DEBUG:   return "DBUG";
		default:          return "UKNW";
	}
}

int open_ctl_pipe(const char *name) {
	int rv;
	
	rv = mkfifo(name, O_WRIGHT | O_CREAT | O_NO_CTTY | O_TRUNC, 0640);
	if( rv == -1 ) {
		LOG(LOG_ERR, "open file %s failed with error %d:%s\n",
				name, errno, strerror(errno));
	}
	else if( lockf(rv, F_TLOCK, 0) == -1) {
		LOG(LOG_ERR, "lock file %s failed with error %d:%s\n",
				name, errno, strerror(errno));
		close(rv);
		rv = -1;
	}

	return rv;
}


int lock_pid_file(const char *name) {
	int rv;

	rv = open(name, O_WRIGHT | O_CREAT | O_NO_CTTY | O_TRUNC, 0640);
	if( rv == -1 ) {
		LOG(LOG_ERR, "open file %s failed with error %d:%s\n",
				name, errno, strerror(errno));
	}
	else if( lockf(rv, F_TLOCK, 0) == -1) {
		LOG(LOG_ERR, "lock file %s failed with error %d:%s\n",
				name, errno, strerror(errno));
		close(rv);
		rv = -1;
	}

	return rv;
}

error_t daemonize() {
    pid_t pid;

    if( (pid = fork()) < 0 ) {
        LOG(LOG_ERR, "first fork failed!");
        return -1 ;
    } else if ( pid != 0 ) {
        exit(0);
    }

    setsid();

    if( (pid = fork()) < 0) {
        LOG(LOG_ERR, "second fork failed!");
        return -1 ;
    } else if ( pid != 0 ) {
        exit( 0 );
    }

    setsid();
    umask(0);

	return 0;
}

int reopen_fd(int fd, const char* name, mode_t mode) {
	if( close(fd) == -1) {
        LOG(LOG_ERR, "close fd=%d failed with error %d: %s\n", 
				fd, errno, strerror(errno));
		return -1;
	}
	
	if( fd == -1) {
		LOG(LOG_ERR, "open file %s failed with error %d:%s\n",
				name, errno, strerror(errno));
		return -1;
	}

	fd = open(name, mode);
	return fd;
}

error_t reopen_stdio() {
	if(reopen_fd(0, "/dev/null", O_RDONLY) == -1)
		return -1;

	if(pd.log.fname) {
		if( close(1) == -1) {
	        LOG(LOG_ERR, "close fd=%d failed with error %d: %s\n", 
					0, errno, strerror(errno));
			return -1;
		}
	
		if( open(pd.log.fname, O_WRONLY | O_CREAT | O_TRUNC, 
					S_IRUSR | S_IWUSR) == -1) {
			LOG(LOG_ERR, "open file %s failed with error %d:%s\n",
					pd.log.fname, errno, strerror(errno));
			return -1;
		}

	} else {
		if(reopen_fd(1, "/dev/null", O_WRONLY) == -1)
			return -1;
	}
		
	if( close(2) == -1) {
	    LOG(LOG_ERR, "close fd=%d failed with error %d: %s\n", 
				2, errno, strerror(errno));
		return -1;
	}
	
	if( dup(1) == -1) {
		LOG(LOG_ERR, "dup file descriptor 1 failed with error %d:%s\n",
			errno, strerror(errno));
		return -1;
	}

    return 0;
}
