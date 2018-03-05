#ifndef GSTRING_H
#define GSTRING_H

typedef unsigned long size_t;
typedef signed long ssize_t;

static void *gmemset(void *s, int c, size_t n) {
    unsigned char *p = (unsigned char *)s;

    if (NULL == p)
        return NULL;

    for (unsigned int i = 0; i < n; i++) {
        *p++ = c;
    }

    return s;
}

static void *gmemcpy(void *dest, const void *src, size_t n) {
    unsigned char *p = (unsigned char *)dest;
    unsigned char *s = (unsigned char *)src;

    if (NULL == dest || NULL == src)
        return NULL;

    for (unsigned int i = 0; i < n; i++) {
        *p++ = *s++;
    }

    return dest;
}

static unsigned int gstrlen(const char *str) {
    unsigned int i = 0;

    if (NULL == str)
        return 0;

    while (*str++ != 0)
        i++;

    return i;
}
#endif // GSTRING_H
