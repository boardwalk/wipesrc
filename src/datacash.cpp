#include <stdio.h>
#include <stdlib.h>

uint32_t* make_data_cache() {
  uint32_t* cache_addr;

  cache_addr = (uint32_t*)malloc(1024);

  if (cache_addr == NULL) {
    fprintf(stderr, "failed to allocate data cache\n");
    exit(-1);
  }

  return cache_addr;
}
