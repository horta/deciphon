#ifndef DB_PROF_READER_H
#define DB_PROF_READER_H

#include "deciphon_limits.h"
#include "lite_pack.h"
#include "model_prof_typeid.h"
#include "model_prot_prof.h"
#include <stdint.h>

struct prof_reader
{
  unsigned npartitions;
  unsigned partition_size[NUM_THREADS];
  long partition_offset[NUM_THREADS + 1];
  struct lip_file file[NUM_THREADS];
  enum prof_typeid prof_typeid;
  union
  {
    // struct standard_profile std;
    struct prot_prof pro;
  } profiles[NUM_THREADS];
};

struct db_reader;

enum rc prof_reader_setup(struct prof_reader *reader, struct db_reader *db,
                          unsigned npartitions);
unsigned prof_reader_npartitions(struct prof_reader const *reader);
unsigned prof_reader_partition_size(struct prof_reader const *reader,
                                    unsigned partition);
unsigned prof_reader_nprofiles(struct prof_reader const *reader);
enum rc prof_reader_rewind_all(struct prof_reader *reader);
enum rc prof_reader_rewind(struct prof_reader *reader, unsigned partition);
enum rc prof_reader_next(struct prof_reader *reader, unsigned partition,
                         struct prof **profile);
bool prof_reader_emd(struct prof_reader *reader, unsigned partition);
void prof_reader_del(struct prof_reader *reader);

#endif
