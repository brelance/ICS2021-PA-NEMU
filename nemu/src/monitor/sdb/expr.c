#include <regex.h>
#include <isa.h>
#include <common.h>
#include <macro.h>
#include <debug.h>
#include "sdb.h"

typedef unsigned long uint64_t;

enum
{
  TK_NOTYPE = 256,
  TK_DREF, //decode dreference
  TK_EQ,       // "=="
  TK_NEQ,      // "!="
  TK_AND,
  TK_ASTERISK, // "*"
  TK_SLASH,    // "/"
  TK_PLUS,     // "+"
  TK_MINUS,    // "-"
  TK_LPAREN,   // "("
  TK_RPAREN,   // ")"
  TK_REG, // registers
  TK_NUM, // decimal number
};

static struct rule
{
  const char *regex;
  int token_type;
} rules[] = {
    {" ", TK_NOTYPE}, // spaces
    {"==", TK_EQ},    // equal|
    {"!=", TK_NEQ},
    {"&&", TK_AND},
    {"\\+", TK_PLUS}, // plus
    {"\\-", TK_MINUS},
    {"\\*", TK_ASTERISK},
    {"\\/", TK_SLASH},
    {"\\(", TK_LPAREN},
    {"\\)", TK_RPAREN},
    {"\\$(0|[a-z][0-9])", TK_REG},
    {"[0-9]+", TK_NUM},
};

#define NR_REGEX ARRLEN(rules)
static regex_t re[NR_REGEX] = {};

