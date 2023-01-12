#ifndef DECIPHON_LIMITS_H
#define DECIPHON_LIMITS_H

enum limits {
  BITS_PER_PROFILE_TYPEID = 16,
  MAX_NPROFILES = (1 << 20),
  NUM_THREADS = 64,
  PROFILE_ACC_SIZE = 32,
  PROF_NAME_SIZE = 256,
  PROT_MODEL_CORE_SIZE_MAX = 4096,
  MIME_TYPE_SIZE = 128,
  FILENAME_SIZE = 256,
  PATH_SIZE = 512,
  ERROR_SIZE = 256,
};

#endif
