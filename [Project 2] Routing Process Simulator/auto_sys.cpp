#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <tuple>
#include <queue>
#include "util.hpp"
#include "forward.hpp"
#include "routing.hpp"
#include "auto_sys.hpp"

/*
============= function in Auto_Sys class =============
*/

Auto_sys::Auto_sys(){}

void Auto_sys::set_AS_IP(IP_prefix IP){
    this->IP = IP;
}

void Auto_sys::set_ASN(uint32_t ASN){
    this->ASN = ASN;
}

IP_prefix Auto_sys::get_AS_IP(){
    return this->IP;
}

uint32_t Auto_sys::get_ASN(){
    return this->ASN;
}

void Auto_sys::add_fw_table(Forward_Info fw_info){
    fw_table.add_fw_info(fw_info);
}

void Auto_sys::change_fw_info_metric(int idx, uint32_t metric){
    fw_table.change_fw_info_metric(idx, metric);
}

int Auto_sys::get_fw_table_len(){
    return fw_table.get_size();
}

int Auto_sys::find_fw_info_idx(uint32_t gateway){
    int sz = this->get_fw_table_len();
    for(int i = 0; i < sz; i++){
        Forward_Info fw_info = fw_table.get_fw_info(i);
        if(in_ip_prefix_range(gateway, fw_info.get_IP_AS())){
            return i;
        }
    }
    return -1;
}

Forward_Info Auto_sys::get_fw_info(int idx){
    return fw_table.get_fw_info(idx);
}

void Auto_sys::add_neighbor(Auto_sys * neighbor){
    neighbor_AS.push_back(neighbor);
}

int Auto_sys::get_num_neighbor(){
    return neighbor_AS.size();
}

Auto_sys* Auto_sys::get_ptr_neighbor(int idx){
    return neighbor_AS[idx];
}

void Auto_sys::receive_packet_neighbor(Packet rcv_packet){
    neighbor_pkt.push(rcv_packet);
}

Auto_sys::~Auto_sys(){
}

/*
============== function in AS_Dist_Vect class ================
*/
AS_Dist_Vect::AS_Dist_Vect(){}

int AS_Dist_Vect::get_rt_table_len(){
    return this->rt_table.get_size();
}

Routing_Info* AS_Dist_Vect::get_rt_info(int idx){
    return this->rt_table.get_rt_info(idx);
}

void AS_Dist_Vect::init_rt_table(){
    int num_nb = this->get_num_neighbor();
    for(int i = 0; i < num_nb; i++){
        Routing_Info* ptr_rt_info = rt_table.init_and_add_rt_info();
        Forward_Info fw_info = this->get_fw_info(i);

        ptr_rt_info->set_gateway(fw_info.get_IP_gateway());
        ptr_rt_info->set_IP_prefix(fw_info.get_IP_AS());
        ptr_rt_info->set_total_metric(fw_info.get_metric());
        
        uint32_t* tmp_metric = new uint32_t[num_nb];
        for(int j = 0; j < num_nb; j++){
            if(i != j){
                tmp_metric[j] = INF;
            }
            else{
                tmp_metric[j] = fw_info.get_metric();
            }
        }

        ptr_rt_info->set_additional_data(metric_via_neighbor, num_nb, tmp_metric);
        delete[] tmp_metric;
    }

}

