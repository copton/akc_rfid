#include "reader.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>

#define BAUDRATE B38400

static int fd;
static Reader_Config* cfg;

typedef struct {
	unsigned char* data;
	size_t length;
} Buffer;

static unsigned int crc16(unsigned char* data, size_t length)
{
	int i,j;
	unsigned int crc = 0xFFFF;
	for (i=0;i<length;i++) {
		crc ^= data[i];
		for (j=0;j<8;j++) {
			if(crc & 0x0001)
				crc = (crc >> 1) ^ 0x8408;
			else
				crc = (crc >> 1);
		}
	}
	return crc;
}

static void execute_feig(unsigned char comAdr, unsigned char controlByte, Buffer in, Buffer* out)
{
	{
		Buffer send;
		send.length = in.length + 5;
		send.data = malloc(send.length);
		send.data[0] = send.length;
		send.data[1] = comAdr;
		send.data[2] = controlByte;
		{ int i;
		for (i=0;i<in.length;i++) 
			send.data[3 + i] = in.data[i];
		}

		{
			unsigned int crc = crc16(send.data, send.length - 2);
			send.data[in.length + 3] = crc;
			send.data[in.length + 4] = crc >> 8;
		}

		usleep(10000);

		write(fd, send.data, send.length);
		free(send.data);
	}

	{
		Buffer receive;

		{
			char length;
			do {
				read(fd, &length,1);
			} while(length <= 0);
			receive.length = length - 1;
		}

		receive.data = malloc(receive.length);
		{
			size_t curr = 0;
			while (curr < receive.length) {
				curr +=read(fd, receive.data + curr, receive.length - curr);
			}
		}

		if (out) {
			out->data = receive.data;
			out->length = receive.length;
		} else {
			free(receive.data);
		}
	}
}


static void reset_feig() {
	Buffer in = {NULL, 0};
	execute_feig(0xff, 0x63, in, NULL);
	printf("reseted \n");
	sleep(3);
}

static void select_channel_feig(int channel) {
	unsigned char buf[11];
	Buffer in = {buf, sizeof(buf)};
	
	/* 9 Bytes constant for selecting channel in the multiplexer */
	in.data[0] = 0x01;
	in.data[1] = 0x00;
	in.data[2] = 0x20;
	in.data[3] = 0x02;
	in.data[4] = 0x1F;
	in.data[5] = 0x02;
	in.data[6] = 0xDD;
	in.data[7] = 0xFE;
	in.data[8] = 0x01;
	in.data[9] = channel & 0xFF;
	in.data[10] = 0x00;

	execute_feig(0xff,0xbf,in, NULL);
	usleep(100000);  
}


static void scan_field(Buffer* out) {
	unsigned char buf[2];
	Buffer in = {buf, sizeof(buf)};

	in.data[0] = 0x01;
	in.data[1] = 0x00;
	execute_feig(0xff, 0xb0, in, out);
}

static void copy(Tag* dst, unsigned char* src)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    int i;
    unsigned char* d = (unsigned char*) dst;
    for (i=0; i<8; i++) {
        d[i] = src[7-i];
    }
#elif __BYTE_ORDER == __BIG_ENDIAN
    memcpy(dst, src, 8);
#else
#error your endianess is not supported
#endif
}

static reader_Result* scan_array() 
{
	reader_Result* result = malloc(sizeof(reader_Result));
	result->size = cfg->numberofAntennas;
	result->antenna_results = malloc(sizeof(Tags) * cfg->numberofAntennas);
	
	int i;
	for(i=0; i<cfg->numberofAntennas; i++) {
		select_channel_feig(i+1);
		Buffer out;
		scan_field(&out);

		Tags* cur = result->antenna_results + i;
		cur->size = out.length>5 ? out.data[3] : 0;
		if (cur->size == 0) {
			cur->tags = NULL;
		} else {
			cur->tags = malloc(sizeof(Tag) * cur->size);
			int j;
			for(j=0; j<cur->size; j++) {
				copy(&cur->tags[j], out.data + (6+10*j));
			}
		}
	}

	return result;
}

static void start()
{
	struct termios options;
	
	fd = open(cfg->device, O_RDWR | O_NOCTTY);
	fcntl(fd, F_SETFL, 0);
	tcgetattr(fd, &options);
	cfsetispeed(&options, BAUDRATE);
	cfsetospeed(&options, BAUDRATE);

	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag |= PARENB;
	options.c_cflag &= ~PARODD;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_cflag &= ~CRTSCTS;
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_iflag &= ~(INPCK | ISTRIP);
	options.c_iflag &= ~(IXON | IXOFF | IXANY);
	options.c_oflag &= ~OPOST;
	tcsetattr(fd, TCSANOW, &options);

	/* initial reset of feig */
	reset_feig();
	/* give the system a second to sattle down ...*/
	sleep(1);
}


void reader_init(Reader_Config* config)
{
    cfg = config;
    start();
}

reader_Result* reader_read()
{
    return scan_array();
}

void reader_free(reader_Result* result)
{
    int i;
    for(i=0;i<result->size;i++) {
        free(result->antenna_results[i].tags);
    }
    free(result->antenna_results);
    free(result);
}

int reader_numberofAntennas()
{
    return cfg->numberofAntennas;
}
