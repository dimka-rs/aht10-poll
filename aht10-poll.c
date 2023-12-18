#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE (6)

extern int errno;

int main(int argc, const char *argv[])
{
    int file, ret;
    int addr = 0x38;
    int temp, hum;
    float temp_f, hum_f;
    const char* dev = argv[1];
    unsigned char buf[BUFSIZE];

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s /dev/i2c-0\n", argv[0]);
        return 1;
    }

    file = open(argv[1], O_RDWR);

    if (file < 0)
    {
        fprintf(stderr, "Failed to open %s: %s\n", dev, strerror(errno));
        return 1;
    }

    if (ioctl(file, I2C_SLAVE, addr) < 0)
    {
        fprintf(stderr, "Failed to ioctl %s: %s\n", dev, strerror(errno));
        return 1;
    }

    if(read(file, buf, BUFSIZE) != BUFSIZE)
    {
        fprintf(stderr, "Failed to read from %s: %s\n", dev, strerror(errno));
    }

    for (int i = 0; i < BUFSIZE; i++)
    {
        printf(" %02X", buf[i]);
    }
    printf("\n");

    hum = ((int)buf[1] << 16) + ((int)buf[2] << 8) + (int)buf[3];
    hum = hum >> 4;
    printf("Hum: %X\n", hum);

    hum_f = ((float)hum / (2 << 19)) * 100;

    temp = ((int)buf[3] << 16) + ((int)buf[4] << 8) + (int)buf[5];
    temp &= 0x0FFFFF;
    printf("Temp: %X\n", temp);
    temp_f = (float)temp / (2 << 19) * 200 - 50;

    printf("Humidity: %.2f %%\n", hum_f);
    printf("Temperature: %.2f C\n", temp_f);

    close(file);
    return 0;
}
