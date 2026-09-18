#include <chrono>
#include <vector>
#include <future>
#include <iostream>
#include <string>

class Clicker
{
public:
	Clicker() :
		mStart(std::chrono::high_resolution_clock::now())
	{}
	double millisec() const
	{
		using namespace std;
		using namespace chrono;

		auto t = high_resolution_clock::now();
		return duration_cast< milliseconds >(t - mStart).count();
	}

private:
	std::chrono::time_point< std::chrono::steady_clock > mStart;
};

using data_t = std::vector< unsigned long long >;
using value_t = data_t::value_type;

size_t task(size_t size, value_t* values)
{
	value_t sum = 0;
	for (size_t i = 0; i < size; i++)
	{
		sum += values[i];
	}

	return sum;
}

int main(int argc, char* argv[])
{
	constexpr size_t size = 0x40000000;
	size_t threads = std::atoi(argv[1]);
	size_t sizeForThread = size / threads;

	data_t values(size, 1);
	value_t* data = values.data();

	std::vector< std::future< size_t > > futures;
	futures.reserve(threads);

	Clicker clicker;
	double init = 0, total = 0;
	init = clicker.millisec();
	for (size_t i = 0; i < threads; i++)
	{
		futures.push_back(std::async(std::launch::async, task, sizeForThread, data + i * sizeForThread));
	}

	size_t sum = 0;
	for (size_t i = 0; i < threads; i++)
	{
		sum += futures[i].get();
	}
	total = clicker.millisec();

	std::cout << total - init << '\n';
}
