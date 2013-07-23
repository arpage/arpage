#ifndef __RUNNER2UI_H__
#define __RUNNER2UI_H__

#include <pthread.h>

#include <jack/jack.h>
#include <jack/ringbuffer.h>

typedef extern "C" struct {
  
  unsigned int arpno;
  unsigned int ctlno;
  double       pvalue;
  
} runner_msgtype;

#define DEFAULT_RB_SIZE 16384           /* ringbuffer size in frames */
jack_ringbuffer_t *rb;

pthread_mutex_t disk_thread_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  data_ready       = PTHREAD_COND_INITIALIZER;

#endif
