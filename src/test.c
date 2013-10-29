/* Decode SiRF Binary Protocol (u-Blox GPS) */

#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#define DECODE_ECEF	1			/* only for old firmware */

#define BUFLEN		2048

#define START1		0xa0
#define START2		0xa2
#define END1		0xb0
#define END2		0Xb3

/*
cmd> [command area]                                                        row 0
             X        Y        Z            North      East         Alt        1
 Pos: -1234567 -1234567 -1234567 m       xx.xxxxx  xx.xxxxx deg xxxxxxx m      2
 Vel:      0.0      0.0      0.0 m/s          0.0       0.0         0.0 m/s    3
 Time: xxxx xxxxxx.xx x xx:xx:xx.xx         Heading:  xxx.x deg xxxxx.x m/s    4
                                                                               5
 DOP: xx.x M1: xx M2: xx Fix: x                                                6
 Max: x.xxx Lat: x.xxx Avg: x.xxx MS: xx                                       7
                                                                               8
Ch SV  Az El Stat  C/N                                                         9
xx xx xxx xx xxxx xx.x                                                        10
*/

#define CHANWIN		10
#define DEBUGWIN	23

int LineFd;					/* fd for RS232 line */
int verbose;
int debugx,debugy;
int nfix,fix[20];
int ix = 3913707,iy = 351947,iz = 5007145,iweek = 1321,itow = 0,iclk = 82914; /* for Init */
FILE *logfile;

char *verbpat[] =
{
    "#Time:",
    "@R Time:",
    "CSTD: New almanac for",
    "NOTICE: DOP Q Boost",
    "RTC not set",
    "numOfSVs = 0",
    "rtcaj tow ",
    NULL
};

#define getb(off)	(buf[off])
#define getw(off)	((short)((getb(off) << 8) | getb(off+1)))
#define getl(off)	((int)((getw(off) << 16) | (getw(off+2) & 0xffff)))

#define putb(off,b)	{ buf[4+off] = (unsigned char)(b); }
#define putw(off,w)	{ putb(off,(w) >> 8); putb(off+1,w); }
#define putl(off,l)	{ putw(off,(l) >> 16); putw(off+2,l); }

#define RAD2DEG		5.729577795E-7		/* RAD/10^8 to DEG */

void decode_sirf(unsigned char buf[],int len);
void decode_time(int week, int tow);
#ifdef DECODE_ECEF
void decode_ecef(double x, double y, double z, double vx, double vy, double vz);
#endif
int openline (char *name,int baud);
int sendpkt (unsigned char *buf,int len);
int readpkt (unsigned char *buf);

int
main (argc, argv)
int argc;
char *argv[];

