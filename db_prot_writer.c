#include "db_prot_writer.h"
#include "defer_return.h"
#include "model_prof_typeid.h"
#include "rc.h"

static enum rc pack_entry_dist(struct lip_file *file,
                               enum entry_dist const *edist) {
  if (!lip_write_cstr(file, "entry_dist"))
    return RC_EFWRITE;
  if (!lip_write_int(file, *edist))
    return RC_EFWRITE;
  return 0;
}

static enum rc pack_epsilon(struct lip_file *file, imm_float const *epsilon) {
  if (!lip_write_cstr(file, "epsilon"))
    return RC_EFWRITE;
  if (!lip_write_float(file, *epsilon))
    return RC_EFWRITE;
  return 0;
}

static enum rc pack_nuclt(struct lip_file *file,
                          struct imm_nuclt const *nuclt) {
  if (!lip_write_cstr(file, "abc"))
    return RC_EFWRITE;
  if (imm_abc_pack(&nuclt->super, file))
    return RC_EFWRITE;
  return 0;
}

static enum rc pack_amino(struct lip_file *file,
                          struct imm_amino const *amino) {
  if (!lip_write_cstr(file, "amino"))
    return RC_EFWRITE;
  if (imm_abc_pack(&amino->super, file))
    return RC_EFWRITE;
  return 0;
}

static enum rc pack_edist_callb(struct lip_file *file, void const *arg) {
  return pack_entry_dist(file, arg);
}

static enum rc pack_eps_callb(struct lip_file *file, void const *arg) {
  return pack_epsilon(file, arg);
}

static enum rc pack_nuclt_callb(struct lip_file *file, void const *arg) {
  return pack_nuclt(file, arg);
}

static enum rc pack_amino_callb(struct lip_file *file, void const *arg) {
  return pack_amino(file, arg);
}

enum rc prot_db_writer_open(struct prot_db_writer *db, FILE *fp,
                            struct imm_amino const *amino,
                            struct imm_nuclt const *nuclt,
                            struct prot_cfg cfg) {
  int rc = 0;

  if ((rc = db_writer_open(&db->super, fp)))
    return rc;

  db->amino = *amino;
  db->nuclt = *nuclt;
  imm_nuclt_code_init(&db->code, &db->nuclt);
  db->cfg = cfg;

  imm_float const *epsilon = &db->cfg.epsilon;
  enum entry_dist const *edist = &db->cfg.entry_dist;

  if ((rc = db_writer_pack_magic_number(&db->super)))
    defer_return(rc);

  if ((rc = db_writer_pack_prof_typeid(&db->super, PROF_PROT)))
    defer_return(rc);

  if ((rc = db_writer_pack_float_size(&db->super)))
    defer_return(rc);

  if ((rc = db_writer_pack_header(&db->super, pack_edist_callb, edist)))
    defer_return(rc);

  if ((rc = db_writer_pack_header(&db->super, pack_eps_callb, epsilon)))
    defer_return(rc);

  if ((rc = db_writer_pack_header(&db->super, pack_nuclt_callb, &db->nuclt)))
    defer_return(rc);

  if ((rc = db_writer_pack_header(&db->super, pack_amino_callb, &db->amino)))
    defer_return(rc);

  return rc;

defer:
  db_writer_close(&db->super, false);
  return rc;
}

static enum rc pack_profile(struct lip_file *file, void const *prof) {
  return prot_prof_pack(prof, file);
}

enum rc prot_db_writer_pack_profile(struct prot_db_writer *db,
                                    struct prot_prof const *profile) {
  return db_writer_pack_prof(&db->super, pack_profile, profile);
}