typedef struct token
{
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

void print_tokne(Token *t)
{
  printf("%s %d\n", t->str, t->type);
}

void init_regex()
{
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++)
  {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0)
    {
      regerror(ret, &re[i], error_msg, 128);
      printf("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

static bool make_token(char *e)
{
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0')
  {
    for (i = 0; i < NR_REGEX; i++)
    {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
      {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        if (rules[i].token_type == TK_NOTYPE)
        {
          break;
        }

        strncpy(tokens[nr_token].str, substr_start, substr_len);

        switch (rules[i].token_type)
        {
        case TK_EQ:
          tokens[nr_token].type = TK_EQ;
          break;

        case TK_NEQ:
          tokens[nr_token].type = TK_NEQ;
          break;

        case TK_PLUS:
          tokens[nr_token].type = TK_PLUS;
          break;

        case TK_MINUS:
          tokens[nr_token].type = TK_MINUS;
          break;

        case TK_ASTERISK:
          tokens[nr_token].type = TK_ASTERISK;
          break;

        case TK_SLASH:
          tokens[nr_token].type = TK_SLASH;
          break;

        case TK_LPAREN:
          tokens[nr_token].type = TK_LPAREN;
          break;

        case TK_RPAREN:
          tokens[nr_token].type = TK_RPAREN;
          break;

        case TK_REG:
          tokens[nr_token].type = TK_ASTERISK;
          break;

        case TK_NUM:
          tokens[nr_token].type = TK_NUM;
          break;

        case TK_AND:
          tokens[nr_token].type = TK_AND;
          break;

        default:
          break;
        }
        nr_token++;
        break;
      }
    }

    if (i == NR_REGEX)
    {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

int check_parentheses(int p)
{
  int lcount = 0;
  while (tokens[p + 1].type != 0)
  {
    if (lcount < 0)
    {
      return -1;
    }
    switch (tokens[p].type)
    {
    case TK_LPAREN:
      lcount += 1;
      break;
    case TK_RPAREN:
      lcount -= 1;
      break;
    default:
      break;
    }
    p++;
  }
  return lcount == 1 && tokens[p].type == TK_RPAREN;
}

int parentheses_position(int p)
{
  int lcount = 0;
  while (tokens[p].type != 0)
  {
    switch (tokens[p].type)
    {
    case TK_LPAREN:
      lcount++;
      break;
    case TK_RPAREN:
      lcount--;
      break;
    default:
      break;
    }
    if (lcount == 0)
    {
      return p;
    }
    p++;
  }
  return -1;
}

uint64_t eval(int p, int q)
{
  if (p > q)
  {
    printf("bad expression: left border %d < right border %d", p, q);
    return -1;
  }
  else if (p == q)
  {
    return strtoul(tokens[p].str, NULL, 10);
  }
  else if (tokens[p].type == TK_LPAREN)
  {
    int tq = parentheses_position(p);
    if (tq < 31 && tokens[tq + 1].type != 0 && tokens[tq + 1].type != TK_RPAREN)
    {
      uint64_t num = eval(p + 1, tq - 1);
      tokens[tq].type = TK_NUM;
      sprintf(tokens[tq].str, "%lu", num);
      return eval(tq, q);
    }
    return eval(p + 1, tq - 1);
  }
  else
  {
    if (tokens[p].type == TK_NUM || tokens[p].type == TK_REG)
    {
      if (tokens[p].type == TK_REG) {
        bool success = false;
        uint64_t reg = isa_reg_str2val(tokens[p].str, &success);
        sprintf(tokens[p].str, "%lu", reg);
      }

      uint64_t num = strtoul(tokens[p].str, NULL, 10);

      switch (tokens[p + 1].type)
      {
      case TK_DREF:
        // TODO:
        break;
      case TK_PLUS:
        if (tokens[p - 1].type != TK_PLUS && tokens[p - 1].type != TK_MINUS && tokens[p - 1].type != TK_LPAREN && tokens[p - 1].type != TK_NUM)
        {
          tokens[p - 1].type = TK_PLUS;
          return num;
        }
        return num + eval(p + 2, q);
      case TK_MINUS:;
        if (tokens[p - 1].type != TK_PLUS && tokens[p - 1].type != TK_MINUS && tokens[p - 1].type != TK_LPAREN && tokens[p - 1].type != TK_NUM)
        {
          tokens[p - 1].type = TK_MINUS;
          return num;
        }
        return num - eval(p + 2, q);
      case TK_ASTERISK:
        if (tokens[p - 1].type == TK_EQ || tokens[p - 1].type == TK_NEQ || tokens[p - 1].type == TK_AND)
        {
          tokens[p - 1].type = TK_ASTERISK;
          return num;
        }
        int anum = eval(p + 2, q);
        num *= anum;
        if (tokens[p + 1].type != TK_ASTERISK)
        {
          if (tokens[p + 1].type == TK_PLUS)
          {
            return num + eval(p + 4, q);
          }
          else
          {
            return num - eval(p + 4, q);
          }
        }
        return num;
      case TK_SLASH:
        if (tokens[p - 1].type == TK_EQ || tokens[p - 1].type == TK_NEQ || tokens[p - 1].type == TK_AND)
        {
          tokens[p - 1].type = TK_SLASH;
          return num;
        }
        int snum = eval(p + 2, q);
        if (tokens[p + 1].type != TK_ASTERISK)
        {
          num /= snum;
          if (tokens[p + 1].type == TK_PLUS)
          {
            return num + eval(p + 4, q);
          }
          else
          {
            return num - eval(p + 4, q);
          }
        }
      case TK_AND:
        if (tokens[p - 1].type == TK_EQ || tokens[p - 1].type == TK_NEQ)
        {
          tokens[p - 1].type = tokens[p + 1].type;
          return num;
        }
        return num && eval(p + 2, q);
      case TK_EQ:
        return num == eval(p + 2, q);
      case TK_NEQ:
        return num != eval(p + 2, q);
      default:
        printf("bad expression: please check your input");
        return -1;
      }
    }
  }
  return -1;
}

uint64_t expr(char *e, bool *success)
{
  if (!make_token(e))
  {
    *success = false;
    return 0;
  }

  if (check_parentheses(0) == -1)
  {
    printf("error: bracket mismatch in expr");
    return -1;
  }

  for (int i = 0; i < nr_token; i++) {
    int pre_type = tokens[i - 1].type;
    if (tokens[i].type == TK_ASTERISK && (i == 0 || (pre_type >= TK_EQ && pre_type <= TK_NEQ))) {
      tokens[i].type = TK_DREF;
    }
  }

  return eval(0, nr_token - 1);
}
