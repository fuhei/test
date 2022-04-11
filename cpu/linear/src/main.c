/**
 * @file main.c
 * @author Rong Tao <rongtao@cestc.cn>
 * @brief Test CPU cores Linearity
 * @version 0.1
 * @date 2022-04-08
 *
 * @copyright Copyright (c) 2022
 *
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <getopt.h>
#include <pthread.h>
#include <numa.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#include "utils.h"

struct pthread_args {
    int cpu;
    int numaid;
    enum test_type test_type; // 和 struct args_opt.test_type 值相同
    unsigned long test_loop; // 和 struct args_opt.test_loop 值相同
    struct timespec start, end;
};

struct args_opt {
    int min_cpu;
    int max_cpu;
    int interval_cpu;
    int nr_thread; // 用 max_cpu, min_cpu, interval_cpu 计算得到
    pthread_t *threadids;
    struct pthread_args **threads_args;
    int cacheline_size;
    enum test_type test_type;
    unsigned long test_loop;
};


/**
 * @brief 默认参数
 *
 */
struct args_opt default_args = {
    .min_cpu = 0,
    .max_cpu = 0,
    .interval_cpu = 1,
    .threadids = NULL,
    .threads_args = NULL,
    .cacheline_size = 64,
    .test_type = TEST_ARITHMETIC,
    .test_loop = 10000,
};

struct numa_node_info {
    int numaid;
    struct bitmask cpumask;
};

struct system_info {
    int nr_numa_node;
    int num_possible_cpus;
    struct numa_node_info *numainfos;
};


static struct system_info systeminfo;

/**
 * @brief 获取 系统信息
 *
 */
void get_system_info(void)
{
    int i;

    /* NUMA 信息 */
    systeminfo.nr_numa_node = numa_max_node() + 1;
    systeminfo.numainfos = malloc(sizeof(struct numa_node_info)*systeminfo.nr_numa_node);
    memset(systeminfo.numainfos, 0, sizeof(struct numa_node_info)*systeminfo.nr_numa_node);

    systeminfo.num_possible_cpus = numa_num_possible_cpus();

    for (i = 0; i < systeminfo.nr_numa_node; i++) {
        systeminfo.numainfos[i].numaid = i;
        numa_node_to_cpus(i, &systeminfo.numainfos[i].cpumask);
    }

    //fprintf(stdout, "Number of NUMA NODE %d\n", systeminfo.nr_numa_node);
    //fprintf(stdout, "Max Number of CPU %d\n", systeminfo.num_possible_cpus);

}

/**
 * @brief 帮助信息
 */
static void usage(int argc, char *argv[])
{
    fprintf(stderr, "Usage: %s\n", argv[0]);
    fprintf(stderr, " -h, --help        Show help info.\n");
    fprintf(stderr, " -i, --min-cpu     Min cpu number to test, default 0.\n");
    fprintf(stderr, " -a, --max-cpu     Max cpu number to test, default 0.\n");
    fprintf(stderr, " -I, --cpu-interval Interval of CPUs, default 1.\n"\
                    "                   Such as \n"\
                    "                       --min-cpu = 0\n"\
                    "                       --max-cpu = 10,\n"\
                    "                       --cpu-interval = 3\n"\
                    "                    the cpu list is 0, 3, 6, 9.\n");
    fprintf(stderr, " -t, --test        Test type, default: arithmetic.\n"\
                    "                    [arithmetic|n-queen|pi]\n");
    fprintf(stderr, " -n, --test-loop   Max test loop, default 10000.\n");
    fprintf(stderr, " -c, --cache-line  Manual set Cache line size, default 64.\n");
    exit(1);
}

/**
 * @brief 解析命令行参数
 */
void parse_args(int argc, char *argv[])
{
    int i;
    static struct option options[] = {
        {"help", no_argument, 0, 'h'},
        {"min-cpu", required_argument, 0, 'i'},
        {"max-cpu", required_argument, 0, 'a'},
        {"cpu-interval", required_argument, 0, 'I'},
        {"test", required_argument, 0, 't'},
        {"test-loop", required_argument, 0, 'n'},
        {"cache-line", required_argument, 0, 'c'},
    };
    struct args_opt *args = &default_args;

    while (1) {
        int option_index = 0;
        int cmd = getopt_long(argc, argv, "i:a:I:c:t:n:h", options, &option_index);
        if (cmd == -1) {
            break;
        }
        switch (cmd) {
        case 'h':
            usage(argc, argv);
            break;
        case 'i':
            args->min_cpu = atoi(optarg);
            break;
        case 'a':
            args->max_cpu = atoi(optarg);
            break;
        case 'I':
            args->interval_cpu = atoi(optarg);
            break;
        case 'c':
            args->cacheline_size = atoi(optarg);
            break;
        case 't':
            if (!strcmp(optarg, "arithmetic")) {
                args->test_type = TEST_ARITHMETIC;
            } else if (!strcmp(optarg, "n-queen")) {
                args->test_type = TEST_N_QUEEN;
                fprintf(stderr, "Not support n-queen yet.\n");
                exit(1);
            } else if (!strcmp(optarg, "pi")) {
                args->test_type = TEST_PI;
            } else {
                fprintf(stderr, "Specify -t, --type arg invalid.\n");
                usage(argc, argv);
            }
            break;
        case 'n':
            args->test_loop = strtoul(optarg, NULL, 0);
            break;
        case '?':
            abort();
            break;
        default:
            fprintf(stderr, "Unknown %s, cmd = %d(%c)\n", optarg, cmd, cmd);
            abort();
            break;
        }
    }

    if (args->min_cpu > args->max_cpu) {
        fprintf(stderr, "Error --min-cpu and --max-cpu parameters, --min-cpu must <= --max-cpu.\n");
        exit(1);
    }

    if (args->max_cpu > systeminfo.num_possible_cpus) {
        fprintf(stderr, "Error Number of possible cpus is %d.\n", systeminfo.num_possible_cpus);
        exit(1);
    }

    for (i = args->min_cpu; i <= args->max_cpu; i++) {
        if (numa_node_of_cpu(i) < 0) {
            fprintf(stderr, "CPU(%d) not belongs to any NUMA node or not exist.\n"\
                " --max-cpu param invalid, use 'nproc' or 'numactl --hardware' check CPU id.\n", i);
            exit(1);
        }
    }

    args->nr_thread = (args->max_cpu - args->min_cpu + (args->interval_cpu ))/args->interval_cpu;

    //fprintf(stdout, "N thread = %d\n", args->nr_thread);
    //exit(1);
}

