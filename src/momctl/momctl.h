#ifndef _MOMCTL_H
#define _MOMCTL_H

int send_command(int stream, int cmd);

int send_command_str(int stream, int cmd, char *query);

int check_success(int stream);

char *read_mom_reply(int *local_errno, int stream);

int do_mom(char *HPtr, int MOMPort, int CmdIndex);

void MCShowUsage(char *Msg);

#endif /* _MOMCTL_H */
