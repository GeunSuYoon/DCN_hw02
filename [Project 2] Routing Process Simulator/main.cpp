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

uint32_t timestamp = 0;

int main(int argc, char **argv){
    if(argc != 3){
        printf("Usage: %s <file name> D/L/P\n", argv[0]);
        printf("D for Distance-Vector, L for Link-State, P for Path-Vector\n");
        return -1;
    }
    if(strncmp(argv[2], "D", 2) != 0 && strncmp(argv[2], "L", 2) != 0 && strncmp(argv[2], "P", 2) != 0){
        printf("Usage: %s <file name> D/L/P\n", argv[0]);
        printf("D for Distance-Vector, L for Link-State, P for Path-Vector\n");
        return -1;
    }
    Network* nw;
    if(strncmp(argv[2], "D", 2) == 0){
        nw = new Network_DV;
    }
    else if(strncmp(argv[2], "L", 2) == 0){
        nw = new Network_LS;
    }
    else if(strncmp(argv[2], "P", 2) == 0){
        nw = new Network_PV;
    }
    
    nw->simulation(argv[1]);
    return 0;    
}