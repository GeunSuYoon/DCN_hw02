#ifndef __NETWORK_H_
#define __NETWORK_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <tuple>
#include "util.hpp"
#include "forward.hpp"
#include "routing.hpp"
#include "auto_sys.hpp"

#define FILEPATH "./graph_csv/"
#define MAX_PATH_SIZE 256

extern uint32_t timestamp;

class Network{
    public:
        Network();
        void make_network(char* filename);
        virtual void malloc_AS(int num) = 0;
        void make_AS(uint32_t ipv4, uint32_t netmask, int idx, int num);
        void make_link(int as_a, int as_b, uint32_t metric);
        void simulation(char* filename);
        void init_simulation();
        void change_simulation(int as_a, int as_b, uint32_t metric_link);
        ~Network();
        
    private:
        uint32_t num_AS_net; 

    protected:
        std::vector<Auto_sys* > AS;
};

class Network_DV : public Network{
    public:
        Network_DV();
        void malloc_AS(int num);
    private:
        
};

class Network_LS : public Network{
    public:
        Network_LS();
        void malloc_AS(int num);
    private:
        
};

class Network_PV : public Network{
    public:
        Network_PV();
        void malloc_AS(int num);
    private:
        
};

#endif //__NETWORK_H_