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
    unsigned char reset[] = {0xE1, 0x08, 0x00};
    unsigned char measure[] = {0xAC, 0x33, 0x00};
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
        fprintf(stderr, "Failed to ioctl: %s\n", strerror(errno));
        goto err;
    }

    if (write(file, reset, sizeof(reset)) != sizeof(reset))
    {
        fprintf(stderr, "Failed to write reset: %s\n", strerror(errno));
        goto err;
    }

    if (write(file, measure, sizeof(measure)) != sizeof(measure))
    {
        fprintf(stderr, "Failed to write measure: %s\n", strerror(errno));
        goto err;
    }

    /* Sleep 100 ms while sensor is measuring */
    usleep(100000);

    if (read(file, buf, BUFSIZE) != BUFSIZE)
    {
        fprintf(stderr, "Failed to read: %s\n", strerror(errno));
        goto err;
    }


    hum = ((int)buf[1] << 16) + ((int)buf[2] << 8) + (int)buf[3];
    hum = hum >> 4;
    hum_f = ((float)hum / (2 << 19)) * 100;

    temp = ((int)buf[3] << 16) + ((int)buf[4] << 8) + (int)buf[5];
    temp &= 0x0FFFFF;
    temp_f = (float)temp / (2 << 19) * 200 - 50;

    printf("Humidity:    %.2f %%\n", hum_f);
    printf("Temperature: %.2f C\n", temp_f);

    return 0;

err:
    close(file);
    return 1;
}
