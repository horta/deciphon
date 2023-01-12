#ifndef FS_H
#define FS_H

#include <stdio.h>

#define FS_MAP(X)                                                              \
  X(EFCNTL, "fcntl failed")                                                    \
  X(EFILENO, "fileno failed")                                                  \
  X(EFOPEN, "fopen failed")                                                    \
  X(EFREAD, "fread failed")                                                    \
  X(EFSEEK, "fseek failed")                                                    \
  X(EFTELL, "ftell failed")                                                    \
  X(EFWRITE, "fwrite failed")                                                  \
  X(ETRUNCPATH, "truncated path")

enum {
#define X(A, _) FS_##A,
  FS_MAP(X)
#undef X
};

int fs_tell(FILE *restrict fp, long *offset);
int fs_seek(FILE *restrict fp, long offset, int whence);
int fs_copy(FILE *restrict dst, FILE *restrict src);
int fs_refopen(FILE *fp, char const *mode, FILE **out);
int fs_fileno(FILE *fp, int *fd);
int fs_getpath(FILE *fp, unsigned size, char *filepath);
char const *fs_strerror(int rc);

#endif
