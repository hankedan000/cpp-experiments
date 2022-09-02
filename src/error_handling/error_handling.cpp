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
	int experiment = -1;
};

void
print_l_separator(
	const std::string &title)
{
	printf("===========================================\n");
	printf("%s\n", title.c_str());
	printf("===========================================\n");
}
void
print_s_separator(
	const std::string &title)
{
	printf("== %s\n", title.c_str());
}

bool
ret_fun(int n)
{
	if (n % 7 == 0)
	{
		return false;
	}
	return true;
}

void
exc_fun(int n)
{
	if (n % 7 == 0)
	{
		throw std::exception();
	}
}

void
do_experiment1(
	const prog_options &opts)
{
	print_l_separator("EXPERIMENT 1");
	tqdm bar;
	unsigned int errors = 0;

	print_s_separator("bool return - mixed errors");
	bar.reset();
	errors = 0;
	for (unsigned int i=0; i<opts.num_iter; i++)
	{
		bar.progress(i,opts.num_iter);
		if ( ! ret_fun(i))
		{
			errors++;
		}
	}
	bar.finish();
	printf("errors = %d\n", errors);

	print_s_separator("bool return - no errors");
	bar.reset();
	errors = 0;
	for (unsigned int i=0; i<opts.num_iter; i++)
	{
		bar.progress(i,opts.num_iter);
		if ( ! ret_fun(1))
		{
			errors++;
		}
	}
	bar.finish();
	printf("errors = %d\n", errors);

	print_s_separator("bool return - all errors");
	bar.reset();
	errors = 0;
	for (unsigned int i=0; i<opts.num_iter; i++)
	{
		bar.progress(i,opts.num_iter);
		if ( ! ret_fun(0))
		{
			errors++;
		}
	}
	bar.finish();
	printf("errors = %d\n", errors);

	print_s_separator("exception - mixed errors");
	bar.reset();
	errors = 0;
	for (unsigned int i=0; i<opts.num_iter; i++)
	{
		bar.progress(i,opts.num_iter);
		try
		{
			exc_fun(i);
		} catch (const std::exception &e) {
			errors++;
		}
	}
	bar.finish();
	printf("errors = %d\n", errors);

	print_s_separator("exception - no errors (try-catch inside loop)");
	bar.reset();
	errors = 0;
	for (unsigned int i=0; i<opts.num_iter; i++)
	{
		bar.progress(i,opts.num_iter);
		try
		{
			exc_fun(1);
		} catch (const std::exception &e) {
			errors++;
		}
	}
	bar.finish();
	printf("errors = %d\n", errors);

	print_s_separator("exception - no errors (try-catch outside loop)");
	bar.reset();
	errors = 0;
	try
	{
		for (unsigned int i=0; i<opts.num_iter; i++)
		{
			bar.progress(i,opts.num_iter);
			exc_fun(1);
		}
	} catch (const std::exception &e) {
		errors++;
	}
	bar.finish();
	printf("errors = %d\n", errors);

	print_s_separator("exception - all errors");
	bar.reset();
	errors = 0;
	for (unsigned int i=0; i<opts.num_iter; i++)
	{
		bar.progress(i,opts.num_iter);
		try
		{
			exc_fun(0);
		} catch (const std::exception &e) {
			errors++;
		}
	}
	bar.finish();
	printf("errors = %d\n", errors);
}

// return codes
#define OTHER   0
#define MULT7   1
#define MULT42  2
#define MULT100 3

int
ret_fun2(int n)
{
	if (n % 100 == 0)
	{
		return MULT100;
	} else if (n % 42 == 0) {
		return MULT42;
	} else if (n % 7 == 0) {
		return MULT7;
	}
	return OTHER;
}

// possible exceptions
class Mult7Exception : public std::exception {};
class Mult42Exception : public std::exception {};
class Mult100Exception : public std::exception {};

void
exc_fun2(int n)
{
	if (n % 100 == 0)
	{
		throw Mult100Exception();
	} else if (n % 42 == 0) {
		throw Mult42Exception();
	} else if (n % 7 == 0) {
		throw Mult7Exception();
	}
}

