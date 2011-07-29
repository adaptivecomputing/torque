#include "license_pbs.h" /* See here for the software license */

int is_pbs_job(void);

void set_if_undefined(char *name, char *val);

void check_environment(void);

int fix_cmdfile(char *ocmd, char *ncmd);

void clean_up(int exval);

char **pbs_setup(char *command, int argc, char *argv[]);

void run_command(char *command, char *argv[]);

char *get_real_command(char *argv0);

int is_interactive_ok(void);

void refuse_interactive(void);

/* main */

