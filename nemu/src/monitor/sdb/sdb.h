#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>
#include <memory/paddr.h>

uint64_t expr(char *e, bool *success);


typedef struct watchpoint WP;
WP *new_wp(uint64_t addr);
int free_wp(int n);
void info_watchpoint();



#endif