{
    int len,i,v,quit = 0;
    char *p;
    fd_set select_set;
    unsigned char buf[BUFLEN];
    char line[80];

    if (argc < 2) {
	fprintf(stderr,"Usage: %s <tty-device>\n",argv[0]);
	exit(1);
    }

    if (openline(argv[1],B19200))
	exit(2);

#ifdef NTPSHM
    ntpshm_init();
#endif

    initscr();
    cbreak();
    noecho();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    scrollok(stdscr,TRUE);
    setscrreg(DEBUGWIN,LINES - 1);

    attrset(A_BOLD);
    move(1,1);
    printw("            X        Y        Z            North      East         Alt");
    move(2,1);
    printw("Pos:                            m                          deg         m");
    move(3,1);
    printw("Vel:                            m/s                                    m/s");
    move(4,1);
    printw("Time:                                      Heading:        deg         m/s");
    move(6,1);
    printw("DOP:      M1:    M2:    Fix:  ");
    move(7,1);
    printw("Max:       Lat:       Avg:       MS:");
    move(CHANWIN-1,0);
    printw("Ch SV  Az El Stat  C/N");
    for (i = 0; i < 12; i++) {
	move(CHANWIN+i,0);
	printw("%2d",i);
    }
    attrset(A_NORMAL);

    move(DEBUGWIN,0);
    getyx(stdscr,debugy,debugx);

    FD_ZERO(&select_set);

    while (!quit)
    {
	move(0,0);
	printw("cmd> ");
	clrtoeol();
	refresh();

	FD_SET(0,&select_set);
	FD_SET(LineFd,&select_set);

	if (select(LineFd + 1,&select_set,NULL,NULL,NULL) < 0)
	    break;

	if (FD_ISSET(0,&select_set)) {
	    move(0,5);
	    refresh();
	    echo();
	    getstr(line);
	    noecho();
	    move(0,0);
	    clrtoeol();
	    refresh();

	    if ((p = strchr(line,'\r')) != NULL)
		*p = '\0';

	    if (!line[0])
		continue;

	    p = line;

	    while (*p != '\0' && !isspace(*p))
		p++;
	    while (*p != '\0' && isspace(*p))
		p++;

	    switch (line[0])
	    {
	    case 'A':
		memset(buf,0,sizeof(buf));
		putb(0,0x82);			/* almanac */
		sendpkt(buf,897);
		break;

	    case 'b':
		v = B19200;
		if (*p == '3')
		    v = B38400;
		if (*p == '9')
		    v = B9600;

		close(LineFd);

		if (openline(argv[1],v))
		    exit(2);

		break;

	    case 'I':
		v = 0x23;
		switch (*p)
		{
		case 'C':			/* COLD init */
		    ix = iy = iz = 0;
		    v = 0x24;
		    break;
		case 'F':			/* FACTORY init */
		    ix = iy = iz = 0;
		    iclk = itow = iweek = 0;
		    v = 0x24;
		    break;
		}
		putb(0,0x80);			/* initialize data source */
		putl(1,ix);			/* X guess */
		putl(5,iy);			/* Y guess */
		putl(9,iz);			/* Z guess */
		putl(13,iclk);			/* clock offset */
		putl(17,itow);			/* TOW */
		putw(21,iweek);			/* week */
		putb(23,12);			/* channels */
		putb(24,v);			/* reset configuration */
		sendpkt(buf,25);
		break;

	    case 'i':
		putb(0,0x8b);			/* elevation mask */
		putw(1,20);			/* tracking */
		if (isdigit(*p))
		    v = 10 * atoi(p);
		else
		    v = 100;
		putw(3,v);			/* navigation */
		sendpkt(buf,5);
		usleep(500000);

		putb(0,0x8c);			/* power mask */
		putb(1,28);			/* tracking */
		putb(2,28);			/* navigation */
		sendpkt(buf,3);
		break;

	    case 'u':
		memset(buf,0,sizeof(buf));
		putb(0,0xa5);			/* set UART config */
		putl(4,19200);			/* port 0 = 19200 */
		putb(8,8);
		putb(9,1);

		putb(13,1);			/* port 1 = 4800 NMEA */
		putb(14,1);
		putb(15,1);
		putl(16,4800);
		putb(20,8);
		putb(21,1);

		putb(25,2);			/* port 2 off */
		putb(26,5);
		putb(27,5);

		putb(37,3);			/* port 3 off */
		putb(38,5);
		putb(39,5);
		sendpkt(buf,49);
		break;

	    case 'l':				/* open logfile */
		if (logfile != NULL)
		    fclose(logfile);

		logfile = fopen(p,"a");
		break;

	    case 'q':
		quit++;
		break;

	    case 's':
		len = 0;

		while (*p != '\0')
		{
		    sscanf(p,"%x",&v);
		    putb(len,v);
		    len++;
		    while (*p != '\0' && !isspace(*p))
			p++;
		    while (*p != '\0' && isspace(*p))
			p++;
		}

		sendpkt(buf,len);
		break;

	    case 'v':
	    	verbose = (*p == '0')? 0 : 1;
		break;
	    }
	}

	if (/* FD_ISSET(LineFd,&select_set) && */ (len = readpkt(buf)) != EOF)
	    decode_sirf(buf,len);
    }

    if (logfile != NULL)
	fclose(logfile);

    endwin();
    exit(0);
}

/* SiRF high level routines */

void
decode_sirf(buf,len)
unsigned char buf[];
int len;

