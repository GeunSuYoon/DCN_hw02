#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <tuple>
#include "util.hpp"
#include "forward.hpp"
#include "routing.hpp"
#include "auto_sys.hpp"
#include "network.hpp"

/*
=============== function in network class =============
*/
Network::Network() {}

void Network::make_network(char *filename)
{
    // open file
    char file_path[MAX_PATH_SIZE] = {0};
    snprintf(file_path, MAX_PATH_SIZE, "%s%s", FILEPATH, filename);

    void *file_buffer = NULL;
    char *file_bf_char = NULL;
    ssize_t file_size = -1;
    file_size = read_file(&file_buffer, file_path);
    file_bf_char = (char *)file_buffer;

    // parsing csv
    char *ptr = strtok(file_bf_char, ",");
    // make AS
    int num_as = atoi(ptr);

    malloc_AS(num_as);

    ptr = strtok(NULL, "\n");

    for (int i = 0; i < num_as; i++)
    {
        uint32_t ipv4 = 0;
        uint32_t tmp = 0;
        for (int j = 0; j < 3; j++)
        {
            ptr = strtok(NULL, ".");
            tmp = (uint32_t)atoi(ptr);
            ipv4 |= tmp;
            ipv4 <<= 8;
        }

        ptr = strtok(NULL, ",");
        tmp = (uint32_t)atoi(ptr);
        ipv4 |= tmp;

        ptr = strtok(NULL, ",");
        tmp = (uint32_t)atoi(ptr);

        uint32_t netmask = make_netmask(tmp);
        this->make_AS(ipv4, netmask, i, num_as);

        ptr = strtok(NULL, "\n");
    }

    this->num_AS_net = (uint32_t)num_as;

    // make link between AS
    ptr = strtok(NULL, ",");
    int num_link = atoi(ptr);
    ptr = strtok(NULL, "\n");
    for (int i = 0; i < num_link; i++)
    {
        int as_a, as_b;
        uint32_t metric;

        ptr = strtok(NULL, ",");
        as_a = atoi(ptr);

        ptr = strtok(NULL, ",");
        as_b = atoi(ptr);

        ptr = strtok(NULL, ",");
        metric = (uint32_t)atoi(ptr);

        ptr = strtok(NULL, "\n");

        this->make_link(as_a, as_b, metric);
    }
}

void Network::make_AS(uint32_t ipv4, uint32_t netmask, int idx, int num)
{
    IP_prefix ip;
    ip.set_IPv4(ipv4);
    ip.set_netmask(netmask);
    AS[idx]->set_AS_IP(ip);

    uint32_t ASN = (uint32_t)(idx);
    AS[idx]->set_ASN(ASN);

    uint32_t num_as = (uint32_t)num;
    AS[idx]->set_num_AS(num_as);
}

void Network::make_link(int as_a, int as_b, uint32_t metric)
{
    Forward_Info fw_info_a, fw_info_b;
    IP_prefix ip_tmp;
    ip_tmp = AS[as_b]->get_AS_IP();
    fw_info_a.set_IP_gateway(ip_tmp.get_IPv4());
    fw_info_a.set_IP_AS(ip_tmp);
    fw_info_a.set_metric(metric);
    int sz_a = AS[as_a]->get_fw_table_len();
    AS[as_a]->add_fw_table(fw_info_a);
    AS[as_a]->add_neighbor(AS[as_b]);

    ip_tmp = AS[as_a]->get_AS_IP();
    fw_info_b.set_IP_gateway(ip_tmp.get_IPv4());
    fw_info_b.set_IP_AS(ip_tmp);
    fw_info_b.set_metric(metric);
    int sz_b = AS[as_b]->get_fw_table_len();
    AS[as_b]->add_fw_table(fw_info_b);
    AS[as_b]->add_neighbor(AS[as_a]);
}

