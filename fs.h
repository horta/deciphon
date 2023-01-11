#ifndef FS_H
#define FS_H

#if defined(_ANSI_SOURCE)
#undef _ANSI_SOURCE
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define FS_MAP(X)                                                              \
  X(ECHMOD, "chmod failed")                                                    \
  X(ECLOSE, "close failed")                                                    \
  X(ECREAT, "creat failed")                                                    \
  X(EFCLOSE, "fclose failed")                                                  \
  X(EFCNTL, "fcntl failed")                                                    \
  X(EFCOPYFILE, "fcopyfile failed")                                            \
  X(EFGETS, "fgets failed")                                                    \
  X(EFILENO, "fileno failed")                                                  \
  X(EFOPEN, "fopen failed")                                                    \
  X(EFPUTC, "fputc failed")                                                    \
  X(EFREAD, "fread failed")                                                    \
  X(EFSEEK, "fseek failed")                                                    \
  X(EFSTAT, "fstat failed")                                                    \
  X(EFSYNC, "fsync failed")                                                    \
  X(EFTELL, "ftell failed")                                                    \
  X(EFWRITE, "fwrite failed")                                                  \
  X(EINVAL, "invalid value")                                                   \
  X(EMKSTEMP, "mkstemp failed")                                                \
  X(EMKDIR, "mkdir failed")                                                    \
  X(ENOMEM, "not enough memory")                                               \
  X(EOPEN, "open failed")                                                      \
  X(EREADLINK, "readlink failed")                                              \
  X(ERMDIR, "rmdir failed")                                                    \
  X(ESENDFILE, "sendfile failed")                                              \
  X(ESTAT, "stat failed")                                                      \
  X(ETMPFILE, "tmpfile failed")                                                \
  X(ETRUNCPATH, "truncated path")                                              \
  X(EUNLINK, "unlink failed")

enum fs_rc {
#define X(A, _) FS_##A,
  FS_MAP(X)
#undef X
};

int fs_tell(FILE *restrict fp, long *offset);
int fs_seek(FILE *restrict fp, long offset, int whence);

int fs_copy_fp(FILE *restrict dst, FILE *restrict src);

int fs_refopen(FILE *fp, char const *mode, FILE **out);
int fs_fileno(FILE *fp, int *fd);
int fs_getpath(FILE *fp, unsigned size, char *filepath);

char const *fs_strerror(int rc);

#endif
