   #include <stdlib.h>
       #include <stdio.h>
       #include <unistd.h>
       #include <string.h>
       #include <sys/ioctl.h>
       #include <linux/perf_event.h>
       #include <asm/unistd.h>
       #include <vector>

       static long
       perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                       int cpu, int group_fd, unsigned long flags)
       {
           int ret;

           ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                          group_fd, flags);
           return ret;
       }

       int
       main(int argc, char **argv)
       {
           struct perf_event_attr pe;
           long long countr, countw, count;
           int fdr, fdw;

           const int sized = 10*1024*1024;
           std::vector<int> myarray(sized);

           memset(&pe, 0, sizeof(struct perf_event_attr));
           //pe.type = PERF_TYPE_HARDWARE;
           pe.type = 12; // dumped 
           pe.size = sizeof(struct perf_event_attr);
           pe.config = 1;  // 1 - data_reads , 2 - data_writes
           pe.disabled = 1;
           pe.inherit = 1;
           pe.exclude_kernel = 0;
           pe.exclude_hv = 0;
#ifdef ENABLE_PERF
           fdr = perf_event_open(&pe, -1, 0, -1, PERF_FLAG_FD_CLOEXEC);
           if (fdr == -1) {
              fprintf(stderr, "Error opening leader %llx\n", pe.config);
              exit(EXIT_FAILURE);
           }
           pe.config = 2;  // 1 - data_reads , 2 - data_writes
           fdw = perf_event_open(&pe, -1, 0, -1, PERF_FLAG_FD_CLOEXEC);
           if (fdr == -1) {
              fprintf(stderr, "Error opening leader %llx\n", pe.config);
              exit(EXIT_FAILURE);
           }
           ioctl(fdr, PERF_EVENT_IOC_RESET, 0);
           ioctl(fdw, PERF_EVENT_IOC_RESET, 0);
           ioctl(fdr, PERF_EVENT_IOC_ENABLE, 0);
           ioctl(fdw, PERF_EVENT_IOC_ENABLE, 0);
#endif

           for (int i =0; i<sized; ++i) {
              myarray[i] = i%100;
           }

#ifdef ENABLE_PERF
           ioctl(fdr, PERF_EVENT_IOC_DISABLE, 0);
           ioctl(fdw, PERF_EVENT_IOC_DISABLE, 0);
           read(fdr, &countr, sizeof(long long));
           read(fdw, &countw, sizeof(long long));

           printf("Read Traffic: %lld MiB\n", countr*64/1024/1024);
           printf("Write Traffic: %lld MiB\n", countw*64/1024/1024);

#endif
           int checksum = 0;
           for (int i =0; i<sized; ++i) {
              checksum= (checksum + myarray[i])%13;
           }

           close(fdr);
           close(fdw);
       }
