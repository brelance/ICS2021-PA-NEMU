#include <isa.h>
#include <string.h>
#include <sdb.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

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
  TK_REG,      // registers
  TK_NUM,      // decimal number
};

static struct rule
{
  const char *regex;
  int token_type;
} rules[] = {
    {" +", TK_NOTYPE}, // spaces
    {"==", TK_EQ},     // equal
    {"\\!=", TK_NEQ},
    {"\\+", TK_PLUS}, // plus
    {"\\-", TK_MINUS},
    {"\\*", TK_ASTERISK},
    {"\\/", TK_SLASH},
    {"\\(", TK_LPAREN},
    {"\\)", TK_RPAREN},
    {"[\\d]+", TK_NUM},
    {"\\$(0|\\w\\d)", TK_REG},
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
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
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token
{
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e)
{
  int position = 0;
  int i;
  regmatch_t pmatch;

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

        switch (rules[i].token_type)
        {
        case TK_NOTYPE:
          break;

        case TK_EQ:
          tokens[i].type = TK_EQ;
          break;

        case TK_NEQ:
          tokens[i].type = TK_NEQ;
          break;

        case TK_PLUS:
          tokens[i].type = TK_PLUS;
          break;

        case TK_MINUS:
          tokens[i].type = TK_MINUS;
          break;

        case TK_ASTERISK:
          tokens[i].type = TK_ASTERISK;
          break;

        case TK_SLASH:
          tokens[i].type = TK_SLASH;
          break;

        case TK_LPAREN:
          tokens[i].type = TK_LPAREN;
          break;

        case TK_RPAREN:
          tokens[i].type = TK_RPAREN;
          break;

        case TK_REG:
          tokens[i].type = TK_ASTERISK;
          strncpy(tokens[i].str, substr_start, substr_len);
          break;

        case TK_NUM:
          tokens[i].type = TK_NUM;
          strncpy(tokens[i].str, substr_start, substr_len);
          break;

        default:
          break;
        }
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

bool check_parentheses(int p, int q)
{
  int lcount = 0;
  int i = 0;
  while (tokens[i + 1].type != 0)
  {
    switch (tokens[i].type)
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

    i++;
  }
  return lcount == 1 && tokens[i].type == TK_RPAREN;
}

int eval(int p, int q)
{
  if (p < q)
  {
    printf("bad expression: left border %d < right border %d", p, q);
    return -1;
  }
  else if (p == q)
  {
    return (int)tokens[p].str;
  }
  else if (check_parentheses(p, q))
  {
    return eval(p + 1, q - 1);
  }
  else
  {
    if (tokens[p].type == TK_NUM || tokens[p].type == TK_REG) {
      if (tokens[p].type == TK_REG) {
        *tokens[p].str = isa_reg_read(tokens[p].str);
      }
      switch (tokens[p + 1].type)
      {
      case TK_PLUS:
        return (uint64_t)tokens[p].str - eval(p + 2, q);
      case TK_MINUS:
        return (uint64_t)tokens[p].str - eval(p + 2, q);
      case TK_ASTERISK:
        return (uint64_t)tokens[p].str * eval(p + 2, q);
      case TK_SLASH:
        return (uint64_t)tokens[p].str / eval(p + 2, q);
      default:
        pruint64_tf("bad expression: please check your input");
        break;
      }
    }
  }
}

word_t expr(char *e, bool *success)
{
  if (!make_token(e))
  {
    *success = false;
    return 0;
  }
  

  return 0;
}
