#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <tuple>
#include "util.hpp"
#include "forward.hpp"



/*
============= function in Forward_Info class =============
============= Do not fix this functions ===============
*/

Forward_Info::Forward_Info(){}

void Forward_Info::set_IP_gateway(uint32_t IP_gateway){
    this->IP_gateway = IP_gateway;
}

void Forward_Info::set_metric(uint32_t metric){
    this->metric = metric;
}

void Forward_Info::set_IP_AS(IP_prefix IP_AS){
    this->IP_AS = IP_AS;
}

uint32_t Forward_Info::get_IP_gateway(){
    return this->IP_gateway;
}

uint32_t Forward_Info::get_metric(){
    return this->metric;
}

IP_prefix Forward_Info::get_IP_AS(){
    return this->IP_AS;
} 
/*
============= function in Forwarding_Table class =============
*/

Forward_Table::Forward_Table(){}

void Forward_Table::add_fw_info(Forward_Info forward_info){
    fw_table.push_back(forward_info);
}

void Forward_Table::change_fw_info_metric(int idx, uint32_t metric){
    if(idx < 0 || idx >= this->get_size()){
        printf("Segmant fault can occur!!!!!");
        return;
    }
    fw_table[idx].set_metric(metric);
}

int Forward_Table::get_size(){
    return fw_table.size();
}

Forward_Info Forward_Table::get_fw_info(int idx){
    if(idx < 0 || idx >= this->get_size()){
        printf("get_fw_info : index error idx : [%d]\n", idx);
        return fw_table[0];
    }
    return fw_table[idx];
}

/*
============= function in Packet class =============
*/
Packet::Packet(){}

void Packet::set_protocol(uint8_t protocol){
    this->protocol = protocol;
}

void Packet::set_type(uint8_t type){
    this->type = type;
}

void Packet::set_source(uint32_t source){
    this->ip_source = source;
}

void Packet::set_dest(uint32_t dest){
    this->ip_dest = dest;
}

void Packet::set_timestamp(uint32_t timestamp){
    this->timestamp = timestamp;
}

void Packet::set_body(uint16_t len, uint32_t* body){
    this->length = len;
    if(len % 4 != 0) {
        printf("length of Packet should be multiple of 4!!!\n");
        return;
    }
    if(len <= 16){
        printf("You should make header!!!\n");
        return;
    }

    this->body = new uint32_t[len/4 - 4];
    for(int i = 0; i < len/4 - 4; i++){
        this->body[i] = body[i];
    }
    
}

uint8_t Packet::get_protocol(){
    return this->protocol;
}

uint8_t Packet::get_type(){
    return this->type;
}

uint32_t Packet::get_source(){
    return this->ip_source;
} 

uint32_t Packet::get_dest(){
    return this->ip_dest;
}

uint32_t Packet::get_timestamp(){
    return this->timestamp;
}

uint16_t Packet::get_len(){
    return this->length;
}

void Packet::get_body(uint32_t *output){
    for(int i = 0; i < this->length / 4 - 4; i++){
        output[i] = this->body[i];
    }

}

void Packet::delete_body(){
    delete[] body;
}

void Packet::print_packet(){
    printf("protocol : ");
    if(this->protocol == Distance_Vector){
        printf("Distant-Vector\n");
    }
    else if(this->protocol == Link_State){
        printf("Link-State\n");
    }
    else if(this->protocol == Path_Vector){
        printf("Path-Vector\n");
    }
    printf("type : ");
    if(this->type == Update){
        printf("Update\n");
    }

    printf("Source : ");
    printIP(this->ip_source);
    
    printf("Dest : ");
    printIP(this->ip_dest);

    printf("length : %u\n", this->length);

    printf("timestamp : %u \n", this->timestamp);
    
    printf("body : \n");
    for(int i = 0; i < (length-16)/4 ; i++){
        printf("%u\n",body[i]);
    }

}

Packet::~Packet(){
}

