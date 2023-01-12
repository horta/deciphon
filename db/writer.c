#define _POSIX_C_SOURCE 200809L
#include "db/writer.h"
#include "compiler.h"
#include "db/types.h"
#include "deciphon_limits.h"
#include "defer_return.h"
#include "fs.h"
#include "imm/imm.h"
#include "logy.h"
#include "model/model.h"
#include "rc.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void destroy_tempfiles(struct db_writer *db) {
  if (db->tmp.header.fp)
    fclose(db->tmp.header.fp);
  if (db->tmp.profile_sizes.fp)
    fclose(db->tmp.profile_sizes.fp);
  if (db->tmp.profiles.fp)
    fclose(db->tmp.profiles.fp);
}

static enum rc create_tempfiles(struct db_writer *db) {
  lip_file_init(&db->tmp.header, tmpfile());
  lip_file_init(&db->tmp.profile_sizes, tmpfile());
  lip_file_init(&db->tmp.profiles, tmpfile());

  int rc = 0;
  if (!db->tmp.header.fp || !db->tmp.profile_sizes.fp || !db->tmp.profiles.fp)
    defer_return(RC_EOPENTMP);

  return rc;

defer:
  destroy_tempfiles(db);
  return rc;
}

enum rc db_writer_open(struct db_writer *db, FILE *fp) {
  db->nprofiles = 0;
  db->header_size = 0;
  lip_file_init(&db->file, fp);
  return create_tempfiles(db);
}

static enum rc pack_header_profile_sizes(struct db_writer *db) {
  enum lip_1darray_type type = LIP_1DARRAY_UINT32;

  if (!lip_write_1darray_size_type(&db->file, db->nprofiles, type))
    return eio("write array size");

  rewind(lip_file_ptr(&db->tmp.profile_sizes));

  unsigned size = 0;
  while (lip_read_int(&db->tmp.profile_sizes, &size))
    lip_write_1darray_u32_item(&db->file, size);

  if (!feof(lip_file_ptr(&db->tmp.profile_sizes)))
    return eio("write profile sizes");

  return 0;
}

static enum rc pack_header(struct db_writer *db) {
  struct lip_file *file = &db->file;
  if (!lip_write_cstr(file, "header"))
    return eio("write key");

  if (!lip_write_map_size(file, db->header_size + 1))
    return eio("write root map size");

  rewind(lip_file_ptr(&db->tmp.header));
  int r = fs_copy(lip_file_ptr(file), lip_file_ptr(&db->tmp.header));
  if (r)
    return eio("%s", fs_strerror(r));

  if (!lip_write_cstr(file, "profile_sizes"))
    return eio("write key");
  return pack_header_profile_sizes(db);
}

static enum rc pack_profiles(struct db_writer *db) {
  if (!lip_write_cstr(&db->file, "profiles"))
    return eio("write key");

  if (!lip_write_array_size(&db->file, db->nprofiles))
    eio("write array size");

  rewind(lip_file_ptr(&db->tmp.profiles));
  int r = fs_copy(lip_file_ptr(&db->file), lip_file_ptr(&db->tmp.profiles));
  return r ? eio("%s", fs_strerror(r)) : 0;
}

enum rc db_writer_close(struct db_writer *db, bool successfully) {
  if (!successfully) {
    destroy_tempfiles(db);
    return 0;
  }

  int rc = 0;
  if (!lip_write_map_size(&db->file, 2))
    defer_return(RC_EFWRITE);

  if ((rc = pack_header(db)))
    defer_return(rc);

  if ((rc = pack_profiles(db)))
    defer_return(rc);

  return rc;

defer:
  destroy_tempfiles(db);
  return rc;
}

enum rc db_writer_pack_magic_number(struct db_writer *db) {
  if (!lip_write_cstr(&db->tmp.header, "magic_number"))
    return RC_EFWRITE;

  if (!lip_write_int(&db->tmp.header, MAGIC_NUMBER))
    return RC_EFWRITE;

  db->header_size++;
  return 0;
}

enum rc db_writer_pack_profile_typeid(struct db_writer *db,
                                      int profile_typeid) {
  if (!lip_write_cstr(&db->tmp.header, "profile_typeid"))
    return RC_EFWRITE;

  if (!lip_write_int(&db->tmp.header, profile_typeid))
    return RC_EFWRITE;

  db->header_size++;
  return 0;
}

enum rc db_writer_pack_float_size(struct db_writer *db) {
  if (!lip_write_cstr(&db->tmp.header, "float_size"))
    return RC_EFWRITE;

  unsigned size = IMM_FLOAT_BYTES;
  assert(size == 4 || size == 8);
  if (!lip_write_int(&db->tmp.header, size))
    return RC_EFWRITE;

  db->header_size++;
  return 0;
}

enum rc db_writer_pack_profile(struct db_writer *db,
                               pack_profile_func_t pack_profile,
                               void const *arg) {
  int rc = 0;

  long start = 0;
  if ((rc = fs_tell(lip_file_ptr(&db->tmp.profiles), &start)))
    return rc;

  if ((rc = pack_profile(&db->tmp.profiles, arg)))
    return rc;

  long end = 0;
  if ((rc = fs_tell(lip_file_ptr(&db->tmp.profiles), &end)))
    return rc;

  if ((end - start) > UINT_MAX)
    return RC_ELARGEPROFILE;

  unsigned prof_size = (unsigned)(end - start);
  if (!lip_write_int(&db->tmp.profile_sizes, prof_size))
    return RC_EFWRITE;

  db->nprofiles++;
  return rc;
}

enum rc db_writer_pack_header(struct db_writer *db,
                              pack_header_item_func_t pack_header_item,
                              void const *arg) {
  db->header_size++;
  return pack_header_item(&db->tmp.header, arg);
}
