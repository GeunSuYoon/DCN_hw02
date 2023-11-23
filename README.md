# data_commu_network_hw02

## Routiong Protocol
### A. Introduction to Routing Protocol
스위치 또는 라우터는 패킷의 위치를 결정합니다.
네트워크의 전체 그래프를 알면 라우터가 쉽게 경로를 결정할 수 있습니다.
그러나 라우터는 처음에 이 정보를 알 수 없습니다.
라우팅 테이블을 사용하여 의사 결정 과정을 돕습니다.
라우팅 테이블은 대상의 IP 접두사에 따라 게이트웨이의 IP 주소를 알립니다.
그렇다면 라우팅 테이블을 어떻게 만드느냐가 효율적인 패킷 전달을 위한 좋은 질문입니다.

인간 전문가는 모든 라우터의 라우팅 테이블을 작성할 수 있습니다.
그러나 인터넷이 증가함에 따라 전 세계의 모든 라우터를 업데이트할 수 있는 여유가 없습니다. 라우팅 테이블을 자동으로 업데이트하는 아웃팅 프로토콜을 제안합니다.
이 목표를 달성하기 위해 라우터는 서로 통신하여 경로를 선택할 수 있는 정보를 변경합니다.
라우팅 유형, 도메인 유형 또는 라우팅 알고리즘 등으로 라우팅 프로토콜을 분류할 수 있습니다.
거리-벡터, 링크-상태, 경로-벡터는 라우팅 알고리즘의 일종입니다.
이 프로젝트에서는 이 세 가지 유형의 라우팅 알고리즘을 구현할 것입니다.

거리-벡터 알고리즘은 라우터 자체와 목적지 사이의 거리를 사용하여 최단 경로를 찾습니다.
Bellman-Ford 알고리즘은 라우터가 거리 정보를 이용하여 최단 경로를 찾을 수 있도록 도와줍니다.
링크 상태 알고리즘은 라우터의 연결 그래프를 이용하여 최단 경로를 찾습니다.
각 라우터는 네트워크 그래프의 전체 구조를 알기 위해 인접 라우터에 대한 정보를 브로드캐스트합니다.
우리는 Dijkstra 알고리즘을 사용하여 이 네트워크 그래프에서 최단 경로를 찾습니다.
경로-벡터 알고리즘은 패킷이 대상에 도달하기 위해 통과하는 라우터 목록을 사용하여 경로를 가져옵니다.

### B. Configuration of Simulator
우리는 C++를 이용하여 시뮬레이터를 구현했습니다. 이 시뮬레이터에는 여러 클래스가 있습니다.
아래는 각 학급에 대한 간단한 설명입니다. 부록 A에서는 각 학급의 기능을 제공하고 있습니다.

클래스 네트워크는 전체 시뮬레이터를 나타냅니다. 상속된 클래스는 3개입니다
Network_DV, Network_LS, Network_PV.
상속된 각 클래스는 각 라우팅 알고리즘을 사용하는 시뮬레이터를 나타냅니다.

Class Auto_sys는 자율 시스템(AS)을 나타냅니다. 자율 시스템은 네트워크 운영자의 제어 하에 연결된 IP 라우팅 프리픽스들의 집합입니다.
본 시뮬레이터에서 각 자율 시스템은 1개의 IP 프리픽스를 가지며, 각 자율 시스템에서 대표 라우터의 IP 주소는 IP 프리픽스 범위의 시작에 있다고 가정합니다. 상속된 클래스는 AS_Dist_Vect, AS_Link_Stat, AS_Path_Vect 3개입니다. 상속된 클래스는 각각의 라우팅 알고리즘을 사용하는 자율 시스템을 나타냅니다.

Class Routing_Table은 라우터에서 라우팅 테이블을 나타냅니다. Routing_Info 클래스의 포인터 벡터가 있습니다. Routing_Info는 라우팅 테이블에서 라우팅 정보를 나타냅니다. Routing_Table, Routing_Table_DV, Routing_Table_LS, Routing_Table_PV에 대한 상속 클래스와 Routing_Info, Routing_Info_DV, Routing_Info_LS, Routing_Info_PV에 대한 상속 클래스가 있습니다.

Class Forward_Table은 라우터에서 전달 테이블을 나타냅니다. 전달 정보를 나타내는 class Forward_Info 벡터가 있습니다. 이 전달 테이블은 이웃 자치 시스템의 정보만 포함하고 있습니다.

Class Packet은 라우터가 통신하는 패킷을 나타냅니다. 라우팅 알고리즘별 패킷의 세부 구조에 대해서 설명하겠습니다.