bool AS_Dist_Vect::update_rt_table()
{
    int num_nb = this->get_num_neighbor();
    //We will return this value. This value means that AS need to send packet to neighboring AS.
    bool ret = false;
    while(!neighbor_pkt.empty()){
        
        Packet packet = neighbor_pkt.front();
        uint32_t pkt_dest = packet.get_dest();
        // For synchronous update if packet timestamp is same as timestamp of AS, finish the update_rt_table()
        if(packet.get_timestamp() == timestamp){
            break;
        }
        neighbor_pkt.pop();
        // check the packet is correctly delivered
        if(!in_ip_prefix_range(pkt_dest, this->get_AS_IP())){
            continue;
        }
        if(packet.get_protocol() != Distance_Vector || packet.get_type() != Update){
            continue;
        }

        uint32_t pkt_src = packet.get_source();
        int fw_idx = this->find_fw_info_idx(pkt_src);
        if(fw_idx == -1) {
            continue;
        }
        Forward_Info fw_info = this->get_fw_info(fw_idx);

        // TODO 1-1 Allocate memory for body using 'new'. What is the number of body segments in the body? 
		int	body_size = packet.get_len() / 4 - 4;
		uint32_t* body = new uint32_t[body_size];
        packet.get_body(body);
		int	num_segment = body_size / 3;

        for(int i = 0; i < num_segment; i++){
			IP_prefix ip_tmp;
			uint32_t	target_ip = body[3 * i];
			uint32_t	target_net = body[3 * i + 1];
			uint32_t	target_mtx = body[3 * i + 2];
			
            ip_tmp.set_IPv4(target_ip); // change the input value using body;
            ip_tmp.set_netmask(target_net); // change the input value using body;
 
            int rt_idx = rt_table.find_idx(ip_tmp.get_IPv4());

            //TODO 1-2  There are three cases 
            //          1. IP address of segment is same as this AS.
            //          2. Routing table haven't yet known about IP address of segment.
            //          3. Routing table have information about IP address of segment.
            //          

            if(this->get_AS_IP().get_IPv4() == target_ip){
				continue ;
            }
            else if(rt_idx == -1){
                // Case 2 Routing table haven't yet known about IP address of segment.
                // We should make new Routing_Info for rt_table.
                // And initialize this Routing_Info.
                // Routing_Table::init_and_add_rt_info() will help this
                // Routing_Table_DV::init_rt_table() should be a good reference.
                // Do we change the value 'ret'?
				uint32_t	new_rt_metric = target_mtx + fw_info.get_metric();
				if (!(new_rt_metric < fw_info.get_metric() || new_rt_metric < target_mtx))
				{
					Routing_Info	*new_rt_info = rt_table.init_and_add_rt_info();
					new_rt_info->set_gateway(fw_info.get_IP_gateway());
					new_rt_info->set_IP_prefix(ip_tmp);
					new_rt_info->set_total_metric(new_rt_metric);
					
					uint32_t* tmp_metric = new uint32_t[num_nb];
					for(int j = 0; j < num_nb; j++){
						if (j != fw_idx)
							tmp_metric[j] = INF;
						else
							tmp_metric[j] = new_rt_metric;
					}
					new_rt_info->set_additional_data(metric_via_neighbor, num_nb, tmp_metric);
					delete[] tmp_metric;
					ret = true;
				}
			}
            else
			{
                // Case 3 Routing table have information about IP address of segment.
                // We should update metric_via_neighbor in Routing_Info.
                // Updated value should be Metric to next hop+ Sum of the metric in the rest of the path.
                // We must always update the value. 
                // Be careful overflow by controlling INF.
                // Do we always change the value 'ret'? 
				Routing_Info	*target_rt_info = rt_table.get_rt_info(rt_idx);
				Routing_Info	*gateway_rt_info = rt_table.get_rt_info(fw_idx);
				uint32_t		new_metric;
				if (rt_idx != fw_idx)
					new_metric = gateway_rt_info->get_total_metric() + target_mtx;
				else
					new_metric = target_mtx;
				if (!(new_metric < gateway_rt_info->get_total_metric() || new_metric < target_mtx))
				{
					if (target_rt_info->get_additional_data_idx(metric_via_neighbor, fw_idx) != new_metric)
					{
						target_rt_info->set_additional_data_idx(metric_via_neighbor, fw_idx, new_metric);
						uint32_t	min_matric = INF, tmp_metric;
						int			hop_idx = 0;
						for (int idx = 0; idx < num_nb; idx++)
						{
							tmp_metric = target_rt_info->get_additional_data_idx(metric_via_neighbor, idx);
							if (tmp_metric < min_matric)
							{
								min_matric = tmp_metric;
								hop_idx = idx;
							}
						}
						if (target_rt_info->get_total_metric() != min_matric)
						{
							target_rt_info->set_total_metric(min_matric);
							target_rt_info->set_gateway(rt_table.get_rt_info(hop_idx)->get_gateway());
							ret = true;
						}
					}
				}
			}
        }
        delete[] body;
    }
	
    return ret;
}

void AS_Dist_Vect::send_packet_neighbor(){
    int num_nb = this->get_num_neighbor();
    int sz = this->get_rt_table_len();

    // TODO 2-1 Allocate memory for body using 'new'. And fill the body segment.
	int			body_size = 3 * sz;
    uint16_t	len = 4 * (body_size + 4);
    uint32_t	*body = new uint32_t[body_size];
    for(int i = 0; i < sz; i++){
		Routing_Info	*body_rt_info = this->get_rt_info(i);
        body[3 * i] = body_rt_info->get_IP_prefix().get_IPv4();
        body[3 * i + 1] = body_rt_info->get_IP_prefix().get_netmask();
        body[3 * i + 2] = body_rt_info->get_total_metric();
    }

    for(int i = 0; i < num_nb; i++){
        Packet packet;
        // TODO 2-2 set the value of packet without timestamp.
        // enumeration in forward.hpp will help this.
		packet.set_body(len, body);
		packet.set_dest(this->get_ptr_neighbor(i)->get_AS_IP().get_IPv4());
		packet.set_protocol(Distance_Vector);
		packet.set_source(this->get_AS_IP().get_IPv4());
		packet.set_type(Update);
        packet.set_timestamp(timestamp);
        this->get_ptr_neighbor(i)->receive_packet_neighbor(packet);
		// packet.print_packet();
    }

    delete[] body;
}

