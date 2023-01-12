#include "expect.h"
#include "array_size.h"
#include "rc.h"
#include <string.h>

int expect_map_size(struct lip_file *file, unsigned size) {
  unsigned sz = 0;
  if (!lip_read_map_size(file, &sz))
    return RC_EFREAD;
  return size == sz ? 0 : RC_EFDATA;
}

int expect_map_key(struct lip_file *file, char const key[]) {
  unsigned size = 0;
  char buf[16] = {0};

  if (!lip_read_str_size(file, &size))
    return RC_EFREAD;

  if (size > array_size(buf))
    return RC_EFDATA;

  if (!lip_read_str_data(file, size, buf))
    return RC_EFREAD;

  if (size != (unsigned)strlen(key))
    return RC_EFDATA;

  return strncmp(key, buf, size) ? RC_EFDATA : 0;
}