{
    int i,j,ch,off,cn;

    switch (buf[0])
    {
    case 0x02:
	move(2,6);
	printw("%8d %8d %8d",getl(1),getl(5),getl(9));
	move(3,6);
	printw("%8.1f %8.1f %8.1f",
		(float)getw(13)/8,(float)getw(15)/8,(float)getw(17)/8);
#ifdef DECODE_ECEF
	decode_ecef((double)getl(1),(double)getl(5),(double)getl(9),
		(float)getw(13)/8,(float)getw(15)/8,(float)getw(17)/8);
#endif
	move(6,6);
	printw("%4.1f",(float)getb(20)/5);
	move(6,15);
	printw("%02x",getb(19));
	move(6,22);
	printw("%02x",getb(21));
	decode_time(getw(22),getl(24));
	move(6,30);
	nfix = getb(28);
	printw("%d",nfix);
	for (i = 0; i < nfix; i++) {
	    printw("%3d",fix[i] = getb(29+i));
	}
	clrtoeol();
	if (nfix >= 4) {
	    ix = getl(1);
	    iy = getl(5);
	    iz = getl(9);
	}
	break;

    case 0x04:
	decode_time(getw(1),getl(3));
	ch = getb(7);
	for (i = 0; i < ch; i++) {
	    int sv,st;

	    off = 8 + 15 * i;
	    move(CHANWIN+i,2);
	    sv = getb(off);
	    printw("%3d",sv);

	    printw(" %3d%3d %04x",(getb(off+1)*3)/2,getb(off+2)/2,getw(off+3));

	    st = ' ';
	    if (getw(off+3) == 0xbf)
		st = 'T';
	    for (j = 0; j < nfix; j++)
		if (sv == fix[j]) {
		    st = 'N';
		    break;
		}

	    cn = 0;

	    for (j = 0; j < 10; j++)
		cn += getb(off+5+j);

	    printw("%5.1f %c",(float)cn/10,st);

	    if (sv == 0)			/* not tracking? */
		clrtoeol();			/* clear other info */
	}
	putb(0,0x90);				/* poll clock status */
	putb(1,0);
	sendpkt(buf,2);
    	break;

    case 0x05:
	for (off = 1; off < len; off += 51) {
	    ch = getl(off);
	    move(CHANWIN+ch,19);
	    cn = 0;

	    for (j = 0; j < 10; j++)
		cn += getb(off+34+j);

	    printw("%5.1f",(float)cn/10);

	    printw("%9d%3d%5d",getl(off+8),getw(off+12),getw(off+14));
	    printw("%8.5f %10.5f",
	    	(float)getl(off+16)/65536,(float)getl(off+20)/1024);
	}
    	break;

    case 0x06:
	move(8,0);
	printw("06 %s",buf + 1);
    	break;

    case 0x07:
	decode_time(getw(1),getl(3));
	move(7,44);
	printw("%2d %lu %lu %lu",getb(7),getl(8),getl(12),getl(16));
	iclk = getl(8);
	clrtoeol();
	break;

    case 0x08:
	ch = getb(1);
	move(CHANWIN+ch,77);
	printw("A");
	if (verbose) {
	    move(debugy,debugx);
	    printw("ALM %d (%d):",getb(2),ch);
	    for (off = 3; off < len; off += 4)
		printw(" %d",getl(off));
	    printw("\n");
	    getyx(stdscr,debugy,debugx);
	}
    	break;

    case 0x09:
	move(7,6);
	printw("%.3f",(float)getw(1)/186);
	move(7,17);
	printw("%.3f",(float)getw(3)/186);
	move(7,28);
	printw("%.3f",(float)getw(5)/186);
	move(7,38);
	printw("%3d",getw(7));
    	break;

    case 0x0a:
	break;

    case 0x0b:
	move(8,0);
	printw("ACK %02x",getb(1));
    	break;

    case 0x0c:
	move(8,0);
	printw("NAK %02x",getb(1));
    	break;

    case 0x0d:
	move(debugy,debugx);
	printw("vis %d:",getb(1));
	for (i = 0; i < getb(1); i++) {
	    off = 2 + 5 * i;
	    printw(" %d",getb(off));
	}
	printw("\n");
	getyx(stdscr,debugy,debugx);
    	break;

    case 0x0e:
    	break;

    case 0x0f:
    	break;

    case 0x11:
    	break;

    case 0x12:
    	break;

    case 0x13:
    	break;

    case 0x62:
	attrset(A_BOLD);
	move(2,40);
	printw("%9.5f %9.5f",RAD2DEG*getl(1),RAD2DEG*getl(5));
	move(2,63);
	printw("%8d",getl(9)/1000);

	move(3,63);
	printw("%8.1f",(float)getl(17)/1000);

	move(4,54);
	if (getl(13) > 50) {
	    float heading = RAD2DEG*getl(21);
	    if (heading < 0)
		heading += 360;
	    printw("%5.1f",heading);
	} else
	    printw("  0.0");

	move(4,63);
	printw("%8.1f",(float)getl(13)/1000);
	attrset(A_NORMAL);

	move(5,13);
	printw("%04d-%02d-%02d %02d:%02d:%02d.%02d",
		getw(26),getb(28),getb(29),getb(30),getb(31),
		(unsigned short)getw(32)/1000,
		((unsigned short)getw(32)%1000)/10);
	{
	    struct timeval clk,gps;
	    struct tm tm;

	    gettimeofday(&clk,NULL);

	    memset(&tm,0,sizeof(tm));
	    tm.tm_sec = (unsigned short)getw(32)/1000;
	    tm.tm_min = getb(31);
	    tm.tm_hour = getb(30);
	    tm.tm_mday = getb(29);
	    tm.tm_mon = getb(28) - 1;
	    tm.tm_year = getw(26) - 1900;
	    tm.tm_isdst = -1;

	    gps.tv_sec = mktime(&tm) - timezone + (daylight ? 3600 : 0 );
	    gps.tv_usec = ((unsigned short)getw(32)%1000) * 1000;

#ifdef NTPSHM
	    ntpshm_put(gps.tv_sec + gps.tv_usec / 1000000.0 + 1.013);
#endif
	    move(5,2);
	    printw("           ");
	    move(5,2);
#if 1
	    printw("%ld",(gps.tv_usec - clk.tv_usec) +
	    		 (gps.tv_sec - clk.tv_sec) * 1000000);
#else
	    printw("%ld %ld %ld %ld",gps.tv_sec % 3600,gps.tv_usec,
	    			     clk.tv_sec % 3600,clk.tv_usec);
#endif
	}
    	break;

    case 0xff:
	move(debugy,debugx);
	while (len > 0 && buf[len-1] == '\n')
	    len--;
	while (len > 0 && buf[len-1] == ' ')
	    len--;
	buf[len] = '\0';
	j = 1;
	if (!verbose) {
	    for (i = 0; verbpat[i] != NULL; i++)
		if (!strncmp(buf+1,verbpat[i],strlen(verbpat[i]))) {
		    j = 0;
		    break;
		}
	}
	if (j)
	    printw("%s\n",buf+1);
	getyx(stdscr,debugy,debugx);
	break;

    default:
	move(8,0);
	printw(" %02x: ",buf[0]);

	if (len > 20)
	    len = 20;
	for (i = 1; i < len; i++)
	    printw("%02x",buf[i]);

	clrtoeol();
	break;
    }
}

