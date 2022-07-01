#include <stdio.h>
#include <modbus.h>
#include <errno.h>

int main(void) {
    modbus_t *mb;
    uint16_t tab_reg[32];
    int i;
    int rc;

    //打开端口
    mb = modbus_new_tcp("192.168.181.1", 502);
    modbus_set_slave(mb, 1);

    //建立链接
    if(modbus_connect(mb)==-1) {
        fprintf(stderr, "Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(mb);
        return -1;
    }

    printf("READ REGISTERS\n\n");

    /* Read 5 registers from the address 0 */
    rc = modbus_read_registers(mb, 0, 5, tab_reg);
    if (rc == -1)
    {
        fprintf(stderr,"%s\n", modbus_strerror(errno));
        return -1;
    }
    printf("[read registers num = %d]\n",rc);
    for(i=0;i<5;i++)
    {
        printf("reg[%d] = %d(0x%x)\n", i, tab_reg[i], tab_reg[i]);
    }
    printf("\n");

    modbus_close(mb);
    modbus_free(mb);
}