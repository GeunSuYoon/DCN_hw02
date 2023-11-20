#ifndef __AUTO_SYS_H_
#define __AUTO_SYS_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <tuple>
#include <queue>
#include "util.hpp"
#include "forward.hpp"
#include "routing.hpp"

extern uint32_t timestamp;

//class for Autonomous system
class Auto_sys {
    public:
        Auto_sys(); // constructor 

        void set_AS_IP(IP_prefix IP); 
        void set_ASN(uint32_t ASN);
        IP_prefix get_AS_IP();
        uint32_t get_ASN();

        virtual void set_num_AS(uint32_t num_AS) = 0;
        virtual uint32_t get_num_AS() = 0;

        void add_fw_table(Forward_Info fw_info);
        void change_fw_info_metric(int idx, uint32_t metric);
        int get_fw_table_len();
        int find_fw_info_idx(uint32_t gateway);
        Forward_Info get_fw_info(int idx);

        void add_neighbor(Auto_sys* neighbor);
        int get_num_neighbor();
        Auto_sys* get_ptr_neighbor(int idx);
        void receive_packet_neighbor(Packet rcv_packet);

        virtual int get_rt_table_len() = 0;
        virtual Routing_Info* get_rt_info(int idx) = 0;

        virtual void init_rt_table() = 0;
        virtual bool update_rt_table() = 0;
        virtual void send_packet_neighbor() = 0;

        virtual void change_rt_info(IP_prefix ip, uint32_t metric) = 0;

        virtual void print_rt_table() = 0;
        ~Auto_sys();
        
    private:
        uint32_t ASN; 
        IP_prefix IP; 
        Forward_Table fw_table; 
        std::vector<Auto_sys *> neighbor_AS;
        int num_nb;
    protected:
        std::queue<Packet> neighbor_pkt;
};

// class for autonomous system that use Distance-Vector Routing Protocol
class AS_Dist_Vect : public Auto_sys{
    public:
        AS_Dist_Vect();
        ~AS_Dist_Vect();
        
        int get_rt_table_len();
        Routing_Info* get_rt_info(int idx);

        void init_rt_table();
        bool update_rt_table();
        void send_packet_neighbor();

        void set_num_AS(uint32_t num_AS);
        uint32_t get_num_AS();

        void change_rt_info(IP_prefix ip, uint32_t metric);

        void print_rt_table();

    private:
        uint32_t num_AS; // total number of Autonomous system
        Routing_Table_DV rt_table;
};

// class for autonomous system that use Link-State Routing Protocol 
class AS_Link_Stat : public Auto_sys{
    public:
        AS_Link_Stat();
        ~AS_Link_Stat();

        int get_rt_table_len();
        Routing_Info* get_rt_info(int idx);

        void init_rt_table();
        bool update_rt_table();
        bool update_map();
        void update_table_dijkstra();
        void send_packet_neighbor();

        void set_num_AS(uint32_t num_AS);
        uint32_t get_num_AS();

        void change_rt_info(IP_prefix ip, uint32_t metric);

        void print_rt_table();

    private:
        uint32_t num_AS; // total number of Autonomous system
        Routing_Table_LS rt_table;
};

// class for autonomous system that us Path-Vector Routing Protocol
class AS_Path_Vect : public Auto_sys{
    public:
        AS_Path_Vect();
        ~AS_Path_Vect();

        int get_rt_table_len();
        Routing_Info* get_rt_info(int idx);

        void init_rt_table();
        bool update_rt_table();
        void send_packet_neighbor();

        void set_num_AS(uint32_t num_AS);
        uint32_t get_num_AS();

        void change_rt_info(IP_prefix ip, uint32_t metric);

        void print_rt_table();

    private:
        Routing_Table_PV rt_table;

};

#endif // __AUTO_SYS_H_
