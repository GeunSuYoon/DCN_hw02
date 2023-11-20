route_sim : util.o auto_sys.o main.o network.o routing.o forward.o
	g++ -o route_sim util.o auto_sys.o main.o network.o routing.o forward.o
	rm *.o

util.o : util.cpp
	g++ -c -o util.o util.cpp -std=c++11

auto_sys.o : auto_sys.cpp
	g++ -c -o auto_sys.o auto_sys.cpp -std=c++11

network.o : network.cpp
	g++ -c -o network.o network.cpp -std=c++11

forward.o : forward.cpp
	g++ -c -o forward.o forward.cpp -std=c++11
  
routing.o : routing.cpp
	g++ -c -o routing.o routing.cpp -std=c++11
  
main.o : main.cpp
	g++ -c -o main.o main.cpp -std=c++11
  
clean : 
	rm *.o