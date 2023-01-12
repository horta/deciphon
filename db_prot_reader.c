#include "db_prot_reader.h"
#include "db_reader.h"
#include "defer_return.h"
#include "expect.h"
#include "imm/imm.h"
#include "logy.h"

static enum rc unpack_entry_dist(struct lip_file *file,
                                 enum entry_dist *edist) {
  int rc = 0;
  if ((rc = expect_map_key(file, "entry_dist")))
    return rc;
  if (!lip_read_int(file, edist))
    return RC_EFREAD;

  return (*edist <= ENTRY_DIST_NULL || *edist > ENTRY_DIST_OCCUPANCY)
             ? einval("invalid entry dist")
             : 0;
}

static enum rc unpack_epsilon(struct lip_file *file, imm_float *epsilon) {
  int rc = 0;
  if ((rc = expect_map_key(file, "epsilon")))
    return rc;
  if (!lip_read_float(file, epsilon))
    return eio("read epsilon");

  return (*epsilon < 0 || *epsilon > 1) ? einval("invalid epsilon") : 0;
}

static enum rc unpack_nuclt(struct lip_file *file, struct imm_nuclt *nuclt) {
  int rc = 0;
  if ((rc = expect_map_key(file, "abc")))
    return rc;
  if (imm_abc_unpack(&nuclt->super, file))
    return RC_EFREAD;
  return 0;
}

static enum rc unpack_amino(struct lip_file *file, struct imm_amino *amino) {
  int rc = 0;
  if ((rc = expect_map_key(file, "amino")))
    return rc;
  if (imm_abc_unpack(&amino->super, file))
    return RC_EFREAD;
  return 0;
}

enum rc prot_db_reader_open(struct prot_db_reader *db, FILE *fp) {
  int rc = db_reader_open(&db->super, fp);
  if (rc)
    return rc;

  if ((rc = expect_map_size(&db->super.file, 2)))
    return rc;

  if ((rc = expect_map_key(&db->super.file, "header")))
    return rc;

  if ((rc = expect_map_size(&db->super.file, 8)))
    return rc;

  rc = db_reader_unpack_magic_number(&db->super);
  if (rc)
    defer_return(rc);

  rc = db_reader_unpack_prof_typeid(&db->super, PROF_PROT);
  if (rc)
    defer_return(rc);

  rc = db_reader_unpack_float_size(&db->super);
  if (rc)
    defer_return(rc);

  rc = unpack_entry_dist(&db->super.file, &db->cfg.entry_dist);
  if (rc)
    defer_return(rc);

  rc = unpack_epsilon(&db->super.file, &db->cfg.epsilon);
  if (rc)
    defer_return(rc);

  rc = unpack_nuclt(&db->super.file, &db->nuclt);
  if (rc)
    defer_return(rc);

  rc = unpack_amino(&db->super.file, &db->amino);
  if (rc)
    defer_return(rc);

  imm_nuclt_code_init(&db->code, &db->nuclt);

  rc = db_reader_unpack_prof_sizes(&db->super);
  if (rc)
    defer_return(rc);

  return rc;

defer:
  db_reader_close(&db->super);
  return rc;
}
