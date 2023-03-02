#include "sdb.h"
#include <isa.h>
#include <utils.h>

#define NR_WP 32

typedef struct watchpoint
{
  int NO;
  struct watchpoint *next;
  uint64_t addr;
  uint64_t value;
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool()
{
  int i;
  for (i = 0; i < NR_WP; i++)
  {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

// TODO: add more code to secure memory safety
WP *new_wp(uint64_t addr)
{
  if (free_ == NULL) {
    printf("new_wp(): No more watchpoint to provide");
    return NULL;
  }
  uint64_t value = paddr_read(addr, 8);
  WP *wp = free_;
  wp->addr = addr;
  wp->value = value;
  wp->next = head;
  head = wp;
  free_ = wp->next;
  return head;
}

// TODO: add more code to secure memory safety
int free_wp(int n)
{
  if (head == NULL) {
    printf("free_wp(): All watchpoints have been freed");
    return -1;
  }
  WP *wp = head;
  WP *fwp = free_;
  if (head->NO != n)
  {
    while (wp != NULL && wp->next->NO != n)
    {
      wp = wp->next;
    }
  }
  wp->next = wp->next->next;
  free_ = wp->next;
  free_->next = fwp;
  return 0;
}

void check_watchpoint() {
  WP *wp = head;
  while (wp != NULL)
  {
    uint64_t cur_value = paddr_read(wp->addr, 8);
    if (cur_value != wp->value) {
      nemu_state.state = NEMU_STOP;
    }
  }
}
