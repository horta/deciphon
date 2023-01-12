#ifndef DB_WRITER_H
#define DB_WRITER_H

#include "lite_pack.h"
#include "model_prof.h"

typedef enum rc (*pack_prof_func_t)(struct lip_file *file, void const *arg);
typedef enum rc (*pack_header_item_func_t)(struct lip_file *file,
                                           void const *arg);

struct db_writer {
  unsigned nprofiles;
  unsigned header_size;
  struct lip_file file;
  struct {
    struct lip_file header;
    struct lip_file prof_sizes;
    struct lip_file profiles;
  } tmp;
};

enum rc db_writer_open(struct db_writer *db, FILE *fp);
enum rc db_writer_close(struct db_writer *db, bool successfully);

enum rc db_writer_pack_magic_number(struct db_writer *);
enum rc db_writer_pack_prof_typeid(struct db_writer *, int prof_typeid);
enum rc db_writer_pack_float_size(struct db_writer *);
enum rc db_writer_pack_prof(struct db_writer *db, pack_prof_func_t pack_profile,
                            void const *arg);
enum rc db_writer_pack_header(struct db_writer *db,
                              pack_header_item_func_t pack_header_item,
                              void const *arg);

#endif
