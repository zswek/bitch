// debug.h

#ifndef	_DEBUG_H_
#define	_DEBUG_H_

#include <lib/printk.h>

#ifdef DEBUG

#define debug(format, ...)      printk("[" __FILE__ ":%d] " \
format "\n",__LINE__, ##__VA_ARGS__)

#else

#define debug(format, ...)

#endif

#endif