void AS_Dist_Vect::set_num_AS(uint32_t num_AS){
    this->num_AS = num_AS;    
}

uint32_t AS_Dist_Vect::get_num_AS(){
    return this->num_AS;
}

void AS_Dist_Vect::change_rt_info(IP_prefix ip, uint32_t metric){
    int rt_idx = rt_table.find_idx(ip.get_IPv4());
    int fw_idx = this->find_fw_info_idx(ip.get_IPv4());
    int num_nb = this->get_num_neighbor();
    Routing_Info* ptr_rt_info = rt_table.get_rt_info(rt_idx);

    ptr_rt_info->set_additional_data_idx(metric_via_neighbor, fw_idx, metric);

    uint32_t tmp = INF, cmp;
    int hop_idx = 0;

    for(int j = 0; j < num_nb; j++){
        cmp = ptr_rt_info->get_additional_data_idx(metric_via_neighbor, j);
        if(tmp > cmp){
            tmp = cmp;
            hop_idx = j;
        }
    }

    ptr_rt_info->set_total_metric(tmp);
    ptr_rt_info->set_gateway(this->get_fw_info(hop_idx).get_IP_gateway());
}

void AS_Dist_Vect::print_rt_table(){
    int rt_size = this->get_rt_table_len();
    int num_nb = this->get_num_neighbor();
    printf("==== Routing Table ====\n");
    printf("IP of AS : ");
    printIP(this->get_AS_IP().get_IPv4());
    printf("ASN : %u\n", this->get_ASN());
    for(int i = 0; i < rt_size; i++){
        Routing_Info* ptr_rt_info = this->get_rt_info(i);
        printf("\n");
        printf("IP destination : ");
        printIP(ptr_rt_info->get_IP_prefix().get_IPv4());
        printf("IP dest_netmask : ");
        printIP(ptr_rt_info->get_IP_prefix().get_netmask());
        printf("IP gateway : ");
        printIP(ptr_rt_info->get_gateway());
        printf("Total metric : %u\n", ptr_rt_info->get_total_metric());

        printf("Metric via neighbor : ");
        for(int j = 0; j < num_nb; j++){
            printf("%u ", ptr_rt_info->get_additional_data_idx(metric_via_neighbor, j));
        }
        printf("\n");
    }
}

AS_Dist_Vect::~AS_Dist_Vect(){
}

/*
============== function in AS_Link_Stat class ================
*/
AS_Link_Stat::AS_Link_Stat(){}

int AS_Link_Stat::get_rt_table_len(){
    return this->rt_table.get_size();
}

Routing_Info* AS_Link_Stat::get_rt_info(int idx){
    return this->rt_table.get_rt_info(idx);
}

void AS_Link_Stat::init_rt_table(){
    // Just refer this annotation. This init_rt_table() should be an empty function.
    /*
    int num_nb = this->get_num_neighbor();
    int num_AS = this->get_num_AS();
    int ASN_nb, ASN_this = this->get_ASN();

    for(int i = 0; i < num_nb; i++){
        Routing_Info* ptr_rt_info = rt_table.init_and_add_rt_info();
        Forward_Info fw_info = this->get_fw_info(i);
        uint32_t ASN = this->get_ASN();

        ptr_rt_info->set_IP_prefix(fw_info.get_IP_AS());
        ptr_rt_info->set_additional_data(additional::ASN, 1, &ASN);

        uint32_t* tmp_metric = new uint32_t[num_AS];
        ASN_nb = get_ptr_neighbor(i)->get_ASN();
        for(int j = 0; j < num_AS; j++){
            tmp_metric[j] = INF;
        }

        ptr_rt_info->set_additional_data(metric_to_AS, num_AS, tmp_metric);
        delete[] tmp_metric;
    }
    */
}

