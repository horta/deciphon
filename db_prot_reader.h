#ifndef DB_PROT_READER_H
#define DB_PROT_READER_H

#include "db_reader.h"
#include "model_entry_dist.h"
#include "model_prot_cfg.h"
#include "model_prot_prof.h"

struct prot_db_reader
{
  struct db_reader super;
  struct imm_amino amino;
  struct imm_nuclt nuclt;
  struct imm_nuclt_code code;
  struct prot_cfg cfg;
};

enum rc prot_db_reader_open(struct prot_db_reader *db, FILE *fp);

#endif
