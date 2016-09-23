#include <iostream>
#include <fstream>
#include "pubsub.hpp"



using namespace ps;


struct global_sub : Subscriber<std::string*>
{
	void execute(topic_raw_ptr topic, data_t data) override
	{
		counter++;

		if (counter == 10)
			std::cout << *data << "\n";

		if (counter % 100 == 0)
		{
			emit_signal(1);
		}
	}

	size_t counter{0};
};


struct publisher_t : Publisher<std::string*>
{
	using Publisher<std::string*>::Publisher;

	virtual void signal(int type_signal) override
	{
		std::cout << "custom_publisher (" << type_signal << ") " << c++ << "\n";
	}

private:
	size_t c{};
};

std::vector<std::string*> v_str;
std::string* push_data(const std::string& e)
{
	v_str.push_back(new std::string(e));
	return v_str[v_str.size() - 1];
}


int main()
{
	auto meteo_a = create_topic<std::string*>("meteo-a");
	auto meteo_a_station = create_publisher<std::string*, publisher_t>(meteo_a);

	auto meteo_b = create_topic<std::string*>("meteo-b");
	auto meteo_b_station = create_publisher<std::string*, publisher_t>(meteo_b);


	global_sub global;
	global.subscribe({meteo_a, meteo_b});

	global.run();

	std::thread th_meteo([&]{
		std::vector<std::string> cities{"Rome", "Florence", "Venice"};

		for (uint32_t i = 0; i < 1000000; i++)
		{
			const std::string tcelsius_a = std::to_string(24 + (i % 10));
			const std::string tcelsius_b = std::to_string(34 + (i % 19));
			meteo_a_station->produce(push_data(cities[i%3] + ", " + tcelsius_a));
			meteo_b_station->produce(push_data(cities[i%3] + ", " + tcelsius_b));
		}
	});

	th_meteo.join();
	global.stop();

	std::cout << "g_counter: " << global.counter << std::endl;

	return 0;
}
