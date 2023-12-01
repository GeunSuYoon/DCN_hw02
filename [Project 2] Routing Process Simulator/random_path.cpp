#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <vector>
#include <tuple>
#include <string>

typedef	std::tuple<int, int, int> rand_link;

int main(void)
{
	std::vector<rand_link>	links;
	rand_link				new_link;
	rand_link				tmp_link;
	std::srand(time(NULL));
	int	ASN_num = 6;	// change what you want.
	int link_num = 10;	// change what you want.
	int	max_cost = 20;	// change what you want.
	int	link_a, link_b, link_cost;
	int	same_counter = 0;
	while (links.size() < link_num)
	{
		same_counter = 0;
		link_a = std::rand() % ASN_num;
		link_b = std::rand() % ASN_num;
		while (link_a == link_b)
			link_b = std::rand() % ASN_num;
		link_cost = std::rand() % max_cost;
		while (!link_cost)
			link_cost = std::rand() % max_cost;
		for (int i = 0; i < links.size(); i++)
		{
			tmp_link = links[i];
			if ((std::get<0>(tmp_link) == link_a && std::get<1>(tmp_link) == link_b)
			|| (std::get<0>(tmp_link) == link_b && std::get<1>(tmp_link) == link_a))
			{
				same_counter = 1;
				break;
			}
		}
		if (same_counter == 1)
			continue;
		new_link = std::make_tuple(link_a, link_b, link_cost);
		links.push_back(new_link);
	}
	for (int i = 0; i < link_num; i++)
		std::cout << std::get<0>(links[i]) << ',' << std::get<1>(links[i]) << ',' << std::get<2>(links[i]) << ",\n";
	return (0);
}
