#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <tuple>
#include "util.hpp"
#include "forward.hpp"
#include "routing.hpp"

/*
============= function in Routing_Info class =============
*/

Routing_Info::Routing_Info(){}

IP_prefix Routing_Info::get_IP_prefix(){
    return this->ip_prefix;
}

uint32_t Routing_Info::get_gateway(){
    return this->gateway;
}

uint32_t Routing_Info::get_total_metric(){
    return this->total_metric;
}

void Routing_Info::set_IP_prefix(uint32_t IPv4, uint32_t netmask){
    this->ip_prefix.set_IPv4(IPv4);
    this->ip_prefix.set_netmask(netmask);
}

void Routing_Info::set_IP_prefix(IP_prefix IP_prefix){
    this->ip_prefix = IP_prefix;
}

void Routing_Info::set_gateway(uint32_t gateway){
    this->gateway = gateway;
}

void Routing_Info::set_total_metric(uint32_t total_metric){
    this->total_metric = total_metric;
}

/*
============= function in Routing_Info_DV class =============
*/

Routing_Info_DV::Routing_Info_DV(){}

void Routing_Info_DV::set_additional_data(add_t ad, int num_data, uint32_t* additional_data){
    if(ad == additional::metric_via_neighbor){
        this->set_metric_via_neighbor(num_data, additional_data);
    }
};

void Routing_Info_DV::set_additional_data_idx(add_t ad, int idx, uint32_t additional_data){
	if(ad == additional::metric_via_neighbor){
        this->set_metric_via_neighbor_idx(idx, additional_data);
    }
};

void Routing_Info_DV::set_metric_via_neighbor(int num_nb, uint32_t* metric_via_neighbor){
    this->metric_via_neighbor = new uint32_t[num_nb];
    this->num_nb = num_nb;

    for(int i = 0; i < num_nb; i++){
        this->metric_via_neighbor[i] = metric_via_neighbor[i];
    }
}

void Routing_Info_DV::set_metric_via_neighbor_idx(int idx, uint32_t metric_via_neighbor_idx){
    if(idx < 0 || idx >= num_nb){
        printf("error : it could get segmentation fault\n");
        return;
    }
    this->metric_via_neighbor[idx] = metric_via_neighbor_idx;
}

int Routing_Info_DV::get_additional_data_size(add_t ad){
    if(ad == additional::metric_via_neighbor){
        return this->num_nb;
    }
    return 0;
}

uint32_t Routing_Info_DV::get_additional_data_idx(add_t ad, int idx){
    if(ad == additional::metric_via_neighbor){
        return this->get_metric_via_neighbor_idx(idx);
    }
    return 0;
}

uint32_t Routing_Info_DV::get_metric_via_neighbor_idx(int idx){
    if(idx < 0 || idx >= num_nb){
        printf("error : it could get segmentation fault");
        return 0;
    }
    return this->metric_via_neighbor[idx];
}

void Routing_Info_DV::delete_additional_data(add_t ad){
    if(ad == additional::metric_via_neighbor){
        delete_metric_via_neighbor();
    }
}

void Routing_Info_DV::delete_metric_via_neighbor(){
    delete[] metric_via_neighbor;
}

Routing_Info_DV::~Routing_Info_DV(){
    delete[] metric_via_neighbor;
}

/*
============= function in Routing_Info_LS class =============
*/

Routing_Info_LS::Routing_Info_LS(){}

void Routing_Info_LS::set_additional_data(add_t ad, int num_data, uint32_t* additional_data){
    if(ad == additional::metric_to_AS){
        this->set_metric_to_AS(num_data, additional_data);
    }
    if(ad == additional::ASN){
        this->set_ASN(additional_data[0]);
    }
};

void Routing_Info_LS::set_additional_data_idx(add_t ad, int idx, uint32_t additional_data){
    if(ad == additional::metric_to_AS){
        this->set_metric_to_AS_idx(idx, additional_data);
    }
};

void Routing_Info_LS::set_metric_to_AS(int num_AS, uint32_t* metric_to_AS){
    this->num_AS = num_AS;
    this->metric_to_AS = new uint32_t[num_AS];
    
    for(int i = 0; i < num_AS; i++){
        this->metric_to_AS[i] = metric_to_AS[i];
    }
}

void Routing_Info_LS::set_metric_to_AS_idx(int idx, uint32_t metric_to_AS_idx){
    if(idx < 0 || idx >= num_AS){
        printf("error : it could get segmentation fault");
        return;
    }

    this->metric_to_AS[idx] = metric_to_AS_idx;
}

void Routing_Info_LS::set_ASN(uint32_t ASN){
    this->ASN = ASN;
}

int Routing_Info_LS::get_additional_data_size(add_t ad){
    if(ad == additional::metric_to_AS){
        return this->num_AS;
    }
    return 0;
}

