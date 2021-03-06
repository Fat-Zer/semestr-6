#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <syslog.h>
#include <getopt.h>
#include <errno.h>
#include <ctype.h>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#include "backlight_ctl.h"

#define LOG(lvl, ...) do { \
		if(pd.log.syslog) { syslog(lvl, __VA_ARGS__); } \
		if( !pd.daemonized || (pd.daemonized && pd.log.fname)) \
			{ fprintf(stderr, "%s: ", loglvl2str(lvl)); \
			  fprintf(stderr,  __VA_ARGS__ ); } \
		if( !pd.flag.daemon && pd.log.fs) \
			{ fprintf( pd.log.fs, "%s: ", loglvl2str(lvl)); \
			  fprintf( pd.log.fs, __VA_ARGS__ ); } \
	} while(0)

#define DEBUG_LOG( ... ) LOG(LOG_DEBUG, __VA_ARGS__)

#define DEFAULT_CONTROL_FIFO "/var/run/backlightd.fifo"
#define DEFAULT_PID_FILE "/var/run/backlightd.pid"
#define COMMAND_BUFF_SZ ((size_t)1 << 10)

typedef int error_t;

enum exit_error_t {
	EERR_SUCCESS=0,
	EERR_BAD_KEY,
	EERR_GETOPT_FAILED,
	EERR_DAEMONIZE_FAILED,
	EERR_REOPEN_DESCRIPTERS,
	EERR_LOCK_PIDFILE,
	EERR_BAD_IFACE,
	EERR_CREATE_FIFO,
	EERR_FCLOSE_FIFO,
	EERR_FDOPEN_FIFO };

static struct {
	const char* prog_name;
	bool daemonized;
	const char *pid_file;
	int pid_fd;
	struct {
		const char* l_name;
		struct addrinfo l_addr;
		int id;
	} socket;
	struct {
		const char *name;
		int fd;
	} fifo;
	struct {
		char *fname;
		FILE *fs;
		bool syslog;
	} log;
	struct {
		bool help;
		bool usage;
		bool daemon;
		bool fifo_ctl;
		bool socket_ctl;
	} flag;
	struct {
		char* iface;
		const char* iface_name;
	} light;
	struct {
		struct {
			
		} msg_que;
	} itc;
} pd = { 
	.prog_name=0,
	.daemonized=0, 
	.pid_file=DEFAULT_PID_FILE,
	.pid_fd=-1,
	.socket={.l_name=0, .l_addr={0,0}, .id=-1},
	.fifo={.name=DEFAULT_CONTROL_FIFO, .fd=-1},
	.log={.fname=0, .fs=0, .syslog=0}, 
	.flag={.help=0, .usage=0, .daemon=1, .fifo_ctl=0, .socket_ctl=0},
	.light={.iface=0, .iface_name=0} };

void usage() {
	fprintf(stderr,"Usage: %s -uhDNs --log log_file --fifo control_fifo\n"
					"\t--listen host:port [brightness_drv]\n", 
			pd.prog_name);
}

void help() {
	usage();
	// TODO: finish the help
	fprintf(stderr,
		"-u\t--usage\tshow usage message\n"
		"-h\t--help\tshow this help message\n"
		"-D\t--daemon\t\n"
		"-N\t--no-daemon\t\n"
		"-s\t--syslog\t\n"
		"-L\t--log log file\t\n"
		"-l\t--listen host:port\ta host port to listen\n"
		"-f\t--fifo<control_fifo\t\n");
}

const char * loglvl2str(int lvl);
void start_backlightd();
int lock_pid_file(const char *name);
int open_ctl_fifo(const char *name);
int open_ctl_socket(const char *name, struct addrinfo *addr);
error_t daemonize();
int reopen_fd(int fd, const char* name, mode_t mode);
error_t reopen_stdio();
void backlightd_loop();
error_t execute_fs_content(FILE* fs);
error_t execute_str_command(const char *str);
char get_str_command(const char *str);
void end_backlightd();
void on_sigterm(int);

