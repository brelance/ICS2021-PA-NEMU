#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static const int buf_size = 65536;
static char buf[65536]= {};
static int point = 0;
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
// static int choose[9] = {2, 1, 2, 0, 0, 1, 2, 0, 0};
// static int i = -1;
// char *t_strcat(char *dest, const char *src)
// {
//   size_t dest_len = strlen(dest);
//   size_t src_len = strlen(src);
//   size_t i;
//   for (i = 0; i < src_len; i++)
//   {
//     if (dest_len > 65536)
//     {
//       printf("buf overflow");
//       return NULL;
//     }
//     dest[dest_len + 1] = src[i];
//   }
// }

static char *code_format =
    "#include <stdio.h>\n"
    "int main() { "
    "  unsigned result = %s; "
    "  printf(\"%%u\", result); "
    "  return 0; "
    "}";

char *gen_rand_op()
{
  srand(time(0) + rand());
  switch (rand() % 4)
  {
  case 0:
    return " + ";
  case 1:
    return " - ";
  case 2:
    return " * ";
  case 3:
    return " / ";
  }
  return "/";
}

static int gen_rand_expr()
{
  if (point > buf_size) {
    return -1;
  }
  srand(time(0) + rand());
  int choose = rand() % 3;

  switch (choose)
  {
  case 0: ;
    int num = rand() % 16;
    if (num == 0 && buf[point - 1] == '/') {
      num = 1;
    }
    char *snum = (char *)malloc(sizeof(int));
    sprintf(snum, "%d", num);
    strcat(buf, snum);
    point += 4;
    break;
  case 1:
    strcat(buf, "(");
    point++;
    gen_rand_expr();
    strcat(buf, ")");
    point++;
    break;
  default:
    gen_rand_expr();
    char *op = gen_rand_op();
    strcat(buf, op);
    point++;
    gen_rand_expr();
    break;
  }
  return 0;
}

int main(int argc, char *argv[])
{
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1)
  {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i++)
  {
    if (gen_rand_expr() == -1) {
      printf("buf overflow");
      continue;
    }

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0)
      continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
