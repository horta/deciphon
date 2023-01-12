#ifndef RC_H
#define RC_H

#define RC_MAP(X)                                                              \
  X(RC_OK, "ok")                                                               \
  X(RC_EOPENHMM, "failed to open HMM file")                                    \
  X(RC_EOPENDB, "failed to open DB file")                                      \
  X(RC_EFWRITE, "failed to write to file")                                     \
  X(RC_EOPENTMP, "failed to open temporary file")                              \
  X(RC_EFTELL, "failed to get file position")                                  \
  X(RC_ELARGEPROFILE, "profile is too large")                                  \
  X(RC_EGETPATH, "failed to get file path")                                    \
  X(RC_ETRUNCPATH, "truncated file path")                                      \
  X(RC_END, "end")                                                             \
  X(RC_TIMEDOUT, "timed out")                                                  \
  X(RC_EFAIL, "unspecified error")                                             \
  X(RC_EINVAL, "invalid values")                                               \
  X(RC_EIO, "i/o failure")                                                     \
  X(RC_ENOMEM, "not-enough-memory")                                            \
  X(RC_EPARSE, "parsing error")                                                \
  X(RC_EAPI, "api error")                                                      \
  X(RC_EHTTP, "http error")

enum rc {
#define X(A, _) A,
  RC_MAP(X)
#undef X
      RC_ENUM_SIZE,
};

char const *rc_string(int rc);

#endif
