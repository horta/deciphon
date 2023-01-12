#include "db_reader.h"
#include "compiler.h"
#include "db_types.h"
#include "deciphon_limits.h"
#include "expect.h"
#include "imm/imm.h"
#include "logy.h"
#include <stdlib.h>

enum rc db_reader_open(struct db_reader *db, FILE *fp) {
  db->nprofiles = 0;
  db->profile_sizes = 0;
  db->prof_typeid = PROF_NULL;
  lip_file_init(&db->file, fp);
  return 0;
}

void db_reader_close(struct db_reader *db) {
  if (db->profile_sizes)
    free(db->profile_sizes);
}

enum rc db_reader_unpack_magic_number(struct db_reader *db) {
  int rc = 0;

  if ((rc = expect_map_key(&db->file, "magic_number")))
    return rc;

  unsigned number = 0;
  if (!lip_read_int(&db->file, &number))
    return RC_EFREAD;

  return number != MAGIC_NUMBER ? einval("invalid magic number") : 0;
}

enum rc db_reader_unpack_prof_typeid(struct db_reader *db,
                                     enum prof_typeid typeid) {
  int rc = 0;

  if ((rc = expect_map_key(&db->file, "profile_typeid")))
    return rc;

  if (!lip_read_int(&db->file, &db->prof_typeid))
    return RC_EFREAD;

  if (db->prof_typeid != typeid)
    return RC_EFDATA;

  return 0;
}

enum rc db_reader_unpack_float_size(struct db_reader *db) {
  int rc = 0;
  if ((rc = expect_map_key(&db->file, "float_size")))
    return rc;

  unsigned size = 0;
  if (!lip_read_int(&db->file, &size))
    return RC_EFREAD;

  return size != IMM_FLOAT_BYTES ? einval("invalid float size") : 0;
}

static enum rc unpack_header_profile_sizes(struct db_reader *db) {
  enum lip_1darray_type type = 0;

  if (!lip_read_1darray_size_type(&db->file, &db->nprofiles, &type))
    return RC_EFREAD;

  if (type != LIP_1DARRAY_UINT32)
    return RC_EFDATA;

  db->profile_sizes = malloc(sizeof(*db->profile_sizes) * db->nprofiles);
  if (!db->profile_sizes)
    return RC_ENOMEM;

  if (!lip_read_1darray_u32_data(&db->file, db->nprofiles, db->profile_sizes)) {
    free(db->profile_sizes);
    db->profile_sizes = 0;
    return RC_EFREAD;
  }

  return 0;
}

enum rc db_reader_unpack_prof_sizes(struct db_reader *db) {
  int rc = 0;
  if ((rc = expect_map_key(&db->file, "profile_sizes")))
    return rc;
  return unpack_header_profile_sizes(db);
}