클래스 IP_prefix는 IP 접두사를 나타냅니다. IP 접두사는 147.46.0.0/15와 같이 표현할 수 있습니다.
코드를 컴파일한 후 아래 명령어를 사용하여 이진 파일을 실행할 수 있습니다.

./route_sim <filename> D/L/P
대문자, D, L 또는 P는 모드 거리-벡터, 링크-상태, 경로-벡터를 의미합니다.
<filename>에 시뮬레이션할 그래프 파일을 입력합니다.
그래프 파일은 graph_csv 폴더에 있어야 하며 .csv 형식이어야 합니다.
그래프 파일의 형식은 다음과 같습니다.
첫 번째 라인은 자율 시스템의 수가 될 것입니다.
(n+1)번째 두번째 줄은 각 자율 시스템의 IP 접두사에 대한 IP 주소와 넷마스크가 될 것입니다.
(n+2)번째 줄은 네트워크의 링크 수 m입니다.
(n+2)번째부터 (n+m+2)번째 줄까지 세 개의 정수가 줄에 있습니다.
이전의 두 정수는 링크의 끝점을 나타내고 마지막 정수는 링크의 메트릭(비용)을 나타냅니다.
이 파일 형식에 따라 자신만의 네트워크를 만들 수 있습니다.

시스템 창에 이전 명령을 입력하면 시뮬레이터는 처음에 csv 파일에 이어 네트워크를 만들고 라우팅 테이블을 만듭니다.
그 후에 시뮬레이터를 사용할 명령어를 입력하면 됩니다. 명령어에 대한 자세한 설명은 다음과 같습니다.

  help : commands의 리스트를 제공한다.
  print[i] :  이 커맨드는 ASN i의 자율적 시스템의 라우팅 테이블을 출력한다. [i]는 int여야 한다. 올바른 인덱스를 입력해야한다.
  change [i] [j] [metric] : 이 커맨드는 endpoints가 i와 j인 특정한 링크의 metric을 바꾼다. 이 커맨드는 링크를 더하지 않는다. AS i와 j 사이에 링크가 존재해야한다.
  exit : 시뮬레이션을 끝낸다.
  timestamp : 최근 timestamp를 출력한다.

이 시뮬레이터에서는 링크 메트릭이 변경될 때 라우팅 테이블 초기화 및 라우팅 테이블 업데이트만 고려하고 구현하면 됩니다.
클래스 Auto_sys 및 해당 상속 클래스에는 update_rt_table() 및 send_packet_neighbor() 함수와 관련된 함수만 구현하면 됩니다.
update_rt_table()은 인접 AS에서 수신한 패킷을 사용하여 라우팅 테이블을 업데이트합니다.
update_rt_table()은 패킷을 neighbor로 보내야 하는지 여부를 반환합니다. send_packet_neighbor()는 패킷을 neighbor AS로 보냅니다.
인접 라우터의 라우팅 테이블에 직접 액세스하지 말고 패킷을 사용하여 업데이트해야 합니다.

