#ifdef __cplusplus
extern "C" {
#endif
void cpu_timer_start(struct timespec *tstart_cpu);
double cpu_timer_stop(struct timespec tstart_cpu);
#ifdef __cplusplus
}
#endif