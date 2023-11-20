#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <tuple>
#include "util.hpp"

/*
=========== Do not fix util.cpp =========
 */

/*
============= function in IP_prefix class =============
*/

IP_prefix::IP_prefix(){}

void IP_prefix::set_IPv4(uint32_t IPv4){
    this->IPv4 = IPv4;
}

void IP_prefix::set_netmask(uint32_t netmask){
    this->netmask = netmask;
}

uint32_t IP_prefix::get_IPv4(){
    return this->IPv4;
}

uint32_t IP_prefix::get_netmask(){
    return this->netmask;
}

/*
============== other functions ================
*/

void printIP(uint32_t IPv4){
    printf("%u.%u.%u.%u\n", (IPv4 & 0xFF000000) >> 24, (IPv4 & 0xFF0000)>> 16,
                            (IPv4 & 0xFF00) >> 8, (IPv4 & 0xFF));
    return;
}

void printIP_no_new_line(uint32_t IPv4){
    printf("%u.%u.%u.%u", (IPv4 & 0xFF000000) >> 24, (IPv4 & 0xFF0000)>> 16,
                            (IPv4 & 0xFF00) >> 8, (IPv4 & 0xFF));
    return;
}

ssize_t read_file (void** output, char *file_path)
{
    if (output == NULL || file_path == NULL)
    {
        ERROR_PRTF ("ERROR read_file(): NULL parameter\n");
        return -1;
    }
    FILE *fp = fopen(file_path, "rb");
    if (fp == NULL)
    {
        ERROR_PRTF ("ERROR read_file(): file does not exist.\n");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (file_size == 0)
    {
        if (*output != NULL)
            free (*output);
        *output = NULL;
        fclose(fp);
        return 0;
    }

    if (*output != NULL)
        free (*output);
    *output = (void *) malloc(file_size);
    if (*output == NULL)
    {
        ERROR_PRTF ("ERROR read_file(): malloc()\n");
        fclose(fp);
        return -1;
    }

    if (fread(*output, 1, file_size, fp) != file_size)
    {
        ERROR_PRTF ("ERROR read_file(): fread()\n");
        fclose(fp);
        free (*output);
        return -1;
    }

    fclose(fp);
    return file_size;
}

uint32_t make_netmask(int power){
    uint32_t ret = 0;
    for(int i = 0; i < 32; i++){
        ret <<= 1;
        if(i < power) ret |= 1;
    }
    return ret;
}

bool in_ip_prefix_range(uint32_t ipv4, IP_prefix ip_range){
    uint32_t tmp = (ip_range.get_netmask()) & ipv4;
    return ((tmp == ip_range.get_IPv4())? true: false); 
}