bool AS_Link_Stat::update_rt_table(){
    bool ret = update_map();
    // updates routing table using dijkstra algorithm when packet should be sent and size of routing table is equal to num_AS -1
    if(this->get_rt_table_len() == this->get_num_AS() - 1){
        update_table_dijkstra(); // test
    }
    return ret;
}

bool AS_Link_Stat::update_map(){
    //We will return this value. This value means that AS need to send packet to neighboring AS.
    bool ret = false;
    int num_nb = this->get_num_neighbor();
    int num_AS = this->get_num_AS();
    while(!neighbor_pkt.empty()){
        Packet packet = neighbor_pkt.front();
        uint32_t pkt_dest = packet.get_dest();
        // For synchronous update if packet timestamp is same as timestamp of AS, finish the update_rt_table()
        if(packet.get_timestamp() == timestamp){
            break;
        }
        neighbor_pkt.pop();

        // check the packet is correctly delivered
        if(!in_ip_prefix_range(pkt_dest, this->get_AS_IP())){
            continue;
        }
        if(packet.get_protocol() != Link_State || packet.get_type() != Update){
            continue;
        }

        uint32_t pkt_src = packet.get_source();
        int fw_idx = this->find_fw_info_idx(pkt_src);
        if(fw_idx == -1) {
            continue;
        }
        // TODO 3-1 Allocate memory for body using 'new'. What is the number of body segments in the body?
        int			body_size = packet.get_len() / 4 - 4;
		uint32_t	*body = new uint32_t[body_size];
        packet.get_body(body);
        int num_segment = body_size / (num_AS + 3);

        for(int i = 0; i < num_segment; i++){
			uint32_t	target_ip = body[(num_AS + 3) * i];
			uint32_t	target_net = body[(num_AS + 3) * i + 1];
			uint32_t	target_ASN = body[(num_AS + 3) * i + 2];
            IP_prefix	ip_tmp;
            ip_tmp.set_IPv4(target_ip); // change the input value using body.
            ip_tmp.set_netmask(target_net); // change the input value using body.

            int rt_idx = rt_table.find_idx(ip_tmp.get_IPv4());
            //TODO 3-2  There are three cases 
            //          1. IP address of segment is same as this AS.
            //          2. Routing table haven't yet known about IP address of segment.
            //          3. Routing table have information about IP address of segment.
            //          

            if(this->get_AS_IP().get_IPv4() == target_ip){
                // Case 1 IP address of segment is same as this AS.
				continue ;
            }
            else if(rt_idx == -1){
                // Case 2 Routing table haven't yet known about IP address of segment.
                // We should make new Routing_Info for rt_table.
                // And initialize this Routing_Info.
                // Routing_Table::init_and_add_rt_info() will help this
                // Routing_Table_LS::init_rt_table() should be a good reference.
                // Do we change the value 'ret'?

				// if every map of each ASN has same val.
				Routing_Info	*new_rt_info = rt_table.init_and_add_rt_info();
				Forward_Info	fw_info = this->get_fw_info(fw_idx);
				uint32_t		*tmp_metric = new uint32_t[num_AS];

				new_rt_info->set_IP_prefix(ip_tmp);
				new_rt_info->set_additional_data(add_t::ASN, fw_idx, &target_ASN);
				new_rt_info->set_gateway(target_ip);
				new_rt_info->set_total_metric(INF);

				for (int data_idx = 0; data_idx < num_AS; data_idx++)
						tmp_metric[data_idx] = body[(num_AS + 3) * i + 3 + data_idx];

				new_rt_info->set_additional_data(metric_to_AS, num_AS, tmp_metric);
				delete[] tmp_metric;

				ret = true;
            }
            else{
                // Case 3 Routing table have information about IP address of segment.
                // We should update metric_to_AS in Routing_Info.
                // Updated value is in the packet body.
                // We must always update the value. 
                // Do we always change the value 'ret'? 
				Routing_Info	*target_rt_info = rt_table.get_rt_info(rt_idx);
				uint32_t		target_data;
				uint32_t		new_data;

				for (int data_idx = 0; data_idx < num_AS; data_idx++)
				{
					target_data = target_rt_info->get_additional_data_idx(metric_to_AS, data_idx);
					new_data = body[(num_AS + 3) * i + 3 + data_idx];
					if (target_data != new_data)
					{
						target_rt_info->set_additional_data_idx(metric_to_AS, data_idx, new_data);
						target_rt_info->set_gateway(target_ip);
						ret = true;
					}
				}
            }
        }
        delete[] body;
    }
    return ret;
}

