#ifndef _UTIL_H
#define _UTIL_H

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((unsigned long) &((TYPE*)0)->MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, type, member) \
        ((type *)(((char *)(ptr)) - offsetof(type,member)))
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof(x[0]))
#endif

#endif