/** returns the position of first character in s not contained in naccept
 * (an oposite function to strspn())
 */
char *strnspn(const char *s, const char *naccept);

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
			{"fifo",     optional_argument,	0,  'f' },
			{"listen",   required_argument,	0,  'l' },
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
		case 'l':
			pd.flag.socket_ctl=1;
			pd.socket.l_name=optarg;
			break;
		case 'f':
			pd.flag.fifo_ctl=1;
			if(optarg) {
				pd.fifo.name = optarg;
			} else {
				pd.fifo.name = DEFAULT_CONTROL_FIFO;
			}
			break;
		case '?':
			usage();
			exit(EERR_BAD_KEY);

		default:
			usage();
			exit(EERR_GETOPT_FAILED);
		}
	}
	if(argc - optind > 1) {
		usage();
		exit(3);
	} else if (argc - optind == 1) {
		pd.light.iface_name = argv[optind];
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
	int err = 0;

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
	
	if(!pd.flag.fifo_ctl && !pd.flag.socket_ctl) {
		LOG(LOG_CRIT, "No control type specified. \n"
				"Pleace specify the -f flag for fifo or -l for socket.\n");
	}

	if( !pd.light.iface_name) {
		pd.light.iface = blc_get_first_iface(&err);		
		if( !pd.light.iface ) {
			LOG(LOG_CRIT, "no interface found\n");
			exit(EERR_BAD_IFACE);
		}
	} else {
		pd.light.iface = blc_construct_iface_by_name(pd.light.iface_name,&err);
		if( !pd.light.iface ) {
			LOG(LOG_CRIT, "bad interface specified.\n");
			exit(EERR_BAD_IFACE);
		}
	}

	LOG(LOG_INFO, "Selected interface %s for control.\n", pd.light.iface);

	umask(0);
	pd.pid_fd = lock_pid_file(pd.pid_file);
	if(pd.pid_fd == -1) {
		LOG(LOG_CRIT, "locking pid file failed.\n");
		exit(EERR_LOCK_PIDFILE);
	}

	if(pd.flag.daemon) {
		if(daemonize() == -1) {
			LOG(LOG_CRIT, "Daemonized failed. Exiting.\n");
			exit(EERR_DAEMONIZE_FAILED);
		}
		LOG(LOG_INFO, "Daemonized successfull.\n");
		pd.daemonized=1;
		if(reopen_stdio() != -1) {
			LOG(LOG_INFO, "Reopened stdio descripters successfully.\n");
		} else {
			LOG(LOG_CRIT, "reopen stdio descripters failed.\n");
			exit(EERR_REOPEN_DESCRIPTERS);
		}
	}

	// flock сбрасывается на fork()
	// => нам надо переоткрыть файл
	close(pd.pid_fd);

	pd.pid_fd = lock_pid_file(pd.pid_file);
	if(pd.pid_fd == -1) {
		LOG(LOG_CRIT, "locking pid file failed.\n");
		exit(EERR_LOCK_PIDFILE);
	}
	
	atexit(end_backlightd); // сопли

	if(signal(SIGTERM, on_sigterm)==SIG_ERR) {
		LOG(LOG_CRIT, "failed to handle sigterm.\n");
		exit(EERR_CREATE_FIFO);
	}
	
	if(pd.flag.fifo_ctl) {
		pd.fifo.fd = open_ctl_fifo(pd.fifo.name);
	}
	if(pd.flag.socket_ctl) {
		pd.socket.id = open_ctl_socket(pd.socket.l_name, &pd.socket.l_addr);
	}

	if( !((pd.flag.socket_ctl && pd.socket.id == -1) ||
	      (pd.flag.fifo_ctl && pd.fifo.fd == -1)) ) {
		LOG(LOG_CRIT, "all specified control interfaces are failed to be created.\n");
		exit(EERR_CREATE_FIFO);
	}

	backlightd_loop();
	//TODO: return and free the memory for interface name
}