void AS_Link_Stat::update_table_dijkstra(){
    // For dijkstra algorithm, we are going to use STL priority queue of tuiu. tuiu is defined in util.hpp.
    // We will use first value as the value related with metric.
    // We will use second value as the ASN.
    // We will use third value as the IP address of next hop.
    // Note that first value is unsigned int, and top of priority queue in STL is biggest one.
    std::priority_queue<tuiu> dijk_queue;
    tuiu output_tuple, input_tuple;
    uint32_t val_fir, val_thr;
    int val_sec;
    int num_AS = this->get_num_AS();
    bool* check_trip = new bool[num_AS];
    int num_nb = this->get_fw_table_len();
    int travelled = 0;

    for(int i = 0; i < num_AS; i++)
        check_trip[i] = false;

    check_trip[this->get_ASN()] = true;
    travelled++;

    // TODO 4-1 initialize the prioirty queues
    for(int i = 0; i < num_nb; i++){
        Forward_Info fw_info = get_fw_info(i);
        int rt_idx = rt_table.find_idx(fw_info.get_IP_gateway());
        Routing_Info* ptr_rt_info = get_rt_info(rt_idx);
        
        // change the value correctly.
        val_fir = INF - fw_info.get_metric();
        val_sec = ptr_rt_info->get_additional_data_idx(add_t::ASN, rt_idx);
        val_thr = fw_info.get_IP_gateway();
        input_tuple = tuiu(val_fir, val_sec, val_thr);
        dijk_queue.push(input_tuple);
    }

    //TODO 4-2 Dijkstra's algorithm
    while(!dijk_queue.empty() && travelled < num_AS){
        output_tuple = dijk_queue.top();
        dijk_queue.pop();

		// printf("test : AS %d, %d\n", this->get_ASN(), std::get<1>(output_tuple));
        //check that this AS has been travelled
        if(check_trip[std::get<1>(output_tuple)]){
            continue;
        }
        check_trip[std::get<1>(output_tuple)] = true;
        travelled++;

        //Find the pointer of Routing_Info* using ASN;
        int rt_size = rt_table.get_size();
        Routing_Info* ptr_rt_info;
        for(int i = 0; i < rt_size; i++){
            ptr_rt_info = rt_table.get_rt_info(i);
            if(ptr_rt_info->get_additional_data_idx(add_t::ASN, 0) == std::get<1>(output_tuple))
                break;
        }
        
        // set the correct value
		uint32_t		total_metric = INF - std::get<0>(output_tuple);
		uint32_t		trip_AS = std::get<1>(output_tuple);
		uint32_t		gateway = std::get<2>(output_tuple);
    	std::priority_queue<tuiu>	tmp_queue;
		tuiu						tmp_tuiu;
        Routing_Info	*trip_rt_info;
		for (int idx = 0; idx < num_AS; idx++)
		{
			uint32_t	data = ptr_rt_info->get_additional_data_idx(metric_to_AS, idx);
			if (data != INF)
			{
				tmp_tuiu = tuiu(INF - (total_metric + data), idx, gateway);
				tmp_queue.push(tmp_tuiu);
			}
		}
		
        ptr_rt_info->set_total_metric(total_metric);
        ptr_rt_info->set_gateway(gateway);

        int	tmp_size = tmp_queue.size();
        // push correct tuiu into priority queue. Note that INF means disconnection
        for(int i = 0; i < tmp_size; i++){
            tmp_tuiu = tmp_queue.top();
			tmp_queue.pop();

            input_tuple = tmp_tuiu;
            dijk_queue.push(input_tuple);
        }

    }
    delete[] check_trip;
    
}

