#ifndef MODEL_STD_PROF_H
#define MODEL_STD_PROF_H

#include "model_prof.h"
#include "rc.h"
#include <stdio.h>

struct std_prof {
  struct prof super;

  struct {
    struct imm_dp null;
    struct imm_dp alt;
  } dp;
};

void std_prof_init(struct std_prof *, char const *accession,
                   struct imm_code const *);
enum rc standard_profile_unpack(struct std_prof *prof, FILE *fp);
enum rc std_prof_pack(struct std_prof const *prof, struct lip_file *);

#endif
