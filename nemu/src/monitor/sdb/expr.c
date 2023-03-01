#include <regex.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <isa.h>
#include <common.h>

typedef unsigned long uint64_t;

enum
{
  TK_NOTYPE = 256,
  TK_EQ,       // "=="
  TK_NEQ,      // "!="
  TK_PLUS,     // "+"
  TK_MINUS,    // "-"
  TK_ASTERISK, // "*"
  TK_SLASH,    // "/"
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
    {"[0-9]+", TK_NUM},
    {" ", TK_NOTYPE}, // spaces
    {"==", TK_EQ},    // equal
    {"\\!=", TK_NEQ},
    {"\\+", TK_PLUS}, // plus
    {"\\-", TK_MINUS},
    {"\\*", TK_ASTERISK},
    {"\\/", TK_SLASH},
    {"\\(", TK_LPAREN},
    {"\\)", TK_RPAREN},
    {"\\$(0|[a-z][0-9])", TK_REG},
};

#define NR_REGEX 11
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
  int token_point = 0;

  nr_token = 0;

  while (e[position] != '\0')
  {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++)
    {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
      {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        if (rules[i].token_type == TK_NOTYPE)
        {
          break;
        }

        strncpy(tokens[token_point].str, substr_start, substr_len);

        switch (rules[i].token_type)
        {
        case TK_EQ:
          tokens[token_point].type = TK_EQ;
          break;

        case TK_NEQ:
          tokens[token_point].type = TK_NEQ;
          break;

        case TK_PLUS:
          tokens[token_point].type = TK_PLUS;
          break;

        case TK_MINUS:
          tokens[token_point].type = TK_MINUS;
          break;

        case TK_ASTERISK:
          tokens[token_point].type = TK_ASTERISK;
          break;

        case TK_SLASH:
          tokens[token_point].type = TK_SLASH;
          break;

        case TK_LPAREN:
          tokens[token_point].type = TK_LPAREN;
          break;

        case TK_RPAREN:
          tokens[token_point].type = TK_RPAREN;
          break;

        case TK_REG:
          tokens[token_point].type = TK_ASTERISK;
          break;

        case TK_NUM:
          tokens[token_point].type = TK_NUM;
          break;

        default:
          break;
        }
        token_point++;
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
    if (tq < 31 && tokens[tq + 1].type != 0 && tokens[tq + 1].type != TK_RPAREN) {
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
        uint64_t reg = isa_reg_read(tokens[p].str);
        sprintf(tokens[p].str, "%lu", reg);
      }
      uint64_t num = strtoul(tokens[p].str, NULL, 10);
      printf("%lu\n", num);
      switch (tokens[p + 1].type)
      {
      case TK_PLUS:
        if (tokens[p - 1].type != TK_PLUS && tokens[p - 1].type != TK_MINUS && tokens[p - 1].type != TK_LPAREN && tokens[p - 1].type != TK_NUM)
        {
          tokens[p - 1].type = tokens[p + 1].type;
          return num;
        }
        return num + eval(p + 2, q);
      case TK_MINUS:;
        if (tokens[p - 1].type != TK_PLUS && tokens[p - 1].type != TK_MINUS && tokens[p - 1].type != TK_LPAREN && tokens[p - 1].type != TK_NUM)
        {
          tokens[p - 1].type = 0;
          return num;
        }
        return num - eval(p + 2, q);
      case TK_ASTERISK:
        if (tokens[p - 1].type == TK_EQ || tokens[p - 1].type == TK_NEQ)
        {
          tokens[p - 1].type = tokens[p + 1].type;
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
        if (tokens[p - 1].type == TK_EQ || tokens[p - 1].type == TK_NEQ)
        {
          tokens[p - 1].type = tokens[p + 1].type;
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

  int end = 0;
  while (tokens[end].type != 0)
  {
    end++;
  }
  if (check_parentheses(0) == -1)
  {
    printf("error: bracket mismatch in expr");
    return -1;
  }
  return eval(0, end - 1);
}