/**
 * @brief 分析结果
 *
 */
void analyze_result(struct args_opt *args)
{
    int i;
    unsigned long total_nsec;

    int Ncpu = args->nr_thread;

    fprintf(stdout, "\033[1;7m%-8s %-8s %-10s %-10s %-10s\033[m\n",
            "CPU", "NUMA", "SEC", "MSEC", "USEC");
    for (i = 0; i < Ncpu; i++) {
        struct pthread_args *thread_arg = args->threads_args[i];
        struct timespec *start = &thread_arg->start;
        struct timespec *end = &thread_arg->end;

        if (end->tv_nsec < start->tv_nsec) {
            end->tv_nsec += 1000000000L;
            end->tv_sec--;
        }
        unsigned long nsec = (end->tv_sec - start->tv_sec)*1000000000UL
                         + (end->tv_nsec - start->tv_nsec);

        fprintf(stdout, "%-8d %-8d %-10ld %-10ld %-10ld\n",
                    thread_arg->cpu, thread_arg->numaid,
                    nsec/1000000, nsec/1000, nsec);
        total_nsec += nsec;
    }

    fprintf(stdout, "\033[32m%-8s %-8s %-10ld %-10ld %-10ld\033[m\n", "Total", "",
            total_nsec/1000000, total_nsec/1000, total_nsec);
    fprintf(stdout, "\033[33m%-8s %-8s %-10ld %-10ld %-10ld\033[m\n", "Average", "",
            total_nsec/1000000/Ncpu, total_nsec/1000/Ncpu, total_nsec/Ncpu);
    fprintf(stdout, "Total CPU core %d\n", Ncpu);
}

/**
 * @brief 线程回调函数
 *
 */
static void *thread_routine(void *arg)
{
    int status = 0;
    cpu_set_t cpuset;

    struct pthread_args *my_arg = arg;

    CPU_ZERO(&cpuset);
    CPU_SET(my_arg->cpu, &cpuset);

    /* 绑核 */
    if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0) {
        perror("pthread_setaffinity_np");
    }

    /* 记录起始时间 */
    clock_gettime(CLOCK_MONOTONIC, &my_arg->start);

    test_execute(my_arg->test_type, my_arg->test_loop);

    /* 记录结束时间 */
    clock_gettime(CLOCK_MONOTONIC, &my_arg->end);

    pthread_exit(&status);
    return NULL;
}

/**
 * @brief 根据 命令行参数启动 线程
 *
 * @param args
 */
void create_threads(struct args_opt *args)
{
    int i, j;

    int nr_thread = args->nr_thread;

    args->threadids = malloc(sizeof(pthread_t)*nr_thread);
    assert(args->threadids && "Malloc Fatal.");

    args->threads_args = malloc(sizeof(struct pthread_args *) * nr_thread);
    assert(args->threads_args && "Malloc Fatal.");

    for (j = 0, i = args->min_cpu; i <= args->max_cpu; j++, i += args->interval_cpu) {
        int cpu = i;
        int idx = j;
        int cache_line_size = args->cacheline_size;

        args->threads_args[idx] = aligned_alloc(cache_line_size, sizeof(struct pthread_args));
        assert(args->threads_args[idx] && "Malloc Fatal.");

        struct pthread_args *thread_arg = args->threads_args[idx];

        thread_arg->cpu = cpu;
        thread_arg->numaid = numa_node_of_cpu(cpu);
        thread_arg->test_type = args->test_type;
        thread_arg->test_loop = args->test_loop;

        pthread_create(&args->threadids[idx], NULL, thread_routine, thread_arg);
    }

    for (i = 0; i < nr_thread; i++) {
        pthread_join(args->threadids[i], NULL);
    }

    analyze_result(args);

    for (i = 0; i < nr_thread; i++) {
        free(args->threads_args[i]);
    }
    free(args->threads_args);
    free(args->threadids);
}

void sig_handler(int signum)
{
    switch (signum) {
    case SIGINT:
        fprintf(stderr, "Catch ctrl-C, k, bye.\n");
        exit(1);
        break;
    default:
        break;
    }
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sig_handler);

    get_system_info();

    parse_args(argc, argv);

    create_threads(&default_args);
    return 0;
}