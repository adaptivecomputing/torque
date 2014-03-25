#ifdef TROOTCPUSET_PATH
#undef TROOTCPUSET_PATH
#endif
#ifdef TBOOTCPUSET_PATH
#undef TBOOTCPUSET_PATH
#endif
#ifdef TTORQUECPUSET_PATH
#undef TTORQUECPUSET_PATH
#endif
#ifdef TROOTCPUSET_BASE
#undef TROOTCPUSET_BASE
#endif
#ifdef TBOOTCPUSET_BASE
#undef TBOOTCPUSET_BASE
#endif
#ifdef TTORQUECPUSET_BASE
#undef TTORQUECPUSET_BASE
#endif
#define TROOTCPUSET_PATH     "./dev/cpuset"
#define TBOOTCPUSET_PATH     "./dev/cpuset/boot"
#define TTORQUECPUSET_PATH   "./dev/cpuset/torque"
#define TROOTCPUSET_BASE   "./"
#define TBOOTCPUSET_BASE   "./boot"
#define TTORQUECPUSET_BASE "./torque"