void end_backlightd() {
	if(pd.pid_fd!=-1) {
		if(close(pd.pid_fd) < 0) {
			LOG(LOG_ERR, "close pid file failed with error %d: %s\n",
				errno, strerror(errno));
		} else {
			LOG(LOG_INFO, "pid file closed successfully\n");
		}
	}

	if(pd.pid_file) {
		if(unlink(pd.pid_file) < 0) {
			LOG(LOG_ERR, "remove pid file failed with error %d: %s\n",
				errno, blc_strerror(errno));
		} else {
			LOG(LOG_INFO, "pid file removed successfully\n");
		}
	}
	if(pd.light.iface) {
		free(pd.light.iface);
	}
}

void on_sigterm(int sig) {
	exit(0);
}

void backlightd_loop() {
	FILE *fifo_fs=fdopen(pd.fifo.fd, "r");
	while(1) {
		fifo_fs = fdopen(pd.fifo.fd, "r");

		if( fifo_fs == 0 ) {
			LOG(LOG_CRIT, "fdopen control fifo failed with error %d:%s\n", 
					errno, strerror(errno));
			exit(EERR_FDOPEN_FIFO);
		}

		if( execute_fs_content(fifo_fs) == -1 ) { // function returns on EOF
			LOG(LOG_CRIT, "read from control pipe failed.\n");
			exit(EERR_FCLOSE_FIFO);
		}

		if(fclose(fifo_fs) != 0) {
			LOG(LOG_CRIT, "fclose control fifo failed with error %d:%s\n", 
					errno, strerror(errno));
			exit(EERR_FCLOSE_FIFO);
		}

		pd.fifo.fd = open_ctl_fifo(pd.fifo.name);
	}
}

error_t execute_fs_content(FILE* fs) {
	char buf[COMMAND_BUFF_SZ];
	char *rc;

	do {
		rc = fgets(buf, COMMAND_BUFF_SZ, fs);
		if( !rc ) {
			break;
		}

		execute_str_command(buf);

	} while(1);

	if(errno) {
		LOG(LOG_ERR, "failed to read from file with error %d:%s\n", 
			errno, strerror(errno));
		return -1;
	}
	return 0;
}

error_t execute_str_command(const char *str) {
	const char *cmd_start;
	char cmd;
	int err=0, rc=0;
	long new_brt, max_brt, cur_brt;
	
	for( cmd_start=str; cmd_start!=0 && isspace(*cmd_start); cmd_start++) {
		// < search for a first non-space symbol
	}

	max_brt = blc_get_max_brightness(pd.light.iface, &err);
	if(err) {
		LOG(LOG_ERR, "geting max brightness failed with error %d: %s\n",
				err, blc_strerror(err));
		errno = 0;
		return -1;
	}

	cur_brt = blc_get_brightness(pd.light.iface, &err);
	if(err) {
		LOG(LOG_ERR, "geting current brightness failed with error %d: %s\n",
				err, blc_strerror(err));
		errno = 0;
		return -1;
	}
	
	cmd = get_str_command(cmd_start);
	if( cmd==0 ) {
		LOG(LOG_ERR, "bad command: %s\n", cmd_start);
		return -1;

	}

	if( strchr("s.", cmd) ) {
		rc = sscanf(cmd_start, "%ld", &new_brt);
	} else if(strchr("di-+", cmd)) {
		rc = sscanf(cmd_start+1, "%ld", &new_brt);
	}
		
	if( rc!=1 ) {
		LOG(LOG_ERR, "bad command: %s", cmd_start);
		return -1;

	}

	if( strchr("sid", cmd) ) {
		new_brt = (new_brt * max_brt) / 100;
	}

	if( strchr("i+", cmd) ) {
		new_brt = cur_brt + new_brt;
	} else if( strchr("d-", cmd) ) {
		new_brt = cur_brt - new_brt;
	}
	if( new_brt > max_brt ) {
		new_brt = max_brt;
	} else if( new_brt < 0 ) {
		new_brt = 0;
	}
	
	blc_set_brightness(pd.light.iface, new_brt, &err);
	if(err) {
		LOG(LOG_ERR, "setting brightness failed with error %d: %s",
				err, blc_strerror(err));
		errno = 0;
		return -1;
	}

	return 0;
}

