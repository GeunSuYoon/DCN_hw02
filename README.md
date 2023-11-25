# data_commu_network_hw02

# flow
## make_network(char *filename)

``` C++
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

```

  - filename으로 들어온 csv 파일을 연다. 예시는 아래와 같다.

    ----
  - graph1.csv :
  - 5,
  - 138.0.0.0,24,
  - 139.0.0.0,24,
  - 140.0.0.0,24,
  - 141.0.0.0,24,
  - 142.0.0.0,24,
  - 5,
  - 0,1,1,
  - 1,2,2,
  - 2,3,4,
  - 3,4,8,
  - 4,0,16,

    ----
  - 첫 줄의 숫자는 AS의 개수(num_as)로, 숫자만큼 AS를 만든다.
  - 이후 num_as개의 줄의 모양은 "???.???.???.???,???,"로 ',' 앞은 ipv4, 뒤는 netmask로 i번째 AS에 각 줄번째의 ipv4, netmask, i, num_as를 보내 AS를 셋팅.
  - 중간에 있는 숫자는 link의 개수(num_link).
  - 이후 num_link개의 줄의 모양은 "???,???,???," 로 각 숫자는 as_a, as_b, metric으로 저장돼 make_link 함수로 전달된다.

## make_link(int as_a, int as_b, uint32_t metric)

``` C++
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

```
  - forward_info fw_info_a, fw_info_b, IP_prefix ip_tmp 선언.
  - fw_info_a에 정보를 저장하고 AS[as_a]에 해당 정보를 저장한다.
  - as_b 번째 AS_IP를 ip_tmp에 저장한다.
  - fw_info_a에 ip_tmp의 IPv4를 게이트웨이로, IP_AS를 ip_tmp로, metric을 metric으로 저장한다.
  - 이후 AS[as_a]->get_fw_table_len()을 sz_a에 저장.
  - AS[as_a]에 fw_info_a를 추가하고 AS[as_b]를 neighbor로 추가한다.
  - fw_info_b는 fw_info_a에 대해 as_a와 as_b만 바꿔 진행한다.

## init_simulation()

``` C++
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
            tmp = AS[i]->update_rt_table();
            check_update_continue |= tmp;
            if(tmp)
                AS[i]->send_packet_neighbor();
        }
        if(!check_update_continue) break;
    }
    for(int i = 0; i < this->num_AS_net; i++){
        //AS[i]->print_rt_table();
    }
}
```

  - 우선 num_AS_net만큼 각 AS[i]에 init_rt_table()과 send_packet_neighbor()로 rt_table을 초기화하고 해당 데이터를 페킷으로 이웃에 전달한다.
  - while문을 실행한다.
  - time_stamp를 늘리고 check_update_continue를 false로 설정한다.
  - num_AS_net만큼 돌며 각 AS[i]에 update_rt_table()을 실행한 결과를 tmp에 저장한다.
  - check_update_continue에 tmp의 결과를 |=로 저장한다.
  - tmp가 참일 경우 send_packet_neighbor()를 실행해 패킷을 보낸다.
  - 다 돌고 check_update_continue가 거짓일 경우 while문을 탈출한다.

## init_rt_table()

### AS_Dist_Vect :

``` C++
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
```

  - 해당 AS의 이웃 수(num_nb) 만큼 idx = i로 for문을 돈다. 
  - rt_table에 새로운 Routing_Info *ptr_rt_info를 추가한다. Forward_Info fw_info는 해당 AS의 i번째 fw_info이다.
  - ptr_rt_info에 해당 fw_info의 gateway, IP_AS, metric을 gateway, IP_prefix, total_metric으로 복사한다.
  - tmp_metric을 num_nb만큼 malloc한다.
  - idx = j로 for문을 num_nb만큼 돈다.
  - i != j이면 tmp_metric[j] = INF, i == j이면 tmp_metric[j] = fw_info.get_metric()
  - ptr_rt_info에 metric_via_neighbor, num_nb, tmp_metric을 인수로 set_additional_data를 실행해 해당 rt_info에 metric을 저장한다.

## set_additional_data()

### Routing_Info_DV :

``` C++
void Routing_Info_DV::set_additional_data(add_t ad, int num_data, uint32_t* additional_data){
    if(ad == additional::metric_via_neighbor){
        this->set_metric_via_neighbor(num_data, additional_data);
    }
};

void Routing_Info_DV::set_metric_via_neighbor(int num_nb, uint32_t* metric_via_neighbor){
    this->metric_via_neighbor = new uint32_t[num_nb];
    this->num_nb = num_nb;

    for(int i = 0; i < num_nb; i++){
        this->metric_via_neighbor[i] = metric_via_neighbor[i];
    }
}
```

  - ad가 metric_via_neighbor일 경우만 실행한다.
  - 해당 rt_info의 metric_via_neighbor를 들어온 data를 복사한다.

## send_packet_neighbor()

### AS_Dist_Vect :

``` C++
void AS_Dist_Vect::send_packet_neighbor(){
    int num_nb = this->get_num_neighbor();
    int sz = this->get_rt_table_len();
	printf("send p n num_nb : [%d]\n", num_nb);

    // TODO 2-1 Allocate memory for body using 'new'. And fill the body segment.
    int len = sz * 12 + 16;
    uint32_t* body = new uint32_t[3 * sz];
    
    for(int i = 0; i < sz; i++){
		Routing_Info* ptr_rt_info = this->get_rt_info(i);
        body[3 * i] = ptr_rt_info->get_IP_prefix().get_IPv4();
        body[3 * i + 1] = ptr_rt_info->get_IP_prefix().get_netmask();
        body[3 * i + 2] = ptr_rt_info->get_total_metric();
    }
    
    for(int i = 0; i < num_nb; i++){
        Packet packet;
        // TODO 2-2 set the value of packet without timestamp.
        // enumeration in forward.hpp will help this.
		packet.set_body(len, body);
		packet.set_dest(body[3 * i]);
		packet.set_protocol(Distance_Vector);
		packet.set_source(this->get_AS_IP().get_IPv4());
		packet.set_type(Update);
        packet.set_timestamp(timestamp);
        this->get_ptr_neighbor(i)->receive_packet_neighbor(packet);
    }

    delete[] body;
}
```
  - 해당 rt_table을 packet으로 만들어 주변 AS의 neighbor_pkt에 전달.

## update_rt_table()

### 