void
decode_time(week,tow)
int week,tow;

{
    int day = tow / 8640000;
    int tod = tow % 8640000;
    int h = tod / 360000;
    int m = tod % 360000;
    int s = m % 6000;

    m = (m - s) / 6000;

    move(4,7);
    printw("%4d %9.2f %d %02d:%02d:%05.2f",week,(double)tow/100,day,
    	h,m,(float)s/100);

    if (week != 986 && week > iweek) {
	iweek = week;
	itow = tow;
    }
}

#ifdef DECODE_ECEF
void
decode_ecef(x,y,z,vx,vy,vz)
double x,y,z,vx,vy,vz;

{
    const double a = 6378137;
    const double f = 1 / 298.257223563;
    const double b = a * (1 - f);
    const double e2 = (a*a - b*b) / (a*a);
    const double e_2 = (a*a - b*b) / (b*b);
    double lambda,p,theta,phi,n,h,vnorth,veast,vup,speed,heading;

    lambda = atan2(y,x);
    p = sqrt(pow(x,2) + pow(y,2));
    theta = atan2(z*a,p*b);
    phi = atan2(z + e_2*b*pow(sin(theta),3),p - e2*a*pow(cos(theta),3));
    n = a / sqrt(1.0 - e2*pow(sin(phi),2));
    h = p / cos(phi) - n;
    vnorth = -vx*sin(phi)*cos(lambda)-vy*sin(phi)*sin(lambda)+vz*cos(phi);
    veast = -vx*sin(lambda)+vy*cos(lambda);
    vup = vx*cos(phi)*cos(lambda)+vy*cos(phi)*sin(lambda)+vz*sin(phi);
    speed = sqrt(pow(vnorth,2) + pow(veast,2));
    heading = atan2(veast,vnorth);
    if (heading < 0)
	heading += 6.283185307;

    move(2,40);
    printw("%9.5f %9.5f",57.29577795*phi,57.29577795*lambda);
    move(2,63);
    printw("%8d",(int)h);

    move(3,40);
    printw("%9.1f %9.1f",vnorth,veast);
    move(3,63);
    printw("%8.1f",vup);

    move(4,54);
    printw("%5.1f",57.29577795*heading);
    move(4,63);
    printw("%8.1f",speed);

    if (logfile != NULL)
	fprintf(logfile,"%d\t%d\t%d\t%d\t%f\t%f\t%.2f\n",
		(int)time(NULL),(int)x,(int)y,(int)z,57.29577795*phi,57.29577795*lambda,h);
}
#endif

