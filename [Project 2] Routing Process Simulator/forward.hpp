#ifndef __FORWARD_H_
#define __FORWARD_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <tuple>
#include "util.hpp"

extern uint32_t timestamp;

class Forward_Info{
    public:
        Forward_Info();

        uint32_t get_IP_gateway();
        uint32_t get_metric();
        IP_prefix get_IP_AS();

        void set_IP_gateway(uint32_t IP_gateway);
        void set_metric(uint32_t metric);
        void set_IP_AS(IP_prefix IP_AS);
    private:
        uint32_t IP_gateway;
        IP_prefix IP_AS;
        uint32_t metric;
};

class Forward_Table{
    public:
        Forward_Table();
        void add_fw_info(Forward_Info forward_info);
        void change_fw_info_metric(int idx, uint32_t metric);
        int get_size();
        Forward_Info get_fw_info(int idx);

    private:
        std::vector<Forward_Info> fw_table;

};

enum Protocol{
    Distance_Vector = 0x80,
    Link_State = 0x81,
    Path_Vector = 0x82
};

enum Packet_type{
    Update = 0xff
};

class Packet{
    public:
        Packet();
        void set_protocol(uint8_t protocol);
        void set_type(uint8_t type);
        void set_source(uint32_t source);
        void set_dest(uint32_t dest);
        void set_timestamp(uint32_t timestamp);
        void set_body(uint16_t len, uint32_t* body);
        
        uint8_t get_protocol();
        uint8_t get_type();
        uint32_t get_source();
        uint32_t get_dest();
        uint16_t get_len();
        uint32_t get_timestamp();
        void get_body(uint32_t *output);

        void print_packet();

        void delete_body();

        ~Packet();

    private:
        uint8_t protocol;
        uint8_t type;
        uint16_t length;
        uint32_t ip_source;
        uint32_t ip_dest;
        uint32_t timestamp;
        uint32_t* body;
};


#endif // __FORWARD_H_