void
do_experiment2(
	const prog_options &opts)
{
	print_l_separator("EXPERIMENT 2");
	tqdm bar;
	unsigned int mult7 = 0;
	unsigned int mult42 = 0;
	unsigned int mult100 = 0;

	print_s_separator("return code - switch case");
	bar.reset();
	mult7 = 0; mult42 = 0; mult100 = 0;
	for (unsigned int i=0; i<opts.num_iter; i++)
	{
		bar.progress(i,opts.num_iter);
		switch (ret_fun2(i))
		{
			case MULT7:
				mult7++;
				break;
			case MULT42:
				mult42++;
				break;
			case MULT100:
				mult100++;
				break;
		}
	}
	bar.finish();
	printf("mult7 = %d; mult42 = %d; mult100 = %d\n", mult7, mult42, mult100);

	print_s_separator("return code - if else");
	bar.reset();
	mult7 = 0; mult42 = 0; mult100 = 0;
	for (unsigned int i=0; i<opts.num_iter; i++)
	{
		bar.progress(i,opts.num_iter);
		int ret = ret_fun2(i);
		if (ret == MULT7)
		{
			mult7++;
		} else if (ret == MULT42) {
			mult42++;
		} else if (ret == MULT100) {
			mult100++;
		}
	}
	bar.finish();
	printf("mult7 = %d; mult42 = %d; mult100 = %d\n", mult7, mult42, mult100);

	print_s_separator("exception");
	bar.reset();
	mult7 = 0; mult42 = 0; mult100 = 0;
	for (unsigned int i=0; i<opts.num_iter; i++)
	{
		bar.progress(i,opts.num_iter);
		try
		{
			exc_fun2(i);
		} catch (const Mult7Exception &e) {
			mult7++;
		} catch (const Mult42Exception &e) {
			mult42++;
		} catch (const Mult100Exception &e) {
			mult100++;
		}
	}
	bar.finish();
	printf("mult7 = %d; mult42 = %d; mult100 = %d\n", mult7, mult42, mult100);
}

#define OTHER    0
#define EQUAL7   1
#define EQUAL42  2
#define EQUAL100 3

int
ret_fun3(int n)
{
	if (n == 7)
	{
		return EQUAL7;
	} else if (n == 42) {
		return EQUAL42;
	} else if (n == 100) {
		return EQUAL100;
	}
	return OTHER;
}

// possible exceptions
class Equal7Exception : public std::exception {};
class Equal42Exception : public std::exception {};
class Equal100Exception : public std::exception {};

void
exc_fun3(int n)
{
	if (n == 7)
	{
		throw Equal7Exception();
	} else if (n == 42) {
		throw Equal42Exception();
	} else if (n == 100) {
		throw Equal100Exception();
	}
}

void
do_experiment3(
	const prog_options &opts)
{
	print_l_separator("EXPERIMENT 3");
	tqdm bar;
	unsigned int equal7 = 0;
	unsigned int equal42 = 0;
	unsigned int equal100 = 0;

	print_s_separator("return code - switch case");
	bar.reset();
	equal7 = 0; equal42 = 0; equal100 = 0;
	for (unsigned int i=0; i<opts.num_iter; i++)
	{
		bar.progress(i,opts.num_iter);
		switch (ret_fun3(i))
		{
			case EQUAL7:
				equal7++;
				break;
			case EQUAL42:
				equal42++;
				break;
			case EQUAL100:
				equal100++;
				break;
		}
	}
	bar.finish();
	printf("equal7 = %d; equal42 = %d; equal100 = %d\n", equal7, equal42, equal100);

	print_s_separator("return code - if else");
	bar.reset();
	equal7 = 0; equal42 = 0; equal100 = 0;
	for (unsigned int i=0; i<opts.num_iter; i++)
	{
		bar.progress(i,opts.num_iter);
		int ret = ret_fun3(i);
		if (ret == EQUAL7)
		{
			equal7++;
		} else if (ret == EQUAL42) {
			equal42++;
		} else if (ret == EQUAL100) {
			equal100++;
		}
	}
	bar.finish();
	printf("equal7 = %d; equal42 = %d; equal100 = %d\n", equal7, equal42, equal100);

	print_s_separator("exception");
	bar.reset();
	equal7 = 0; equal42 = 0; equal100 = 0;
	for (unsigned int i=0; i<opts.num_iter; i++)
	{
		bar.progress(i,opts.num_iter);
		try
		{
			exc_fun3(i);
		} catch (const Equal7Exception &e) {
			equal7++;
		} catch (const Equal42Exception &e) {
			equal42++;
		} catch (const Equal100Exception &e) {
			equal100++;
		}
	}
	bar.finish();
	printf("equal7 = %d; equal42 = %d; equal100 = %d\n", equal7, equal42, equal100);
}

void
display_usage()
{
	printf("usage: %s [options]\n",PROG_NAME);
	printf(" -a,--affinity    : CPU affinity: -1 for none; else [0 to <N_CORES-1>]\n");
	printf(" -p,--priority    : scheding priority: 0 for SCHED_OTHER; [1 to 99] for SCHED_FIFO\n");
	printf("                    requires root permissions\n");
	printf(" -n,--num-iter    : number of iterations (default %d)\n",DEFAULT_ITER);
	printf(" -e,--experiment  : only run specific experiment (default -1 runs all)\n");
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
		{"experiment"    , required_argument, 0                   , 'e'},
		{"help"          , no_argument      , 0                   , 'h'},
		{0, 0, 0, 0}
	};

	while (true)
	{
		int option_index = 0;
		int c = getopt_long(
			argc,
			argv,
			"a:p:n:e:h",
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
			case 'e':
				opts.experiment = atoi(optarg);
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
	if (opts.experiment == -1 || opts.experiment == 1)
	{
		do_experiment1(opts);
	}
	if (opts.experiment == -1 || opts.experiment == 2)
	{
		do_experiment2(opts);
	}
	if (opts.experiment == -1 || opts.experiment == 3)
	{
		do_experiment3(opts);
	}

	return 0;
}