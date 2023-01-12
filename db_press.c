#include "db_press.h"
#include "defer_return.h"
#include "logy.h"
#include "sizeof_field.h"
#include "strlcpy.h"
#include <string.h>

static enum rc count_profiles(struct db_press *p);
static enum rc prepare_writer(struct db_press *p);
static enum rc finish_writer(struct db_press *p, bool succesfully);
static void prepare_reader(struct db_press *p);
static enum rc finish_reader(struct db_press *p);
static enum rc prof_write(struct db_press *p);

enum rc db_press_init(struct db_press *p, char const *hmm, char const *db) {
  p->writer.fp = NULL;
  p->reader.fp = NULL;

  int rc = 0;
  if (!(p->reader.fp = fopen(hmm, "rb")))
    defer_return(RC_EOPENHMM);

  if (!(p->writer.fp = fopen(db, "wb")))
    defer_return(RC_EOPENDB);

  info("counting number of profiles...");
  if ((rc = count_profiles(p)))
    defer_return(rc);

  if ((rc = prepare_writer(p)))
    defer_return(rc);

  prepare_reader(p);

  prot_prof_init(&p->profile, p->reader.h3.prof.meta.acc, &p->writer.db.amino,
                 &p->writer.db.code, p->writer.db.cfg);

  return rc;

defer:
  if (p->writer.fp)
    fclose(p->writer.fp);
  if (p->reader.fp)
    fclose(p->reader.fp);
  p->writer.fp = NULL;
  p->reader.fp = NULL;
  return rc;
}

#define HMMER3 "HMMER3/f"

long db_press_nsteps(struct db_press const *p) { return p->prof_count; }

static enum rc count_profiles(struct db_press *p) {
  unsigned count = 0;

  while (fgets(p->buffer, sizeof_field(struct db_press, buffer),
               p->reader.fp) != NULL) {
    if (!strncmp(p->buffer, HMMER3, strlen(HMMER3)))
      ++count;
  }

  if (!feof(p->reader.fp))
    return eio("failed to count profiles");

  p->prof_count = count;
  rewind(p->reader.fp);
  return RC_OK;
}

enum rc db_press_step(struct db_press *p) {
  enum rc rc = prot_h3reader_next(&p->reader.h3);
  return rc ? rc : prof_write(p);
}

enum rc db_press_cleanup(struct db_press *p, bool succesfully) {
  enum rc rc_r = finish_reader(p);
  enum rc rc_w = finish_writer(p, succesfully);
  return rc_r ? rc_r : (rc_w ? rc_w : RC_OK);
}

static enum rc prepare_writer(struct db_press *p) {
  struct imm_amino const *a = &imm_amino_iupac;
  struct imm_nuclt const *n = &imm_dna_iupac.super;
  struct prot_cfg cfg = PROT_CFG_DEFAULT;

  return prot_db_writer_open(&p->writer.db, p->writer.fp, a, n, cfg);
}

static enum rc finish_writer(struct db_press *p, bool succesfully) {
  if (!p->writer.fp)
    return RC_OK;
  if (!succesfully) {
    db_writer_close((struct db_writer *)&p->writer.db, false);
    fclose(p->writer.fp);
    return RC_OK;
  }
  enum rc rc = db_writer_close((struct db_writer *)&p->writer.db, true);
  if (rc) {
    fclose(p->writer.fp);
    return rc;
  }
  return fclose(p->writer.fp) ? eio("fclose") : RC_OK;
}

static void prepare_reader(struct db_press *p) {
  struct imm_amino const *amino = &p->writer.db.amino;
  struct imm_nuclt_code const *code = &p->writer.db.code;
  struct prot_cfg cfg = p->writer.db.cfg;

  prot_h3reader_init(&p->reader.h3, amino, code, cfg, p->reader.fp);
}

static enum rc finish_reader(struct db_press *p) {
  return fclose(p->reader.fp) ? eio("fclose") : RC_OK;
}

static enum rc prof_write(struct db_press *p) {
  enum rc rc = prot_prof_absorb(&p->profile, &p->reader.h3.model);
  if (rc)
    return rc;

  dcp_strlcpy(p->profile.super.accession, p->reader.h3.prof.meta.acc,
              PROFILE_ACC_SIZE);

  return prot_db_writer_pack_profile(&p->writer.db, &p->profile);
}