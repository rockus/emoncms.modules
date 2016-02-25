//#include <ctype.h>	// for isprint
#include <stdio.h>	// for sprintf, printf, fprintf
#include <string.h>	// for strerror, strlen, index, strncmp, strncpy, strcmp, strcpy
#include <stdlib.h>	// for strol, strtod, exit, abort
#include <unistd.h>	// for close, read, getopt, opterr
#include <fcntl.h>	// for open, fcntl, O_RDWR, O_NOCTTY, O_NONBLOCK, F_SETFL
//#include <sys/ioctl.h>	// for ioctl
#include <errno.h>	// for errno
//#include <termios.h>	// for struct termios, tcgetattr, cfmakeraw, cfsetspeed, tcsetattr, VMIN, VTIMES, CSB, tcdrain
#include <sysexits.h>	// for EX_IOERR, EX_OK, EX_UNAVAILABLE
#include <sys/param.h>	// for MAXPATHLEN
//#include <time.h>	// for ctime, time
#include <signal.h>	// for sigaction, SIGINT

#include <sys/socket.h>	// for socket
#include <arpa/inet.h>	// for sockaddr_in
#include <netdb.h>	// for gethostbyname

#define VERSION "v1.01"
#define TOOLNAME "Huawei emonCMS"
#define COPYRIGHT "(c)2015,2016 Oliver Gerler (rockus@rockus.at)"

// data from GSM/UMTS stick
struct data
{
	char network[16];	// provider network
	int mcc;			// mobile country code
	int mnc;			// mobile network code
	long totDown;		// total download
	long totUp;			// total upload
	long monthDown;		// total download
	long monthUp;		// total upload
	long dataLimit;		// data limit per month (as configured in web interface of GSM stick)
};
