#ifndef RC_H
#define RC_H

#define RC_MAP(X)                                                              \
  X(OK, "ok")                                                                  \
  X(EDIFFABC, "different alphabets")                                           \
  X(EEOF, "unexpected end-of-file")                                            \
  X(EFAIL, "unspecified error")                                                \
  X(EFCLOSE, "failed to close file")                                           \
  X(EFDATA, "invalid file data")                                               \
  X(EFOPEN, "failed to open file")                                             \
  X(EFREAD, "failed to read from file")                                        \
  X(EFSEEK, "failed to seek file")                                             \
  X(EFTELL, "failed to get file position")                                     \
  X(EFUNCUSE, "invalid function usage")                                        \
  X(EFWRITE, "failed to write to file")                                        \
  X(EGETPATH, "failed to get file path")                                       \
  X(EINVAL, "invalid value")                                                   \
  X(ELARGEMODEL, "model is too large")                                         \
  X(ELARGEPROFILE, "profile is too large")                                     \
  X(EMANYPARTS, "too may partitions")                                          \
  X(EMANYTRANS, "too many transitions")                                        \
  X(END, "end")                                                                \
  X(ENOMEM, "not enough memory")                                               \
  X(EOPENDB, "failed to open DB file")                                         \
  X(EOPENHMM, "failed to open HMM file")                                       \
  X(EOPENTMP, "failed to open temporary file")                                 \
  X(ETRUNCPATH, "truncated file path")                                         \
  X(EDPUNPACK, "failed to unpack DP")

enum
{
#define X(A, _) A,
  RC_MAP(X)
#undef X
      RC_SIZE,
};

char const *rc_string(int rc);

#endif
