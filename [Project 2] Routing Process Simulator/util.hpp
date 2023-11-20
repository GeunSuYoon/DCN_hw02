#ifndef __PROJ2_UTIL_H_
#define __PROJ2_UTIL_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <tuple>

#define RED_PRTF(...) {printf("\033[0;31m"); printf(__VA_ARGS__); printf("\033[0m");}
#define GREEN_PRTF(...) {printf("\033[0;32m"); printf(__VA_ARGS__); printf("\033[0m");}
#define YELLOW_PRTF(...) {printf("\033[0;33m"); printf(__VA_ARGS__); printf("\033[0m");}
#define ERROR_PRTF(...) {fprintf(stderr, "\033[0;31m"); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\033[0m");}

#define STR_LEN 64
#define INF 0xffffffff


/*
=========== Do not fix util.hpp =========
 */

class IP_prefix{
    public:
        IP_prefix();
        uint32_t get_IPv4();
        uint32_t get_netmask();
        void set_IPv4(uint32_t IPv4);
        void set_netmask(uint32_t netmask);

    private:
        uint32_t IPv4;
        uint32_t netmask;
};

void printIP(uint32_t IPv4);
void printIP_no_new_line(uint32_t IPv4);

ssize_t read_file (void** output, char *file_path);

uint32_t make_netmask(int power);

bool in_ip_prefix_range(uint32_t IPv4, IP_prefix ip_range);

extern uint32_t timestamp;

typedef std::tuple<uint32_t, int, uint32_t> tuiu;

#endif // __PROJ2_UTIL_H_