char get_str_command(const char *str) {
	switch(str[0]) {
		case '+':
			if( strchr(str, '%') ) {
				return 'i';
			} else {
				return '+';
			}
		case '-':
			if( strchr(str, '%') ) {
				return 'd';
			} else {
				return '-';
			}
		default:
			if( isdigit(str[0]) ) {
				if( strchr(str, '%') ) {
					return 's';
				} else {
					return '.';
				}
			} else 
				return 0;
	}
}

#if 0
char *strnspn(const char *s, const char *naccept) {
	size_t i;
	for( i=0; s[i]!=0 && !strchr(naccept, s[i]); i++) {}
	return s + i;
}
#endif // 0

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

int open_ctl_fifo(const char *name) {
	int rv;
	struct stat sb;

	rv = stat(name, &sb);
	if( rv == -1 ) {
		rv = mkfifo(name, 0620);
		if( rv == -1 ) {
			LOG(LOG_ERR, "failed to create fifo %s becouse of error %d:%s\n",
				name, errno, strerror(errno));
		}
		return rv;
	} else if(!S_ISFIFO(sb.st_mode)) {
		LOG(LOG_ERR, "file %s exists but is not a fifo\n",	name);
		rv = -1;
		return rv;
	} else if (	(rv = open(name, O_RDONLY)) == -1 ) {
		LOG(LOG_ERR, "open file %s failed with error %d:%s\n",
				name, errno, strerror(errno));
	}
	return rv;
}

int open_ctl_socket(const char *name, struct addrinfo *addr) {
	int rv, rc;
	struct addrinfo hints;
	struct addrinfo *addr_res, *addr_p;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;     /* For wildcard IP address */
	hints.ai_protocol = 0;	         /* Any protocol */

	rc = getaddrinfo(NULL, name, &hints, &addr_res);
	if( rc == 0 ) {
		LOG(LOG_ERR, "getaddrinfo failed with error %d:%s\n",
				rc, gai_strerror(rc));

		return -1;
	}

	for( addr_p = addr_res; addr_p != 0; addr_p = addr_p->ai_next) {
		rv = socket(addr_p->ai_family, addr_p->ai_socktype,	addr_p->ai_protocol);
    	if (rv != -1) {
			if (bind(rv, addr_p->ai_addr, addr_p->ai_addrlen) == -1) {
				close(rv);
			} else {
				break; // Success
			}
		}
	}

	if( addr_p == 0 ) {
		LOG(LOG_ERR, "failed to create a server socket for specified address\n");
		rv = -1;
	} else {
		memcpy(addr_p, addr, sizeof(*addr));
	}
	freeaddrinfo(addr_p);
	return rv;
}

int lock_pid_file(const char *name) {
	int rv=0;
	rv = open(name, O_WRONLY | O_CREAT | O_NOCTTY | O_TRUNC, 0644);
	if( rv == -1 ) {
		LOG(LOG_ERR, "open file %s failed with error %d:%s\n",
				name, errno, strerror(errno));
	} else {
		char buf[32]="";
		int len;
		snprintf(buf, 32, "%d\n", getpid());
		len = strlen(buf);
		if(write(rv, buf, len)!=len) {
			LOG(LOG_ERR, "write lock file %s failed with error %d:%s\n",
				name, errno, strerror(errno));
			close(rv);
		} else if( lockf(rv, F_TLOCK, 0) == -1) {
			LOG(LOG_ERR, "lock file %s failed with error %d:%s\n",
					name, errno, strerror(errno));
			close(rv);
			rv = -1;
		}
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