/* RS232-line routines (initialization and SiRF pkt send/receive) */
/* addition: when gpsd is running on localhost, connect it instead */

int
openline (name,baud)
char *name;
int baud;

{
    struct hostent *phe;
    struct servent *pse;
    struct sockaddr_in sin;
    struct termios tios;
    char buf[BUFLEN];

    memset((char *) &sin, '\0', sizeof(sin));
    sin.sin_family = AF_INET;

    if ((pse = getservbyname("gpsd", "tcp")))
	sin.sin_port = htons(ntohs((u_short) pse->s_port));

    if ((phe = gethostbyname("localhost")))
	memcpy((char *) &sin.sin_addr, phe->h_addr, phe->h_length);

    if ((LineFd = socket(PF_INET, SOCK_STREAM, 0)) >= 0 &&
	connect(LineFd, (struct sockaddr *) &sin, sizeof(sin)) == 0)
    {
	snprintf(buf,sizeof(buf),"F=%s\r\n",name);
	write(LineFd,buf,strlen(buf));
	read(LineFd,buf,sizeof(buf));
	snprintf(buf,sizeof(buf),"O\r\n");
	write(LineFd,buf,strlen(buf));
	read(LineFd,buf,sizeof(buf));
	snprintf(buf,sizeof(buf),"R=2\r\n");
	write(LineFd,buf,strlen(buf));
	read(LineFd,buf,sizeof(buf));
	return 0;
    }
    else
	close(LineFd);

    if ((LineFd = open(name,O_RDWR)) < 0) {
	perror(name);
	return 1;
    }

    if (ioctl(LineFd,TCGETS,&tios) < 0) {
	perror(name);
	return 1;
    }

    tios.c_iflag = IGNBRK|IGNPAR;
    tios.c_oflag = 0;
    tios.c_cflag = baud|CS8|CREAD|CLOCAL;
    tios.c_lflag = 0;
    tios.c_line = N_TTY;
    memset(tios.c_cc,0,sizeof(tios.c_cc));
    tios.c_cc[VMIN] = 1;

    if (ioctl(LineFd,TCSETS,&tios) < 0) {
	perror(name);
	return 1;
    }

    return 0;
}

int
readbyte ()

{
    static int cnt = 0,pos = 0;
    static unsigned char inbuf[BUFLEN];

    if (pos >= cnt) {
	fd_set select_set;
	struct timeval timeval;

	FD_ZERO(&select_set);
	FD_SET(LineFd,&select_set);
	timeval.tv_sec = 0;
	timeval.tv_usec = 500000;

	if (select(LineFd + 1,&select_set,NULL,NULL,&timeval) < 0)
	    return EOF;

	if (!FD_ISSET(LineFd,&select_set))
	    return EOF;

	usleep(100000);

	if ((cnt = read(LineFd,inbuf,BUFLEN)) <= 0)
	    return EOF;

	pos = 0;
    }

    return inbuf[pos++];
}

int readword ()

{
    int byte1,byte2;

    if ((byte1 = readbyte()) == EOF || (byte2 = readbyte()) == EOF)
	return EOF;

    return (byte1 << 8) | byte2;
}

int
readpkt (buf)
unsigned char *buf;

{
    int byte,len,csum,cnt;

    do {
	while ((byte = readbyte()) != START1)
	    if (byte == EOF)
		return EOF;
    } while ((byte = readbyte()) != START2);

    if ((len = readword()) == EOF || len > BUFLEN)
	return EOF;

    csum = 0;
    cnt = len;

    while (cnt-- > 0) {
	if ((byte = readbyte()) == EOF)
	    return EOF;
	*buf++ = byte;
	csum += byte;
    }

    csum &= 0x7fff;

    if (readword() != csum)
	return EOF;

    if (readbyte() != END1 || readbyte() != END2)
	return EOF;

    return len;
}

int
sendpkt (buf,len)
unsigned char *buf;
int len;

{
    int i,csum;

    putb(-4,START1);			/* start of packet */
    putb(-3,START2);
    putw(-2,len);			/* length */

    csum = 0;
    for (i = 0; i < len; i++)
	csum += buf[4 + i];

    csum &= 0x7fff;
    putw(len,csum);			/* checksum */
    putb(len + 2,END1);			/* end of packet */
    putb(len + 3,END2);
    len += 8;

    if (verbose) {
	move(debugy,debugx);
	printw(">>>");
	for (i = 0; i < len; i++)
	    printw(" %02x",buf[i]);
	printw("\n");
	getyx(stdscr,debugy,debugx);
    }

    return (write(LineFd,buf,len) == len);
}
