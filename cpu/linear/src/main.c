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
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#include "utils.h"

/**
 * @brief 默认参数
 *
 */
struct args_opt default_args = {
    .run_on_numa = -1,
    .min_cpu = 0,
    .max_cpu = 0,
    .interval_cpu = 1,
    .threadids = NULL,
    .threads_args = NULL,
    .cacheline_size = 64,
    .test_type = TEST_ARITHMETIC,
    .custom_test_loop = false,
    .test_loop = TEST_LOOP,
};


static struct system_info systeminfo;

/**
 * @brief 获取 系统信息
 *
 */
void get_system_info(void)
{
    int i;
    struct cache_info *cache = &systeminfo.cache_info;

    /* NUMA 信息 */
    systeminfo.nr_numa_node = numa_max_node() + 1;
    systeminfo.numainfos = malloc(sizeof(struct numa_node_info)*systeminfo.nr_numa_node);
    memset(systeminfo.numainfos, 0, sizeof(struct numa_node_info)*systeminfo.nr_numa_node);

    systeminfo.num_possible_cpus = numa_num_possible_cpus();

    for (i = 0; i < systeminfo.nr_numa_node; i++) {
        systeminfo.numainfos[i].numaid = i;
        numa_node_to_cpus(i, &systeminfo.numainfos[i].cpumask);
    }

#define READ_CACHE(to, name) \
    cache->to.assoc = sysconf(_SC_##name##_ASSOC);    \
    cache->to.size = sysconf(_SC_##name##_SIZE);  \
    cache->to.linesize = sysconf(_SC_##name##_LINESIZE);

    READ_CACHE(l1d, LEVEL1_DCACHE);
    READ_CACHE(l1i, LEVEL1_ICACHE);
    READ_CACHE(l2, LEVEL2_CACHE);
    READ_CACHE(l3, LEVEL3_CACHE);
    READ_CACHE(l4, LEVEL4_CACHE);

#undef READ_CACHE

    if (cache->l2.linesize >= 0) {
        systeminfo.nr_cache_level = 2;
        cache->llc = &cache->l2;
    }
    if (cache->l3.linesize >= 0) {
        systeminfo.nr_cache_level = 3;
        cache->llc = &cache->l3;
    }
    if (cache->l4.linesize >= 0) {
        systeminfo.nr_cache_level = 4;
        cache->llc = &cache->l4;
    }
}

void show_system_info(void)
{
    struct cache_info *cache = &systeminfo.cache_info;

    fprintf(stdout, "Number of NUMA NODE %d\n", systeminfo.nr_numa_node);
    fprintf(stdout, "Max Number of CPU %d\n", systeminfo.num_possible_cpus);

    if (cache->l2.linesize >= 0) {
        systeminfo.nr_cache_level = 2;
        cache->llc = &cache->l2;
    }
    if (cache->l3.linesize >= 0) {
        systeminfo.nr_cache_level = 3;
        cache->llc = &cache->l3;
    }
    if (cache->l4.linesize >= 0) {
        systeminfo.nr_cache_level = 4;
        cache->llc = &cache->l4;
    }

    fprintf(stdout, "Cache info may not accurate, use command:\n"\
            "   $ lstopo-no-graphics --no-io --no-legend --of txt\n");
    fprintf(stdout, "\033[32m%-8s %-8s %-8s %-8s\033[m\n", \
                    "Cache", "Assoc", "Size(KB)", "LineSize");
#define PRINT_CACHE(l) \
    if (cache->l.linesize > 0) \
    fprintf(stdout, "%-8s %-8d %-8d %-8d %s\n", #l, \
        cache->l.assoc, cache->l.size / 1024, cache->l.linesize, \
        cache->llc==&cache->l?"\033[31m(LLC)\033[m":"");
    PRINT_CACHE(l1d);
    PRINT_CACHE(l1i);
    PRINT_CACHE(l2);
    PRINT_CACHE(l3);
    PRINT_CACHE(l4);
#undef PRINT_CACHE
}

/**
 * @brief 帮助信息
 */
static void usage(int argc, char *argv[])
{
    fprintf(stderr, "Usage: %s\n", argv[0]);
    fprintf(stderr, " -h, --help        Show help info.\n");
    fprintf(stderr, " -N, --numa        Use CPU on a specific NUMA, default: no specific.\n");
    fprintf(stderr, " -i, --min-cpu     Min cpu number to test, default 0.\n");
    fprintf(stderr, " -a, --max-cpu     Max cpu number to test, default 0.\n");
    fprintf(stderr, " -I, --cpu-interval Interval of CPUs, default 1.\n"\
                    "                   Such as \n"\
                    "                       --min-cpu = 0\n"\
                    "                       --max-cpu = 10,\n"\
                    "                       --cpu-interval = 3\n"\
                    "                    the cpu list is 0, 3, 6, 9.\n");
    fprintf(stderr, " -t, --test        Test type, default: arithmetic.\n"\
                    "                    [arithmetic|n-queen|pi|qsort]\n"\
                    "                    `arithmetic,integer,add` to test integer addition\n"\
                    "                    `arithmetic,integer,add` to test integer subtraction\n"\
                    "                    `arithmetic,integer,add` to test integer multiplication\n"\
                    "                    `arithmetic,integer,add` to test integer division\n"\
                    "                    `arithmetic,float,add` to test float addition\n"\
                    "                    `arithmetic,float,add` to test float subtraction\n"\
                    "                    `arithmetic,float,add` to test float multiplication\n"\
                    "                    `arithmetic,float,add` to test float division\n");
    fprintf(stderr, " -n, --test-loop   Max test loop, default %ld.\n"\
                    "                    `arithmetic default loop is %ld\n"\
                    "                    `pi default loop(accurate) is %ld\n"\
                    "                    `qsort loop means elements of array, default %ld.\n",
                        default_args.test_loop,
                        TEST_ARITHMETIC_LOOP * default_args.test_loop,
                        TEST_PI_LOOP * default_args.test_loop,
                        TEST_QSORT_NR_ELEMENTS * default_args.test_loop);
    fprintf(stderr, " -c, --cache-line  Manual set Cache line size, default 64.\n"\
                    "                   Use `lstopo-no-graphics --no-io --no-legend --of txt` show cache detail.\n");
    fprintf(stderr, "\n");
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
        {"numa", required_argument, 0, 'N'},
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
        int cmd = getopt_long(argc, argv, "N:i:a:I:c:t:n:h", options, &option_index);
        if (cmd == -1) {
            break;
        }
        switch (cmd) {
        case 'h':
            usage(argc, argv);
            break;
        case 'N':
            args->run_on_numa = atoi(optarg);
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
            if (!strncmp(optarg, "arithmetic", 10)) {
                /**
                 * 整形运算
                 */
                if (strstr(optarg, "integer")) {
                    if (strstr(optarg, "add")) {
                        args->test_type = TEST_ARITHMETIC_INT_ADD;
                    } else if (strstr(optarg, "sub")) {
                        args->test_type = TEST_ARITHMETIC_INT_SUB;
                    } else if (strstr(optarg, "mul")) {
                        args->test_type = TEST_ARITHMETIC_INT_MUL;
                    } else if (strstr(optarg, "div")) {
                        args->test_type = TEST_ARITHMETIC_INT_DIV;
                    } else {
                        fprintf(stderr, "Must specify add,sub,mul,div.\n");
                    }
                /**
                 * 浮点运算
                 */
                } else if (strstr(optarg, "float")) {
                    if (strstr(optarg, "add")) {
                        args->test_type = TEST_ARITHMETIC_FLOAT_ADD;
                    } else if (strstr(optarg, "sub")) {
                        args->test_type = TEST_ARITHMETIC_FLOAT_SUB;
                    } else if (strstr(optarg, "mul")) {
                        args->test_type = TEST_ARITHMETIC_FLOAT_MUL;
                    } else if (strstr(optarg, "div")) {
                        args->test_type = TEST_ARITHMETIC_FLOAT_DIV;
                    } else {
                        fprintf(stderr, "Must specify add,sub,mul,div.\n");
                        usage(argc, argv);
                    }
                } else if (strlen(optarg) > strlen("arithmetic")) {
                    fprintf(stderr, "Not support %s.\n", optarg);
                    exit(1);
                } else {
                    args->test_type = TEST_ARITHMETIC;
                }
            } else if (!strcmp(optarg, "n-queen")) {
                args->test_type = TEST_N_QUEEN;
                fprintf(stderr, "Not support n-queen yet.\n");
                exit(1);
            } else if (!strcmp(optarg, "pi")) {
                args->test_type = TEST_PI;
            } else if (!strcmp(optarg, "qsort")) {
                args->test_type = TEST_SORT_QSORT;
                if (!args->custom_test_loop) {
                    args->test_loop *= TEST_QSORT_NR_ELEMENTS;
                }
            } else {
                fprintf(stderr, "Specify -t, --type arg invalid.\n");
                usage(argc, argv);
            }
            break;
        case 'n':
            args->test_loop = strtoul(optarg, NULL, 0);
            args->custom_test_loop = true;
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

    if (args->run_on_numa >= 0) {
        if (systeminfo.nr_numa_node < args->run_on_numa + 1) {
            fprintf(stderr, "Error -N, --numa(%d) must <= `number of NUMA NODE -1`(%d), "\
                        "check with numactl --hardware.\n",
                        args->run_on_numa, systeminfo.nr_numa_node-1);
            exit(1);
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

unsigned long timespec_nsec_interval(struct timespec start, struct timespec end)
{
    unsigned long nsec = 0;

    if (end.tv_nsec < start.tv_nsec) {
        end.tv_nsec += 1000000000L;
        end.tv_sec--;
    }
    nsec = (end.tv_sec - start.tv_sec)*1000000000UL
                        + (end.tv_nsec - start.tv_nsec);
    return nsec;
}

/**
 * @brief 分析结果
 *
 */
void analyze_result(struct args_opt *args)
{
    int i;
    unsigned long total_nsec, total_init, total_invoke, total_release;

    int Ncpu = args->nr_thread;

    total_nsec = total_init = total_invoke = total_release = 0;

    fprintf(stdout, "\033[1;7m%-8s %-8s %-10s %-10s %-10s\033[m "\
                    "\033[1;7m%-10s %-10s %-10s\033[m\n",
            "CPU", "NUMA", "SEC", "MSEC", "USEC", "Init(us)", "Invoke(us)", "Release(us)");
    for (i = 0; i < Ncpu; i++) {
        struct pthread_args *thread_arg = args->threads_args[i];

        unsigned long nsec = timespec_nsec_interval(thread_arg->start, thread_arg->end);
        unsigned long nsec_init = timespec_nsec_interval(thread_arg->start_init,
                        thread_arg->end_init);
        unsigned long nsec_invoke = timespec_nsec_interval(thread_arg->start_invoke,
                        thread_arg->end_invoke);
        unsigned long nsec_release = timespec_nsec_interval(thread_arg->start_release,
                        thread_arg->end_release);

        fprintf(stdout, "%-8d %-8d %-10ld %-10ld %-10ld %-10ld %-10ld %-10ld\n",
                    thread_arg->cpu, thread_arg->numaid,
                    nsec/1000000000, nsec/1000000, nsec/1000,
                    nsec_init/1000, nsec_invoke/1000, nsec_release/1000);
        total_nsec += nsec;
        total_init += nsec_init;
        total_invoke += nsec_invoke;
        total_release += nsec_release;
    }

    fprintf(stdout, "\033[32m%-8s %-8s %-10ld %-10ld %-10ld %-10ld %-10ld %-10ld\033[m\n", \
            "Total", "",
            total_nsec/1000000000, total_nsec/1000000, total_nsec/1000,
            total_init/1000, total_invoke/1000, total_release/1000);
    fprintf(stdout, "\033[33m%-8s %-8s %-10ld %-10ld %-10ld %-10ld %-10ld %-10ld\033[m\n", \
            "Average", "",
            total_nsec/1000000000/Ncpu, total_nsec/1000000/Ncpu, total_nsec/1000/Ncpu,
            total_init/1000/Ncpu, total_invoke/1000/Ncpu, total_release/1000/Ncpu);
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

    test_execute(my_arg);

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

    /* 首先检查 NUMA */
    if (args->run_on_numa >= 0) {
        for (j = 0, i = args->min_cpu; i <= args->max_cpu; j++, i += args->interval_cpu) {
            int cpu = i;
            if (numa_node_of_cpu(cpu) != args->run_on_numa) {
                fprintf(stderr, "CPU%d is not on NUMA %d\n", cpu, args->run_on_numa);
                exit(1);
            }
        }
    }

    /* 创建线程 */
    for (j = 0, i = args->min_cpu; i <= args->max_cpu; j++, i += args->interval_cpu) {
        int cpu = i;
        int idx = j;
        int cache_line_size = args->cacheline_size;

        args->threads_args[idx] = aligned_alloc(cache_line_size, sizeof(struct pthread_args));
        assert(args->threads_args[idx] && "Malloc Fatal.");

        struct pthread_args *thread_arg = args->threads_args[idx];

        thread_arg->cpu = cpu;
        thread_arg->numaid = numa_node_of_cpu(cpu);
        thread_arg->cacheline_size = args->cacheline_size;
        thread_arg->test_type = args->test_type;
        thread_arg->test_loop = args->test_loop;

        pthread_create(&args->threadids[idx], NULL, thread_routine, thread_arg);
    }

    /* 等待线程结束 */
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

/**
 * @brief 主程序
 */
int main(int argc, char *argv[])
{
    signal(SIGINT, sig_handler);

    get_system_info();

    parse_args(argc, argv);

    show_system_info();

    create_threads(&default_args);

    return 0;
}