void AS_Link_Stat::send_packet_neighbor(){
    int num_nb = this->get_num_neighbor();
    int num_AS = this->get_num_AS();
    int rt_sz = this->get_rt_table_len();
    
    // TODO 5-1 Allocate memory for body using 'new'. And fill the body segment.
    // Note that packet in Link-state routing protocol, information of AS itself should be included.
    int			body_size = (num_AS + 3) * (rt_sz + 1);
    uint16_t	len = 4 * (body_size + 4);
    uint32_t*	body = new uint32_t[body_size];
	
    for(int i = 0; i < rt_sz + 1; i++){
		if (i != rt_sz)
		{
			Routing_Info	*body_rt_info = this->get_rt_info(i);
			body[(num_AS + 3) * i] = body_rt_info->get_IP_prefix().get_IPv4();
			body[(num_AS + 3) * i + 1] = body_rt_info->get_IP_prefix().get_netmask();
			body[(num_AS + 3) * i + 2] = body_rt_info->get_additional_data_idx(add_t::ASN, i);
			for (int as_idx = 0; as_idx < num_AS; as_idx++)
				body[(num_AS + 3) * i + 3 + as_idx] = body_rt_info->get_additional_data_idx(metric_to_AS, as_idx);
		}
		else
		{
			body[(num_AS + 3) * i] = this->get_AS_IP().get_IPv4();
			body[(num_AS + 3) * i + 1] = this->get_AS_IP().get_netmask();
			body[(num_AS + 3) * i + 2] = this->get_ASN();
			for (int AS_idx = 0; AS_idx < num_AS; AS_idx++)
				body[(num_AS + 3) * i + 3 + AS_idx] = INF;
			for (int idx = 0; idx < num_AS; idx++)
			{
				if (idx < rt_sz)
				{
					Routing_Info	*body_rt_info = this->get_rt_info(idx);
					uint32_t		rt_metric = body_rt_info->get_additional_data_idx(metric_to_AS, this->get_ASN());
					uint32_t		rt_ASN = body_rt_info->get_additional_data_idx(add_t::ASN, idx);
					body[(num_AS + 3) * i + 3 + rt_ASN] = rt_metric;
				}
				else if (idx < this->get_fw_table_len())
				{
				for (int fw_idx = 0; fw_idx < this->get_fw_table_len(); fw_idx++)
					{
						uint32_t	fw_metric = this->get_fw_info(fw_idx).get_metric();
						uint32_t	fw_ASN = this->get_ptr_neighbor(fw_idx)->get_ASN();
						body[(num_AS + 3) * i + 3 + fw_ASN] = fw_metric;
					}
				}
				else
					break ;
			}
		}
    }
    
    for(int i = 0; i < num_nb; i++){
        Packet packet;
        // TODO 5-2 set the value of packet without timestamp.
        // enumeration in forward.hpp will help this.
		packet.set_body(len, body);
		packet.set_dest(this->get_ptr_neighbor(i)->get_AS_IP().get_IPv4());
		packet.set_protocol(Link_State);
		packet.set_source(this->get_AS_IP().get_IPv4());
		packet.set_type(Update);
        packet.set_timestamp(timestamp);
        this->get_ptr_neighbor(i)->receive_packet_neighbor(packet);
		// packet.print_packet();
    }

    delete[] body;
}

void AS_Link_Stat::set_num_AS(uint32_t num_AS){
    this->num_AS = num_AS;
}

uint32_t AS_Link_Stat::get_num_AS(){
    return this->num_AS;
}

void AS_Link_Stat::change_rt_info(IP_prefix ip, uint32_t metric){
    int rt_idx = rt_table.find_idx(ip.get_IPv4());
    Routing_Info* ptr_rt_info = this->get_rt_info(rt_idx);
    ptr_rt_info->set_additional_data_idx(metric_to_AS, this->get_ASN(), metric);

}

void AS_Link_Stat::print_rt_table(){
    int rt_size = this->get_rt_table_len();
    int num_as = this->get_num_AS();
    printf("==== Routing Table ====\n");
    printf("IP of AS : ");
    printIP(this->get_AS_IP().get_IPv4());
    printf("ASN : %u\n", this->get_ASN());
    for(int i = 0; i < rt_size; i++){
        Routing_Info* ptr_rt_info = this->get_rt_info(i);
        printf("\n");
        printf("IP destination : ");
        printIP(ptr_rt_info->get_IP_prefix().get_IPv4());
        printf("IP dest_netmask : ");
        printIP(ptr_rt_info->get_IP_prefix().get_netmask());
        printf("IP gateway : ");
        printIP(ptr_rt_info->get_gateway());
        printf("Total metric : %u\n", ptr_rt_info->get_total_metric());

        printf("Metric to AS : ");
        for(int j = 0; j < num_as; j++){
            if(ptr_rt_info->get_additional_data_idx(metric_to_AS, j) != INF)
                printf("%u ", ptr_rt_info->get_additional_data_idx(metric_to_AS, j));
            else printf("INF ");
        }
        printf("\n");
    }
}

AS_Link_Stat::~AS_Link_Stat(){}

/*
============== function in AS_Path_Vect class ================
*/
AS_Path_Vect::AS_Path_Vect(){}

int AS_Path_Vect::get_rt_table_len(){
    return this->rt_table.get_size();
}

Routing_Info* AS_Path_Vect::get_rt_info(int idx){
    return this->rt_table.get_rt_info(idx);
}

