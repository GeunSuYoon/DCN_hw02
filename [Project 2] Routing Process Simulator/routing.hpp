#ifndef __ROUTING_H_
#define __ROUTING_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <tuple>
#include "util.hpp"
#include "forward.hpp"

extern uint32_t timestamp;

typedef enum additional{
    metric_via_neighbor,
    metric_to_AS,
    ASN,
    path_to_AS,
} add_t;

class Routing_Info{
    public:
        Routing_Info();

        IP_prefix get_IP_prefix();
        uint32_t get_gateway();
        uint32_t get_total_metric();
        
        virtual uint32_t get_additional_data_idx(add_t ad, int idx) = 0;
        virtual int get_additional_data_size(add_t ad) = 0;

        void set_IP_prefix(uint32_t IPv4, uint32_t netmask);
        void set_IP_prefix(IP_prefix ip_prefix);
        void set_gateway(uint32_t gateway);
        void set_total_metric(uint32_t total_metric);
        
        virtual void set_additional_data(add_t ad, int num_data, uint32_t* additional_data) = 0;
        virtual void set_additional_data_idx(add_t ad, int idx, uint32_t addtional_data) = 0;
        virtual void delete_additional_data(add_t ad) = 0;

    private:
        IP_prefix ip_prefix;
        uint32_t gateway;
        uint32_t total_metric;

};

class Routing_Info_DV : public Routing_Info{
    public:
        Routing_Info_DV();

        void set_additional_data(add_t ad, int num_data, uint32_t* additional_data);
        void set_additional_data_idx(add_t ad, int idx, uint32_t addtional_data);
        void set_metric_via_neighbor(int num_nb, uint32_t* metric_via_neighbor);
        void set_metric_via_neighbor_idx(int idx, uint32_t metric_via_neighbor_idx);
        
        int get_additional_data_size(add_t ad);
        uint32_t get_additional_data_idx(add_t ad, int idx);
        uint32_t get_metric_via_neighbor_idx(int idx);
        
        void delete_additional_data(add_t ad);
        void delete_metric_via_neighbor();

        ~Routing_Info_DV();

    private:
        uint32_t* metric_via_neighbor;
        int num_nb;
};

class Routing_Info_LS : public Routing_Info{
    public:
        Routing_Info_LS();

        void set_additional_data(add_t ad, int num_data, uint32_t* additional_data);
        void set_additional_data_idx(add_t ad, int idx, uint32_t addtional_data);
        void set_metric_to_AS(int num_AS, uint32_t* metric_to_AS);
        void set_metric_to_AS_idx(int idx, uint32_t metric_to_AS_idx);
        void set_ASN(uint32_t ASN);

        int get_additional_data_size(add_t ad);
        uint32_t get_additional_data_idx(add_t ad, int idx);
        uint32_t get_metric_to_AS_idx(int idx);
        uint32_t get_ASN();
        
        void delete_additional_data(add_t ad);
        void delete_metric_to_AS();

        ~Routing_Info_LS();
    private:
        uint32_t* metric_to_AS;
        uint32_t ASN;
        int num_AS;
};

class Routing_Info_PV : public Routing_Info{
    public:
        Routing_Info_PV();

        void set_additional_data(add_t ad, int num_data, uint32_t* additional_data);
        void set_additional_data_idx(add_t ad, int idx, uint32_t addtional_data);
        void set_path_to_AS(int num_data, uint32_t* additional_data);
        void set_path_to_AS_idx(int num_data, uint32_t additional_data);

        int get_additional_data_size(add_t ad);
        uint32_t get_additional_data_idx(add_t ad, int idx);
        uint32_t get_path_to_AS_idx(int idx);

        void delete_additional_data(add_t ad);
        void delete_path_to_AS();

        ~Routing_Info_PV();

    private:
        uint32_t* path_to_AS;
        uint32_t path_length;
};


class Routing_Table{
    public:
        Routing_Table();
        virtual Routing_Info* init_and_add_rt_info() = 0;
        int get_size();
        Routing_Info* get_rt_info(int idx);
        int find_idx(uint32_t ipv4);
        ~Routing_Table();

    protected:
        std::vector<Routing_Info *> rt_table;
};

class Routing_Table_DV : public Routing_Table{
    public:
        Routing_Info* init_and_add_rt_info();
        ~Routing_Table_DV();

    private:
};

class Routing_Table_LS : public Routing_Table{
    public:
        Routing_Info* init_and_add_rt_info();
        ~Routing_Table_LS();

    private:

};

class Routing_Table_PV : public Routing_Table{
    public:
        Routing_Info* init_and_add_rt_info();
        ~Routing_Table_PV();

    private:

};

#endif // __FORWARD_H_