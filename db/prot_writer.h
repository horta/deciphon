#ifndef DB_PROT_WRITER_H
#define DB_PROT_WRITER_H

#include "model/entry_dist.h"
#include "model/prot_cfg.h"
#include "model/prot_prof.h"
#include "rc.h"
#include "writer.h"
#include <stdio.h>

struct prot_db_writer {
  struct db_writer super;
  struct imm_amino amino;
  struct imm_nuclt nuclt;
  struct imm_nuclt_code code;
  struct prot_cfg cfg;
};

enum rc prot_db_writer_open(struct prot_db_writer *db, FILE *fp,
                            struct imm_amino const *amino,
                            struct imm_nuclt const *nuclt, struct prot_cfg cfg);

enum rc prot_db_writer_pack_profile(struct prot_db_writer *db,
                                    struct prot_prof const *profile);

#endif
