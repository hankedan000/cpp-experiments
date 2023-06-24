#include <stdio.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

void
thread_fun(
	std::string &str)
{
	usleep(100000);// 100ms
	printf("str = %s\n", str.c_str());
}

int main()
{
	const unsigned int N_THREADS = 10;
	std::vector<std::thread> threads;
	threads.reserve(N_THREADS);

	printf("starting %d threads\n", N_THREADS);
	for (unsigned int i=0; i<N_THREADS; i++)
	{
		std::string t_str("Thread " + std::to_string(i));
		std::thread t(thread_fun, std::ref(t_str));
		threads.push_back(std::move(t));
	}

	printf("waiting for join...\n");
	for (auto &thread : threads)
	{
		thread.join();
	}

	return 0;
}