void Network::simulation(char* filename){
    this->make_network(filename);
    this->init_simulation();
    while (1){
        char cmd[STR_LEN] = {0};
        YELLOW_PRTF("ENTER COMMAND");
        printf(" (\"help\" for help): ");
        fflush(stdout);
        if (fgets (cmd, STR_LEN, stdin) == NULL)
            break;

        cmd[strcspn(cmd, "\n")] = '\0';

        if (strncmp (cmd, "help", 5) == 0){
            GREEN_PRTF ("COMMANDS:\n");
            printf("change i j m : ");
            printf("i, j is ASN which is endpoint of link, ");
            printf("m is metric that we want to change.\n");
            printf("exit: ");
            printf("End this program.\n");
            printf("print i: ");
            printf("It will print routing table of AS which ASN is \"i\"\n");
            printf("help: ");
            printf("It will print these lines again.\n");
            printf("timestemp: ");
            printf("It will print current timestamp.\n");
        }
        else if(strncmp (cmd, "change ", 7) == 0){
            char* ptr_tmp = strtok(cmd, " ");
            int AS_first, AS_second;
            uint32_t metric_link;
            ptr_tmp = strtok(NULL, " ");
            AS_first = atoi(ptr_tmp);
            ptr_tmp = strtok(NULL, " ");
            AS_second = atoi(ptr_tmp);
            ptr_tmp = strtok(NULL, "\n");
            metric_link = (uint32_t)atoi(ptr_tmp);
            if(AS_first >= num_AS_net || AS_second >= num_AS_net){
                RED_PRTF("Please write valid index 0 ~ %u\n",num_AS_net - 1);
                continue;
            }
            change_simulation(AS_first, AS_second, metric_link);
        }
        else if(strncmp (cmd, "exit", 5) == 0){
            YELLOW_PRTF("Exiting...\n\n");
            break;
        }
        else if(strncmp (cmd, "print ", 6) == 0){
            char* ptr_tmp = strtok(cmd, " ");
            int AS_print;
            ptr_tmp = strtok(NULL, "\n");
            AS_print = (uint32_t)atoi(ptr_tmp);
            if(AS_print < num_AS_net)
                AS[AS_print]->print_rt_table();
            else
                RED_PRTF("Please write valid index 0 ~ %u\n",num_AS_net - 1);
        }
        else if(strncmp (cmd, "timestamp", 10) == 0){
            printf("Timestamp is %u\n", timestamp);
        }
    }
}

void Network::init_simulation(){
    bool check_update_continue, tmp;
    for(int i = 0; i < this->num_AS_net; i++){
        AS[i]->init_rt_table();
        AS[i]->send_packet_neighbor();
        // AS[i]->print_rt_table();
    }
    while(1){
        timestamp++;
        check_update_continue = false;
        for(int i = 0; i < this->num_AS_net; i++){
			// printf("test : update AS %d\n", i);
            tmp = AS[i]->update_rt_table();
            check_update_continue |= tmp;
            if(tmp)
                AS[i]->send_packet_neighbor();
        }
        if(!check_update_continue) break;
    }
    for(int i = 0; i < this->num_AS_net; i++){
        AS[i]->print_rt_table();
    }
}

void Network::change_simulation(int as_a, int as_b, uint32_t metric_link){
    IP_prefix ip_a, ip_b;
    ip_a = AS[as_a]->get_AS_IP();
    ip_b = AS[as_b]->get_AS_IP();
    int fw_idx_a, fw_idx_b;
    fw_idx_a = AS[as_a]->find_fw_info_idx(ip_b.get_IPv4());
    fw_idx_b = AS[as_b]->find_fw_info_idx(ip_a.get_IPv4());
    if(fw_idx_a == -1){
        RED_PRTF("There is no link between %d and %d\n",as_a, as_b);
        return;
    }
    AS[as_a]->change_fw_info_metric(fw_idx_a, metric_link);
    AS[as_b]->change_fw_info_metric(fw_idx_b, metric_link);

    AS[as_a]->change_rt_info(ip_b, metric_link);
    AS[as_b]->change_rt_info(ip_a, metric_link);

    AS[as_a]->send_packet_neighbor();
    AS[as_b]->send_packet_neighbor();

    while(1){
        timestamp++;
		printf("change %d[%d] %d[%d] %u start\n", as_a, fw_idx_a, as_b, fw_idx_b, metric_link);
        bool check_update_continue = false;
        for(int i = 0; i < this->num_AS_net; i++){
            bool tmp = AS[i]->update_rt_table();
            check_update_continue |= tmp;
            if(tmp){
                AS[i]->send_packet_neighbor();
            }
                
        }
        if(!check_update_continue) break;
    }

}

Network::~Network() {}
/*
=============== function in Network_DV class =============
*/
Network_DV::Network_DV() {}

void Network_DV::malloc_AS(int num)
{
    for (int i = 0; i < num; i++)
    {
        Auto_sys *as = new AS_Dist_Vect;
        this->AS.push_back(as);
    }
}

/*
=============== function in Network_LS class =============
*/
Network_LS::Network_LS() {}

void Network_LS::malloc_AS(int num)
{
    for (int i = 0; i < num; i++)
    {
        Auto_sys *as = new AS_Link_Stat;
        this->AS.push_back(as);
    }
}

/*
=============== function in Network_PV class =============
*/
Network_PV::Network_PV() {}

void Network_PV::malloc_AS(int num)
{
    for (int i = 0; i < num; i++)
    {
        Auto_sys *as = new AS_Path_Vect;
        this->AS.push_back(as);
    }
}