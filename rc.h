#ifndef RC_H
#define RC_H

#define RC_MAP(X)                                                              \
  X(RC_OK, "ok")                                                               \
  X(RC_EFOPEN, "failed to open file")                                          \
  X(RC_EOPENHMM, "failed to open HMM file")                                    \
  X(RC_EOPENDB, "failed to open DB file")                                      \
  X(RC_EFWRITE, "failed to write to file")                                     \
  X(RC_EFREAD, "failed to read from file")                                     \
  X(RC_EOPENTMP, "failed to open temporary file")                              \
  X(RC_EFTELL, "failed to get file position")                                  \
  X(RC_ELARGEMODEL, "model is too large")                                      \
  X(RC_ELARGEPROFILE, "profile is too large")                                  \
  X(RC_EMANYTRANS, "too many transitions")                                     \
  X(RC_EGETPATH, "failed to get file path")                                    \
  X(RC_ETRUNCPATH, "truncated file path")                                      \
  X(RC_EMANYPARTS, "too may partitions")                                       \
  X(RC_EFUNCUSE, "invalid function usage")                                     \
  X(RC_EFCLOSE, "failed to close file")                                        \
  X(RC_EFSEEK, "failed to seek file")                                          \
  X(RC_EEOF, "unexpected end-of-file")                                         \
  X(RC_END, "end")                                                             \
  X(RC_TIMEDOUT, "timed out")                                                  \
  X(RC_EFAIL, "unspecified error")                                             \
  X(RC_EINVAL, "invalid value")                                                \
  X(RC_EFDATA, "invalid file data")                                            \
  X(RC_EIO, "i/o failure")                                                     \
  X(RC_ENOMEM, "not enough memory")                                            \
  X(RC_EPARSE, "parsing error")                                                \
  X(RC_EAPI, "api error")                                                      \
  X(RC_EHTTP, "http error")

enum rc
{
#define X(A, _) A,
  RC_MAP(X)
#undef X
      RC_ENUM_SIZE,
};

char const *rc_string(int rc);

#endif
