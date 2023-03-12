#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s)
{
  size_t len;
  for (len = 0; s[len] != '\0'; len++)
  {
  };
  return len;
}

char *strcpy(char *dst, const char *src)
{
  size_t i;
  for (i = 0; src[i] != '\0'; i++)
    dst[i] = src[i];
  dst[i] = '\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n)
{
  size_t i;
  for (i = 0; i < n && src[i] != '\0'; i++)
    dst[i] = src[i];
  for (; i < n; i++)
    dst[i] = '\0';
  return dst;
}

char *strcat(char *dst, const char *src)
{
  size_t dlen = strlen(dst);
  size_t i;
  for (i = 0; src[i] != '\0'; i++)
    dst[dlen + i] = src[i];
  dst[dlen + i] = '\0';
  return dst;
}


char *strncat(char *dst, const char *src, size_t n)
{
  size_t dlen = strlen(dst);
  size_t i;
  for (i = 0; src[i] != '\0' && i < n; i++)
    dst[dlen + i] = src[i];
  dst[dlen + i] = '\0';
  return dst;
}


int strcmp(const char *s1, const char *s2)
{
  const unsigned char *p1 = (const unsigned char *)s1;
  const unsigned char *p2 = (const unsigned char *)s2;
  unsigned char c1, c2;
  do
  {
    c1 = (unsigned char)*p1++;
    c2 = (unsigned char)*p2++;
    if (c1 == '\0')
      return c1 - c2;
  } while (c1 == c2);

  return c1 - c2;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
  const unsigned char *p1 = (const unsigned char *)s1;
  const unsigned char *p2 = (const unsigned char *)s2;
  unsigned char c1, c2;
  size_t i = 0;
  do
  {
    c1 = (unsigned char)*p1++;
    c2 = (unsigned char)*p2++;
    i++;
    if (c1 == '\0')
      return c1 - c2;
  } while (c1 == c2 && i < n);
  return c1 - c2;
}

void *memset(void *s, int c, size_t n)
{
  size_t i;
  unsigned char *p = s;
  // TODO: bug
  for (i = 0; i < n; i++)
    *p++ = (char)c;
  return s;
}

void *memmove(void *dst, const void *src, size_t n)
{
  // char *temp = (char*)malloc(s)
  panic("not implemetion");
}

void *memcpy(void *out, const void *in, size_t n)
{
  unsigned char *p1 = out;
  unsigned const char *p2 = in;
  size_t i;
  for (i = 0; i < n; i++)
    p1[i] = p2[i];
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
  return strncmp((const char *)s1, (const char *)s2, n);
}

#endif