시뮬레이터는 init_rt_table()을 사용하여 네트워크의 모든 AS의 라우팅 테이블을 초기화합니다.
그런 다음 send_packet_neighbor()가 패킷을 보냅니다. 시뮬레이터는 라우팅 테이블을 업데이트하기 위해 update_rt_table() - send_packet_neighbor()를 반복합니다.
시뮬레이터는 모든 AS가 패킷을 보낼 필요가 없으면 이 작업을 중지합니다.
링크 메트릭을 변경할 때 시뮬레이터의 프로세스는 라우팅 테이블의 초기화 프로세스와 유사합니다.
change_rt_table()을 사용하여 두 엔드포인트의 라우팅 테이블을 변경합니다. 그런 다음 send_packet_neighbor()는 패킷을 보냅니다.
시뮬레이터는 라우팅 테이블을 업데이트하기 위해 update_rt_table() - send_packet_neighbor()를 반복합니다.
이하 그림은 구현해야 할 시뮬레이터의 로직 플로우입니다.
![image](https://github.com/GeunSuYoon/data_commu_network_hw02/assets/146644182/063085a2-081f-47ec-9f3f-6dacc61b24fc)

### C. Detail in Routing Protocol
우리는 먼저 구현할 프로토콜에 공통점을 소개합니다.
3개의 라우팅 프로토콜은 동기식 업데이트를 사용합니다. 즉, 각 라우터는 자신의 라우팅 테이블을 동시에 업데이트합니다.
또한 패킷을 이웃 AS로 동시에 보냅니다.
라우팅 정보는 목적지의 IP 접두사, 넥스트 홉의 IP 주소, 목적지까지의 총 메트릭을 운반해야 합니다. 라우팅 프로토콜이 사용하는 패킷 헤더의 형식은 프로토콜 유형에 의존하지 않습니다.
패킷 헤더의 형식은 표 1과 같습니다.
![image](https://github.com/GeunSuYoon/data_commu_network_hw02/assets/146644182/17a87158-fa62-4b96-8432-abddd22db997)
  protocol segment는 우리에게 routing protocol의 타입을 보여준다. Disctance-vector : 0x80, Link-state routing protocol : 0x81, Path-vector routing protocol : 0x82
  Type segment는 routing protocol 내 packet의 타입을 보여준다. 이번 시뮬레이션에서 해당 값은 0xFF다.
  Length는 header와 body를 포함한 packet의 총 길이를 보여준다. Length의 단위는 byte다. A packet의 body가 없다면 length는 16일 것이다.
  IP address of Source and IP address of destination 은 각 router의 IPv4 address이다. 이 IPv4 address는 32bit으로 표현 가능하다.
  Timestamp는 업데이트 동기화를 돕기 위한 32-bit int이다.

이 packet header는 class packet에서 구현된다.

Routing table 내의 routing information은 the destination IP prefix, next hop IP address, total metric to destination으로 구성되는 것이 일반적이다.
각 routing algorithm에서 추가 데이터는 the routing information에 포함된다.
추가 데이터는 나중에 자세히 다룰 것이다.

#### Distance-Vector Routing Protocol
Distance-vector routing protocol에서 우리는 packets가 각 이웃 AS를 통과할 때 total metrics를 이용해 next hop을 업데이트한다.

Distance-vector routing algorithm의 addtional data는 metric_via_neighbor, 우리가 각 이웃 AS를 통과할 때 destination까지 모든 metric이다.
The additional data의 크기는 이웃 AS의 숫자와 같다.
Additional data의 인덱스는 forwarding table의 인덱스를 따른다.
초기 metric_via_neighbor의 값은 INF이다.

Distance-vector routing protocol을 위한 packet body의 segment는 Table 2와 같다.
![image](https://github.com/GeunSuYoon/data_commu_network_hw02/assets/146644182/245ba235-5440-44f4-82bd-5b9a517fb55f)
하나의 routing information을 위한 packet body의 segment는 12bytes로 이루어진다.
우리는 routing table 내의 모든 data를 위핸 이 segments를 보낸다.
만약 routing table에 routing information이 n IP개가 존재한다면, 우리는 12n bytes의 packet body를 보낼 것이다.

#### Link-State Routiong Protocol
Link-state routing protocol에서, 우리는 link의 information을 이용해 next hop을 update할 것이다.
먼저 인접한 matrix와 유사한 네트워크의 map을 만들 것이다.
그리고 Dijkstra's algorithm을 이용해 link를 update 할 것이다.

Link-state routing algorithm을 위한 추가 data는 ASN(autonomous system number)와 matric_to_AS다.
matric_to_AS는 target AS가 ASN i와 이 metric이 연결되어 있음을 보여준다.
만약 link가 존재하지 않는다면, 값은 INF일 것이다.
Self-looping link는 존재하지 않다.
AS의 인덱스는 ASN의 인덱스와 같다.

Link-state routing protocol을 위한 packet body의 segment는 Table 3에 나와있다.
![image](https://github.com/GeunSuYoon/data_commu_network_hw02/assets/146644182/962a09a9-49ed-4423-bf43-02b8c9e00ab8)
하나의 routing information을 위한 packet body segment는 4m + 12 bytes로 이뤄진다.(m은 autonomous systems의 숫자이다.)
Routers는 모든 data를 위한 segments를 routing table과 그들 자신에게 보낸다.
만약 routing table 내에 정해진 routing information이 n IP라면, 4(n+1)(m+1) bytes의 packet body를 보낸다.

init_rt_table()은 빈 함수이다.
update_rt_table()은 update_map()과 update_table_dijkstra()로 이루어져있다.
update_map()은 받은 packets를 이용해 the connection map을 update한다.
update_table_dijkstra()는 the next hop과 Dijkstra's algorithm을 이용해 total metric을 update한다.

Dijkstra's algorithm은 가장 짧은 길을 찾는다.
방문하지 않은 가장 짧은 노드를 찾는다.
그 후 그것을 계산하고 해당 노드가 존재하던 것 보다 짧으면 update한다.
모든 노드 중 가장 짧은 길을 찾을 동안 반복한다.

#### Path-Vector Routing Protocol
