# data_commu_network_hw02

# flow
## make_network(char *filename) 함수:

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

## make_link(int as_a, int as_b, uint32_t metric) 함수:

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

```
  - 우선 num_AS_net만큼 각 AS[i]에 init_rt_table()과 send_packet_neighbor()로 rt_table을 초기화하고 해당 데이터를 페킷으로 이웃에 전달한다.
  - while문을 실행한다.
  - time_stamp를 늘리고 check_update_continue를 false로 설정한다.
  - num_AS_net만큼 돌며 각 AS[i]에 update_rt_table()을 실행한 결과를 tmp에 저장한다.
  - check_update_continue에 tmp의 결과를 |=로 저장한다.
  - tmp가 참일 경우 send_packet_neighbor()를 실행해 패킷을 보낸다.
  - 다 돌고 check_update_continue가 거짓일 경우 while문을 탈출한다.
