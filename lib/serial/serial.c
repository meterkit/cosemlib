/*
 * OS independent serial interface
 *
 * Heavily based on Pirate-Loader:
 * http://the-bus-pirate.googlecode.com/svn/trunk/bootloader-v4/pirate-loader/source/pirate-loader.c
 *
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <string.h>

#include "serial.h"

int serial_setup(int fd, unsigned long speed)
{
#if USE_WINDOWS_OS
	COMMTIMEOUTS timeouts;
	DCB dcb = {0};
	HANDLE hCom = (HANDLE)fd;

	dcb.DCBlength = sizeof(dcb);

	dcb.BaudRate = speed;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;

	if( !SetCommState(hCom, &dcb) ){
		return -1;
	}

	timeouts.ReadIntervalTimeout = 100;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.ReadTotalTimeoutConstant = 100;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 100;

	if (!SetCommTimeouts(hCom, &timeouts)) {
		return -1;
	}

	return 0;
#else
	struct termios t_opt;
	speed_t baud;

	switch (speed) {
		case 921600:
			baud = B921600;
			break;
		case 2400:
            baud = B2400;
            break;
		case 4800:
            baud = B4800;
            break;
		case 9600:
		    baud = B9600;
		    break;
		case 115200:
			baud = B115200;
			break;
		case 1000000:
			baud = B1000000;
			break;
		case 1500000:
			baud = B1500000;
			break;
		default:
			printf("unknown speed setting \n");
			return -1;
			break;
	}

	/* set the serial port parameters */
	fcntl(fd, F_SETFL, 0);
	tcgetattr(fd, &t_opt);
	cfsetispeed(&t_opt, baud);
	cfsetospeed(&t_opt, baud);
	t_opt.c_cflag |= (CLOCAL | CREAD);
	t_opt.c_cflag &= ~PARENB;
	t_opt.c_cflag &= ~CSTOPB;
	t_opt.c_cflag &= ~CSIZE;
	t_opt.c_cflag |= CS8;
	t_opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	t_opt.c_iflag &= ~(IXON | IXOFF | IXANY);
	t_opt.c_iflag &= ~(ICRNL | INLCR);
	t_opt.c_oflag &= ~(OCRNL | ONLCR);
	t_opt.c_oflag &= ~OPOST;
	t_opt.c_cc[VMIN] = 0;
	t_opt.c_cc[VTIME] = 10;

#if IS_DARWIN
	if( tcsetattr(fd, TCSANOW, &t_opt) < 0 ) {
		return -1;
	}

	return ioctl( fd, IOSSIOSPEED, &baud );
#else
	tcflush(fd, TCIOFLUSH);

	return tcsetattr(fd, TCSANOW, &t_opt);
#endif
#endif
}

int serial_write(int fd, const char *buf, int size)
{
	int ret = 0;
#if USE_WINDOWS_OS
	HANDLE hCom = (HANDLE)fd;
	int res = 0;
	unsigned long bwritten = 0;

	res = WriteFile(hCom, buf, size, &bwritten, NULL);

	if( res == FALSE ) {
		ret = -1;
	} else {
		ret = bwritten;
	}
#else
	ret = write(fd, buf, size);
#endif

#if DEBUG
	if (ret != size)
		fprintf(stderr, "Error sending data (written %d should have written %d)\n", ret, size);
#endif

	return ret;
}

int serial_read(int fd, char *buf, int size, int timeout)
{
	int len = 0;
	int ret = 0;

#if USE_WINDOWS_OS
	HANDLE hCom = (HANDLE)fd;
	unsigned long bread = 0;

	ret = ReadFile(hCom, buf, size, &bread, NULL);

	if( ret == FALSE || ret==-1 ) {
		len = -1;
	} else {
		len = bread;
	}

#else

	fd_set readfs;
	int    maxfd;     /* maximum file desciptor used */
	maxfd = fd + 1;  /* maximum bit entry (fd) to test */

    struct timeval Timeout;

    /* set timeout value within input loop */
    Timeout.tv_usec = 0;  /* milliseconds */
    Timeout.tv_sec  = timeout;  /* seconds */

    FD_ZERO(&readfs);
    FD_SET(fd, &readfs);  /* set testing for source 1 */

    ret = select(maxfd, &readfs, NULL, NULL, &Timeout);

    if (ret)
    {
        if (FD_ISSET(fd, &readfs))
        {
            ret = read(fd, buf, size);
            len = ret;
        }
    }
    else
    {
        // Timeout
        len = -1;
    }

#endif

#if DEBUG
	if (len != size)
		fprintf(stderr, "Error receiving data (read %d should have read %d)\n", len, size);
#endif

	return len;
}

int serial_open(const char *port)
{
	int fd = -1;
#if USE_WINDOWS_OS
	static char full_path[32] = {0};

	HANDLE hCom = NULL;

	if( port[0] != '\\' ) {
		_snprintf(full_path, sizeof(full_path) - 1, "\\\\.\\%s", port);
		port = full_path;
	}

	hCom = CreateFileA(port, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if( !hCom || hCom == INVALID_HANDLE_VALUE ) {
		fd = -1;
	} else {
		fd = (int)hCom;
	}
#else
	fd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd == -1) {
		//fprintf(stderr, "Could not open serial port.\n");
		return -1;
	}

	/* Make the file descriptor asynchronous (the manual page says only        |
	|           O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
	fcntl(fd, F_SETFL, FASYNC);
#endif
	return fd;
}

int serial_close(int fd)
{
#if USE_WINDOWS_OS
	HANDLE hCom = (HANDLE)fd;

	CloseHandle(hCom);
#else
	close(fd);
#endif
	return 0;
}
