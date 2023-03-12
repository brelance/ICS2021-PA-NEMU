#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

char *format_match(char *buf, const char **fmt_addr, va_list arg)
{
  int d;
  const char *fmt = *fmt_addr;
  if (*fmt == '%')
  {
    switch (*++fmt)
    {
    case 's':
      fmt++;
      *fmt_addr = fmt;
      return va_arg(arg, char *);
    case 'd':
      d = va_arg(arg, int);
      itoa(d, buf, 10);
      fmt++;
      *fmt_addr = fmt;
      return buf;
    default:
      break;
    }
  }
  else
    fmt++;

  *fmt_addr = fmt;
  return NULL;
}

int printf(const char *fmt, ...)
{
  char buf[128];
  va_list arg;
  va_start(arg, fmt);
  vsprintf(buf, fmt, arg);
  for (int i = 0; i < strlen(buf); i++)
  {
    putch(buf[i]);
  }
  va_end(arg);
  return 1;
}

int vsprintf(char *out, const char *fmt, va_list ap)
{
  char buf[10];
  const char *start = fmt;
  out[0] = '\0';
  buf[0] = '\0';
  while (*fmt)
  {
    char *fmt_word = format_match(buf, &fmt, ap);
    if (fmt_word != NULL)
    {
      strncat(out, start, fmt - start - 2);
      strcat(out, fmt_word);
      start = fmt;
    }
  }
  strcat(out, start);
  return 1;
}

int sprintf(char *out, const char *fmt, ...)
{
  va_list arg;
  va_start(arg, fmt);
  char buf[10];
  const char *start = fmt;
  // TODO: bug
  out[0] = '\0';
  buf[0] = '\0';
  // memset(out, '\0', strlen(out));
  // memset(buf, '\0', 10);
  while (*fmt)
  {
    char *fmt_word = format_match(buf, &fmt, arg);
    if (fmt_word != NULL)
    {
      strncat(out, start, fmt - start - 2);
      strcat(out, fmt_word);
      start = fmt;
    }
  }
  strcat(out, start);
  va_end(arg);
  return 1;
}

// TODO: test
int snprintf(char *out, size_t n, const char *fmt, ...)
{
  va_list arg;
  va_start(arg, fmt);
  char buf[10];
  const char *start = fmt;
  out[0] = '\0';
  buf[0] = '\0';
  while (*fmt)
  {
    char *fmt_word = format_match(buf, &fmt, arg);
    if (fmt_word != NULL)
    {
      if (n < fmt - start - 2)
      {
        strncat(out, start, n);
        n = -1;
        break;
      }
      n -= fmt - start - 2;

      size_t arglen = strlen(fmt_word);
      if (n < arglen)
      {
        strncat(out, start, n);
        n = -1;
        break;
      }
      n -= arglen;
      strncat(out, start, fmt - start - 2);
      strcat(out, arg);
      start = fmt;
    }
  }
  va_end(arg);

  if (n > 0)
    strncat(out, start, n);
  return 1;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap)
{
  char buf[10];
  const char *start = fmt;
  out[0] = '\0';
  buf[0] = '\0';
  while (*fmt)
  {
    char *fmt_word = format_match(buf, &fmt, ap);
    if (fmt_word != NULL)
    {
      if (n < fmt - start - 2)
      {
        strncat(out, start, n);
        n = -1;
        break;
      }
      n -= fmt - start - 2;

      size_t arglen = strlen(fmt_word);
      if (n < arglen)
      {
        strncat(out, start, n);
        n = -1;
        break;
      }
      n -= arglen;

      strncat(out, start, fmt - start - 2);
      strcat(out, ap);
      start = fmt;
    }
  }

  if (n > 0)
    strncat(out, start, n);
  return 1;
}

#endif
