// lib/string.h

#ifndef _LIB_STRING_H_
#define _LIB_STRING_H_

#include <bits/types.h>

inline void memcpy(void *dest, void *src, _size_t len)
{
        _u8_t *sr = src;
        _u8_t *dst = dest;

        while (len != 0) {
                *dst++ = *sr++;
                len--;
        }
}

inline int memcmp(void *dest, void *src, _size_t len)
{
        _u8_t *sr = src;
        _u8_t *dst = dest;

        int ret = 0;
        
        while (len != 0) {
                if (!(ret = (*dst++) - (*sr++))) break;
                len--;
        }
        
        return ret;
}

inline void memset(void *dest, _u8_t val, _size_t len)
{
        for (_u8_t *dst = (_u8_t *)dest; len != 0; len--) {
                *dst++ = val;
        }
}

inline int strcmp(const char *dest, const char *src)
{
        int ret = 0;

        while(!(ret = *(unsigned char *)src - *(unsigned char *)dest) && *dest) {
                ++src;
                ++dest;
        }

        if (ret < 0) {
                ret = -1;
        }
        else if (ret > 0) {
                ret = 1;
        }

        return ret;
}

inline char *strcpy(char *dest, const char *src)
{
        char *tmp = dest;

        while (*src) {
              *dest++ = *src++;
        }

        *dest = '\0';
        
        return tmp;
}

inline char *strncpy(char *dest, const char *src, _size_t len)
{
        char *dst = dest;

        while (len > 0) {
                while (*src) {
                        *dest++ = *src++;
                }
                len--;
        }

        *dest = '\0';

        return dst;
}

inline char *strcat(char *dest, const char *src)
{
        char *cp = dest;

        while (*cp) {
              cp++;
        }

        while ((*cp++ = *src++))
              ;

        return dest;
}

inline _size_t strlen(const char *src)
{
        const char *eos = src;

        while (*eos++)
              ;
                
        return (eos - src - 1);
}

inline char *strchr(char *s,char c)
{
    while(*s != '\0' && *s != c) {
        ++s;
    }
    
    return *s==c ? s : NULL;
}

#endif