void AS_Path_Vect::init_rt_table(){
    int num_nb = this->get_num_neighbor();
    for(int i = 0; i < num_nb; i++){
        Routing_Info* ptr_rt_info = rt_table.init_and_add_rt_info();
        Forward_Info fw_info = this->get_fw_info(i);

        ptr_rt_info->set_gateway(fw_info.get_IP_gateway());
        ptr_rt_info->set_IP_prefix(fw_info.get_IP_AS());
        ptr_rt_info->set_total_metric(1);
        
        uint32_t* tmp_metric = new uint32_t[1];
        tmp_metric[0] = fw_info.get_IP_gateway();

        ptr_rt_info->set_additional_data(path_to_AS, 1, tmp_metric);
        delete[] tmp_metric;
    }
}

bool AS_Path_Vect::update_rt_table(){
    int num_nb = this->get_num_neighbor();
    //We will return this value. This value means that AS need to send packet to neighboring AS.
    bool ret = false;

    while(!neighbor_pkt.empty()){
        Packet packet = neighbor_pkt.front();
        uint32_t pkt_dest = packet.get_dest();
        // For synchronous update if packet timestamp is same as timestamp of AS, finish the update_rt_table()
        if(packet.get_timestamp() == timestamp){
            break;
        }
        neighbor_pkt.pop();
        if(!in_ip_prefix_range(pkt_dest, this->get_AS_IP())){
            continue;
        }
        if(packet.get_protocol() != Path_Vector || packet.get_type() != Update){
            continue;
        }

        uint32_t pkt_src = packet.get_source();
        int fw_idx = this->find_fw_info_idx(pkt_src);
        if(fw_idx == -1) {
            continue;
        }
		// printf("test : PV at %d before get_body from ", this->get_ASN());
		// printIP(pkt_src);
        // TODO 6-1 Allocate memory for body using 'new'
        int			body_size = packet.get_len() / 4 - 4;
		uint32_t	*body = new uint32_t[body_size];
        packet.get_body(body);
		// printf("test : PV at %d after get_body, size is %d\n", this->get_ASN(), body_size);
		
        // TODO 6-2 How can we separate the segment? The key is path_length in packet segment.
        int num_segment = 0;
		int	each_body_size = 0;
		// printf("test : \n");
		while (each_body_size != body_size)
		{
			each_body_size += 3;
			each_body_size += body[each_body_size - 1];
			num_segment++;
			// printf("test : tmp size is %d\n", each_body_size);
		}
		// printf("test : PV at %d has %d segments\n", this->get_ASN(), num_segment);
		int	body_cnt = 0;
        for(int i = 0; i < num_segment; i++){
			// printf("test : PV at %d parsing body %d\n", this->get_ASN(), i);
			uint32_t	target_ip = body[body_cnt++];
			uint32_t	target_net = body[body_cnt++];
			uint32_t	target_length = body[body_cnt++] + 1;
            IP_prefix ip_tmp;
            ip_tmp.set_IPv4(target_ip); // change the input value using body.
            ip_tmp.set_netmask(target_net); // change the input value using body.

            int rt_idx = rt_table.find_idx(ip_tmp.get_IPv4());
            //TODO 6-3  There are three cases 
            //          1. IP address of segment is same as this AS.
            //          2. Routing table haven't yet known about IP address of segment.
            //          3. Routing table have information about IP address of segment.
            //          
			// printf("test : length %d\n", target_length);

            if(this->get_AS_IP().get_IPv4() == target_ip){
                // Case 1 IP address of segment is same as this AS.
				body_cnt += target_length - 1;
				continue;
            }
            else if(rt_idx == -1){
                // Case 2 Routing table haven't yet known about IP address of segment.
                // We should make new Routing_Info for rt_table.
                // And initialize this Routing_Info.
                // Routing_Table::init_and_add_rt_info() will help this
                // Routing_Table_PV::init_rt_table() should be a good reference.
                // Do we change the value 'ret'?
				Routing_Info	*new_rt_info = rt_table.init_and_add_rt_info();
				uint32_t		*tmp_metric = new uint32_t[target_length];

				new_rt_info->set_gateway(pkt_src);
				new_rt_info->set_IP_prefix(ip_tmp);
				new_rt_info->set_total_metric(target_length);
				// printf("test : \n");

				tmp_metric[0] = pkt_src;
				for (int metric_idx = 1; metric_idx < target_length; metric_idx++)
					tmp_metric[metric_idx] = body[body_cnt++];
				// printf("test : \n");
				
				new_rt_info->set_additional_data(path_to_AS, target_length, tmp_metric);
				delete[] tmp_metric;
				// printf("test : \n");

				ret = true;
				// printf("test : make new rt end\n");
            }
            else{
                // Case 3 Routing table have information about IP address of segment.
                // We should update path in Routing_Info.
                // Updated total_metric value is total_metric_received + 1
                // We update when we can make the shorter path using received path.
                // Do we always change the value 'ret'? 
				Routing_Info	*target_rt_info = rt_table.get_rt_info(rt_idx);
				uint32_t		target_rt_length = target_rt_info->get_total_metric();
                if (target_length < target_rt_length)
				{
					uint32_t	*tmp_metric = new uint32_t[target_length];

					target_rt_info->set_gateway(pkt_src);
					target_rt_info->set_total_metric(target_length);

					tmp_metric[0] = pkt_src;
					for (int metric_idx = 1; metric_idx < target_length; metric_idx++)
						tmp_metric[metric_idx] = body[body_cnt++];

					target_rt_info->set_additional_data(path_to_AS, target_length, tmp_metric);
					delete[] tmp_metric;

					ret = true;
				}
				else
					body_cnt += target_length - 1;
            }
        }
    }
    return ret;
}

