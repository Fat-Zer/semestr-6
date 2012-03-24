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
			  fprintf(stderr,  __VA_ARGS__ ); } \
		if( !pd.flag.daemon && pd.log.fs) \
			{ fprintf( pd.log.fs, "%s: ", loglvl2str(lvl)); \
			  fprintf( pd.log.fs, __VA_ARGS__ ); } \
	} while(0)

typedef int error_t;

static struct {
	const char* prog_name;
	bool daemonized;
	const char *pipe;
	struct {
		char *fname;
		FILE *fs;
		bool syslog;
		bool stderr;
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
	.log={.fname=0, .fs=0, .syslog=0, .stderr=0}, 
	.flag={.help=0, .usage=0, .daemon=1},
	.ligth={.iface=0, .iface_name=0} };

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
			exit(1);

		default:
			fprintf(stderr, "?? getopt returned character code 0%o ??\n", c);
			usage();
			exit(2);
		}
	}
	if (argc - optind > 1) {
		usage();
		exit(3);
	} else if (argc - optind != 1) {
		pd.ligth.iface_name = argv[optind];
	} 
	
	return 0;
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
	
	fd = open(name, mode);
	if( fd == -1) {
		LOG(LOG_ERR, "open file %s failed with error %d:%s\n",
				name, errno, strerror(errno));
		return -1;
	}

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
