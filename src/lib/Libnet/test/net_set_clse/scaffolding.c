#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "net_connect.h" /* connection */
#include "server_limits.h" /* PBS_NET_MAX_CONNECTIONS */

struct connection svr_conn[PBS_NET_MAX_CONNECTIONS];