void AS_Path_Vect::send_packet_neighbor(){
    int rt_sz = this->get_rt_table_len();
    int num_nb = this->get_num_neighbor();

    // TODO 7-1 Allocate memory for body using 'new'. And fill the body segment.
    // How do we calculate body_size? Using for loop.

    int	body_size = 3 * rt_sz;
    for(int i = 0; i < rt_sz; i++){
        body_size += this->get_rt_info(i)->get_total_metric();
    }
	uint16_t	len = 4 * (body_size + 4);

    uint32_t* body = new uint32_t[body_size];
	int	body_cnt = 0;
    for(int i = 0; i < rt_sz; i++){
		Routing_Info	*body_rt_info = this->get_rt_info(i);
		body[body_cnt++] = body_rt_info->get_IP_prefix().get_IPv4();
		body[body_cnt++] = body_rt_info->get_IP_prefix().get_netmask();
		body[body_cnt++] = body_rt_info->get_total_metric();
		for (int body_idx = 0; body_idx < body_rt_info->get_total_metric(); body_idx++)
			body[body_cnt++] = body_rt_info->get_additional_data_idx(path_to_AS, body_idx);
    }

    for(int i = 0; i < num_nb; i++){
        Packet packet;
        // TODO 7-2 set the value of packet without timestamp.
        // enumeration in forward.hpp will help this.
		packet.set_body(len, body);
		packet.set_dest(this->get_ptr_neighbor(i)->get_AS_IP().get_IPv4());
		packet.set_protocol(Path_Vector);
		packet.set_source(this->get_AS_IP().get_IPv4());
		packet.set_type(Update);
        packet.set_timestamp(timestamp);
        this->get_ptr_neighbor(i)->receive_packet_neighbor(packet);
		// packet.print_packet();
    }
    delete[] body;
}

void AS_Path_Vect::set_num_AS(uint32_t num_AS){
    return;
}

uint32_t AS_Path_Vect::get_num_AS(){
    printf("We don\'t need total number of AS in Path-Vector system\n");
    return 0;
}

void AS_Path_Vect::change_rt_info(IP_prefix ip, uint32_t metric){
    YELLOW_PRTF("Metric change does not effect to Path-Vector protocol.\n");
}

void AS_Path_Vect::print_rt_table(){
    int rt_size = this->get_rt_table_len();
    printf("==== Routing Table ====\n");
    printf("IP of AS : ");
    printIP(this->get_AS_IP().get_IPv4());
    printf("ASN : %u\n", this->get_ASN());
    for(int i = 0; i < rt_size; i++){
        Routing_Info* ptr_rt_info = this->get_rt_info(i);
        printf("\n");
        printf("IP destination : ");
        printIP(ptr_rt_info->get_IP_prefix().get_IPv4());
        printf("IP dest_netmask : ");
        printIP(ptr_rt_info->get_IP_prefix().get_netmask());
        printf("IP gateway : ");
        printIP(ptr_rt_info->get_gateway());
        printf("Total metric : %u\n", ptr_rt_info->get_total_metric());

        printf("path to AS : ");
        int sz = ptr_rt_info->get_additional_data_size(path_to_AS);
        for(int j = 0; j < sz; j++){
            printIP_no_new_line(ptr_rt_info->get_additional_data_idx(path_to_AS, j));
            if(j!=sz-1) printf(" - ");
        }
        printf("\n");
    }
}

AS_Path_Vect::~AS_Path_Vect(){}