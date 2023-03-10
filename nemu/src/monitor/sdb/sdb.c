#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory/paddr.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char *rl_gets()
{
  static char *line_read = NULL;

  if (line_read)
  {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read)
  {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args)
{
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args)
{
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args)
{
  int n = 1;
  if (args != NULL)
  {
    n = sscanf(args, "%d", &n);
  }
  printf("%d", n);
  cpu_exec(n);
  return 0;
}

static int cmd_info(char *args)
{
  char subcmd = *args;
  switch (subcmd)
  {
  case 'r':
    isa_reg_display();
    break;
  case 'w':
    break;
  default:
    printf("Unknown command info '%c'\n", subcmd);
    return -1;
  }
  return 0;
}

static int cmd_scan_mem(char *args)
{
  int n;
  char *save_ptr;
  strtok_r(args, " ", &save_ptr);
  sscanf(args, "%d", &n);
  uint64_t addr = strtoul(save_ptr, NULL, 0);
  for (int i = 0; i < n; i++)
  {
    uint64_t mem = paddr_read(addr, 4);
    printf("%lu: %lu\n", addr, mem);
    addr += 8;
  }
  return 0;
}

static int cmd_expr(char *args)
{
  bool success;
  uint64_t result = expr(args, &success);
  if (success == true)
  {
    printf("Expr result: %lu\n", result);
    return 0;
  }
  printf("cmd_expr(): Calculating expression fails");
  return -1;
}

static int cmd_wp(char *args)
{
  bool success;
  uint64_t addr = expr(args, &success);
  if (!success)
  {
    printf("cmd_wp(): expr fail");
    return -1;
  }
  // TODO:paddr_read() args need bo be considered
  if (new_wp(addr) == NULL)
  {
    return -1;
  };
  return 0;
}
// TODO:
static int cmd_rm_wp(char *args)
{
  bool success;
  uint64_t addr = expr(args, &success);
  if (!success)
  {
    printf("cmd_wp(): expr fail");
    return -1;
  }
  return free_wp(addr);
}

static struct
{
  const char *name;
  const char *description;
  int (*handler)(char *);
} cmd_table[] = {
    {"help", "Display informations about all supported commands", cmd_help},
    {"c", "Continue the execution of the program", cmd_c},
    {"q", "Exit NEMU", cmd_q},
    {"si", "Make progress to run N instructions. If N is no be given, the default number of N is 1", cmd_si},
    {"info", "Print the status of registers or the status of watchpoint", cmd_info},
    {"x", "Output N 4bytes from the initial address defined by the result of EXPR", cmd_scan_mem},
    {"p", "Calculate EXPR", cmd_expr},
    {"w EXPR", "Stop progress when the number of EXPR has changed", cmd_wp},
    {"d N", "Delete watchpoint whose number equals N", cmd_rm_wp},
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args)
{
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL)
  {
    /* no argument given */
    for (i = 0; i < NR_CMD; i++)
    {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else
  {
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(arg, cmd_table[i].name) == 0)
      {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode()
{
  is_batch_mode = true;
}

void sdb_mainloop()
{
  if (is_batch_mode)
  {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL;)
  {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL)
    {
      continue;
    }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end)
    {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(cmd, cmd_table[i].name) == 0)
      {
        if (cmd_table[i].handler(args) < 0)
        {
          return;
        }
        break;
      }
    }

    if (i == NR_CMD)
    {
      printf("Unknown command '%s'\n", cmd);
    }
  }
}

void init_sdb()
{
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
