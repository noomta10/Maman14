#pragma once
#define MY_DEBUG
#ifdef MY_DEBUG
#define LOG_DEBUG(x) printf("debug %s\n",x)
#else
#define LOG_DEBUG(x) 
#endif
