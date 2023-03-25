
#include "cstr.h"


void
cstr_remove (char *src, char *dst, char *remove)
{
    if (! dst)
        return;
    
    char *tmp;
    for (tmp = src; *tmp != '\0'; tmp++)
    {
        char cont = 1;
        for (char *t = remove; *t != '\0'; t++)
            if (*tmp == *t)
            {
                cont = 0;
                break;
            }

        if (cont)
            *dst++ = *tmp;
    }

    *dst = '\0';
}


char *
cstr_get (char *first, char **last, char c)
{
    *last = NULL;

    if (! first)
        return NULL;

    char *tmp = strchr (first, c);
    if (tmp)
    {
        *tmp = '\0';
        *last = tmp + 1;
    }
    return first;
}

