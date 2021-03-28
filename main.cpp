#include <iostream>
#include <fstream>
#include <condition_variable>
#include "pubsub.hpp"

using namespace ps;

std::mutex g_m;

struct global_sub : public Subscriber<std::string*>
{
	void execute(topic_raw_ptr topic, data_t data) override
	{

        ++counter;
        if (counter % 10 == 0) {
            std::lock_guard l{g_m};
            std::cout << m_name << " : " << *data << "\n";
		}

		if (counter % 1000 == 0)
		{
			emit_signal(1);
		}

	}

    void set_name(const std::string & name) { m_name = name; }

    ~global_sub()
    {
        unsubscribe();
        stop_wait();
    }

    size_t counter{0};
private:
    std::string m_name{"."};
};


struct publisher_t : public Publisher<std::string*>
{
	using Publisher<std::string*>::Publisher;

	void set_name(std::string n) { name = std::move(n); }
	virtual void signal(int type_signal) override
	{
		std::cout << "custom_publisher (name: " << name << ", signal: " << type_signal << ") " << c++ << "\n";
	}

private:
	std::string name{"."};
	size_t c{};
    std::mutex m_lock;
};

constexpr int max_num = 10'000;
std::vector<std::string *> v_str(max_num, nullptr);

std::string* push_data(const std::string& e)
{
    std::lock_guard l{g_m};
    static int index = 0;
    if(index >= max_num) {
        index = 0;
    }

    delete v_str[index];
    v_str[index] = new std::string(e);
    return v_str[index++];
}


int main()
{
	auto meteo_a = create_topic<std::string*>("meteo-a");
	auto meteo_a_station = create_publisher<std::string*, publisher_t>(meteo_a);
	meteo_a_station->set_name("A");

//	auto meteo_b = create_topic<std::string*>("meteo-b");
//	auto meteo_b_station = create_publisher<std::string*, publisher_t>(meteo_b);
//	meteo_b_station->set_name("B");

    auto meteo_station = create_publisher<std::string*, publisher_t>(std::vector<topic_ptr_t<std::string*>>{meteo_a/*, meteo_b*/});

	global_sub global;
    global.set_name("meteo_a, meteo_b");
    global.subscribe({meteo_a/*, meteo_b*/});

	global.counter = 0;
	global.run();



    std::thread th_meteo([&]{
		std::vector<std::string> cities{"Rome", "Florence", "Venice"};
        static size_t i = 0;
        while(1)
		{
			const std::string tcelsius_a = std::to_string(24 + (i % 10));
            //const std::string tcelsius_b = std::to_string(34 + (i % 19));
            meteo_station->produce(push_data(cities[i%3] + ", " + tcelsius_a));
            ++i;
			//meteo_b_station->produce(push_data(cities[i%3] + ", " + tcelsius_b));
		}
	});

	th_meteo.join();
	global.stop_wait();

	std::cout << "g_counter: " << global.counter << std::endl;

	return 0;
}
