#include <cstring>
#include <getopt.h>
#include <iostream>
#include <sched.h>
#include <tqdm.h>
#include <unistd.h>// sleep

const unsigned int DEFAULT_ITER = 10000000;
const char *PROG_NAME = "error_handling";

struct prog_options
{
	int affinity = -1;// -1 for none; else [0 to <N_CORES-1>]
	int priority = 0; // 0 for SCHED_OTHER; [1 to 99] for SCHED_FIFO
	unsigned int num_iter = DEFAULT_ITER; // number of iterations
};

bool
bool_fun(int n)
{
	if (n % 7 == 0)
	{
		return false;
	}
	return true;
}

void
excp_fun(int n)
{
	if (n % 7 == 0)
	{
		throw std::runtime_error("");
	}
}

void
print_separator(
	const std::string &title)
{
	printf("===========================================\n");
	printf("%s\n", title.c_str());
}

void
do_experiment(
	const prog_options &opts)
{
	tqdm bar;
	unsigned int errors = 0;

	print_separator("bool_fun()");
	bar.reset();
	errors = 0;
	for (unsigned int i=0; i<opts.num_iter; i++)
	{
		bar.progress(i,opts.num_iter);
		if ( ! bool_fun(i))
		{
			errors++;
		}
	}
	bar.finish();
	printf("errors = %d\n", errors);

	print_separator("bool_fun() - no errors");
	bar.reset();
	errors = 0;
	for (unsigned int i=0; i<opts.num_iter; i++)
	{
		bar.progress(i,opts.num_iter);
		if ( ! bool_fun(1))
		{
			errors++;
		}
	}
	bar.finish();
	printf("errors = %d\n", errors);

	print_separator("bool_fun() - all errors");
	bar.reset();
	errors = 0;
	for (unsigned int i=0; i<opts.num_iter; i++)
	{
		bar.progress(i,opts.num_iter);
		if ( ! bool_fun(0))
		{
			errors++;
		}
	}
	bar.finish();
	printf("errors = %d\n", errors);

	print_separator("excp_fun()");
	bar.reset();
	errors = 0;
	for (unsigned int i=0; i<opts.num_iter; i++)
	{
		bar.progress(i,opts.num_iter);
		try
		{
			excp_fun(i);
		} catch (const std::exception &e) {
			errors++;
		}
	}
	bar.finish();
	printf("errors = %d\n", errors);

	print_separator("excp_fun() - no errors");
	bar.reset();
	errors = 0;
	for (unsigned int i=0; i<opts.num_iter; i++)
	{
		bar.progress(i,opts.num_iter);
		try
		{
			excp_fun(1);
		} catch (const std::exception &e) {
			errors++;
		}
	}
	bar.finish();
	printf("errors = %d\n", errors);

	print_separator("excp_fun() - all errors");
	bar.reset();
	errors = 0;
	for (unsigned int i=0; i<opts.num_iter; i++)
	{
		bar.progress(i,opts.num_iter);
		try
		{
			excp_fun(0);
		} catch (const std::exception &e) {
			errors++;
		}
	}
	bar.finish();
	printf("errors = %d\n", errors);
}

void
display_usage()
{
	printf("usage: %s [options]\n",PROG_NAME);
	printf(" -a,--affinity    : CPU affinity: -1 for none; else [0 to <N_CORES-1>]\n");
	printf(" -p,--priority    : scheding priority: 0 for SCHED_OTHER; [1 to 99] for SCHED_FIFO\n");
	printf("                    requires root permissions\n");
	printf(" -n,--num-iter    : number of iterations (default %d)\n",DEFAULT_ITER);
	printf(" -h,--help        : display this menu\n");
}

void
parse_args(
	int argc,
	char *argv[],
	prog_options &opts)
{
	static struct option long_options[] =
	{
		{"affinity"      , required_argument, 0                   , 'a'},
		{"priority"      , required_argument, 0                   , 'p'},
		{"num-iter"      , required_argument, 0                   , 'n'},
		{"help"          , no_argument      , 0                   , 'h'},
		{0, 0, 0, 0}
	};

	while (true)
	{
		int option_index = 0;
		int c = getopt_long(
			argc,
			argv,
			"a:p:n:h",
			long_options,
			&option_index);

		// detect the end of the options
		if (c == -1)
		{
			break;
		}

		switch (c)
		{
			case 0:
				// flag setting
				break;
			case 'a':
				opts.affinity = atoi(optarg);
				break;
			case 'p':
				opts.priority = atoi(optarg);
				break;
			case 'n':
				opts.num_iter = atoi(optarg);
				break;
			case 'h':
			case '?':
			default:
				display_usage();
				exit(0);
				break;
		}
	}
}

void
setup(
	const prog_options &opts)
{
	// setup realtime scheduling policy/priorities
	if (opts.priority > 0)
	{
		struct sched_param sched_p;
		sched_p.sched_priority = opts.priority;
		std::cout << "setting sched policy to SCHED_FIFO w/ sched_priority " << opts.priority << std::endl;
		if (sched_setscheduler(0,SCHED_FIFO,&sched_p) < 0)
		{
			if (errno == EPERM)
			{
				std::cerr << "unable to set scheduling policy to 'SCHED_FIFO'."
					" root permissions are required. defaulting to 'SCHED_OTHER' policy." << std::endl;
			} else {
				std::cerr << "uncaught sched_setscheduler() error!"
					" errno = " << errno << "(" << strerror(errno) << ")" << std::endl;
			}
		}
	}

	if (opts.affinity >= 0)
	{
		std::cout << "setting CPU affinity to core " << opts.affinity << std::endl;
		cpu_set_t cpu_set;
		CPU_ZERO(&cpu_set);
		CPU_SET(opts.affinity,&cpu_set);
		if (sched_setaffinity(0,sizeof(cpu_set),&cpu_set) < 0)
		{
			std::cerr << "uncaught sched_setaffinity() error!"
				" errno = " << errno << "(" << strerror(errno) << ")" << std::endl;
		}
	}
}

int main(int argc, char *argv[])
{
	prog_options opts;
	parse_args(argc,argv,opts);

	setup(opts);
	do_experiment(opts);

	return 0;
}