uint32_t Routing_Info_LS::get_additional_data_idx(add_t ad, int idx){
    if(ad == additional::metric_to_AS){
        return this->get_metric_to_AS_idx(idx);
    }
    if(ad == additional::ASN){
        return this->get_ASN();
    }
    return 0;
}

uint32_t Routing_Info_LS::get_metric_to_AS_idx(int idx){
    if(idx < 0 || idx >= num_AS){
        printf("error : it could get segmentation fault");
        return 0;
    }
    return this->metric_to_AS[idx];
}
uint32_t Routing_Info_LS::get_ASN(){
    return this->ASN;
}

void Routing_Info_LS::delete_additional_data(add_t ad){
    if(ad ==  additional::metric_to_AS){
        delete_metric_to_AS();
    }
}

void Routing_Info_LS::delete_metric_to_AS(){
    delete[] metric_to_AS;
}

Routing_Info_LS::~Routing_Info_LS(){
    delete[] metric_to_AS;
}

/*
============= function in Routing_Info_PV class =============
*/

Routing_Info_PV::Routing_Info_PV(){}

void Routing_Info_PV::set_additional_data(add_t ad, int num_data, uint32_t* additional_data){
    if(ad == additional::path_to_AS){
        this->set_path_to_AS(num_data, additional_data);
    }
};

void Routing_Info_PV::set_additional_data_idx(add_t ad, int idx, uint32_t additional_data){
    if(ad == additional::path_to_AS){
        this->set_path_to_AS_idx(idx, additional_data);
    }
}

void Routing_Info_PV::set_path_to_AS(int num_data, uint32_t* additional_data){
    this->path_to_AS = new uint32_t[num_data];
    this->path_length = num_data;

    for(int i = 0; i < this->path_length; i++){
        this->path_to_AS[i] = additional_data[i];
    }
}

void Routing_Info_PV::set_path_to_AS_idx(int idx, uint32_t path_to_AS_idx){
    if(idx < 0 || idx >= path_length){
        printf("error : it could get segmentation fault");
        return;
    }

    this->path_to_AS[idx] = path_to_AS_idx;
}

int Routing_Info_PV::get_additional_data_size(add_t ad){
     if(ad == additional::path_to_AS){
        return this->path_length;
    }
    return 0;
}

uint32_t Routing_Info_PV::get_additional_data_idx(add_t ad, int idx){
    if(ad == additional::path_to_AS){
        return this->get_path_to_AS_idx(idx);
    }
    return 0;
}

uint32_t Routing_Info_PV::get_path_to_AS_idx(int idx){
    if(idx < 0 || idx >= path_length){
        printf("error : it could get segmentation fault");
        return 0;
    }
    return this->path_to_AS[idx];
}

void Routing_Info_PV::delete_additional_data(add_t ad){
    if(ad == additional::path_to_AS){
        delete_path_to_AS();
    }
}

void Routing_Info_PV::delete_path_to_AS(){
    delete[] path_to_AS;
}


Routing_Info_PV::~Routing_Info_PV(){
    delete[] path_to_AS;
}


/*
============= function in Routing_Table class =============
*/

Routing_Table::Routing_Table(){}

int Routing_Table::get_size(){
    return rt_table.size();
}

Routing_Info* Routing_Table::get_rt_info(int idx){
    return rt_table[idx];
}

int Routing_Table::find_idx(uint32_t ipv4){
    int sz = this->get_size();
    for(int i = 0; i < sz; i++){
        if(in_ip_prefix_range(ipv4, rt_table[i]->get_IP_prefix())){
            return i;
        }
    }
    return -1;
}

Routing_Table::~Routing_Table(){
}

/*
============= function in Routing_Table_DV class =============
*/

Routing_Info* Routing_Table_DV::init_and_add_rt_info(){
    rt_table.push_back(nullptr);
    int idx = this->get_size() - 1;
    rt_table[idx] = new Routing_Info_DV;
    return rt_table[idx];
}
 
Routing_Table_DV::~Routing_Table_DV(){}

/*
============= function in Routing_Table_LS class =============
*/

Routing_Info* Routing_Table_LS::init_and_add_rt_info(){
    rt_table.push_back(nullptr);
    int idx = this->get_size() - 1;
    rt_table[idx] = new Routing_Info_LS;
    return rt_table[idx];
}

Routing_Table_LS::~Routing_Table_LS(){}

/*
============= function in Routing_Table_PV class =============
*/

Routing_Info* Routing_Table_PV::init_and_add_rt_info(){
    rt_table.push_back(nullptr);
    int idx = this->get_size() - 1;
    rt_table[idx] = new Routing_Info_PV;
    return rt_table[idx];
}

Routing_Table_PV::~Routing_Table_PV(){}

