#include "model_std_prof.h"
#include "imm/imm.h"
#include "lite_pack/file/file.h"
#include "lite_pack/lite_pack.h"
#include "logy.h"
#include "model_prof.h"
#include "model_prof_typeid.h"
#include "model_std_prof.h"
#include "model_std_state.h"
#include "rc.h"

static void del(struct prof *prof) {
  if (prof) {
    struct std_prof *p = (struct std_prof *)prof;
    imm_del(&p->dp.null);
    imm_del(&p->dp.alt);
  }
}

static enum rc read(struct prof *prof, struct lip_file *file) {
  struct std_prof *p = (struct std_prof *)prof;
  if (imm_dp_unpack(&p->dp.null, file))
    return RC_EFAIL;
  if (imm_dp_unpack(&p->dp.alt, file))
    return RC_EFAIL;
  return RC_OK;
}

static struct imm_dp const *null_dp(struct prof const *prof) {
  struct std_prof *p = (struct std_prof *)prof;
  return &p->dp.null;
}

static struct imm_dp const *alt_dp(struct prof const *prof) {
  struct std_prof *p = (struct std_prof *)prof;
  return &p->dp.alt;
}

static struct prof_vtable vtable = {PROF_STD, del, read, null_dp, alt_dp};

void std_prof_init(struct std_prof *prof, char const *accession,
                   struct imm_code const *code) {
  imm_dp_init(&prof->dp.null, code);
  imm_dp_init(&prof->dp.alt, code);
  prof_init(&prof->super, accession, code, vtable, std_state_name);
}

enum rc std_prof_pack(struct std_prof const *prof, struct lip_file *file) {
  if (!lip_write_cstr(file, "null"))
    return eio("write null state key");
  if (imm_dp_pack(&prof->dp.null, file))
    return RC_EFAIL;

  if (!lip_write_cstr(file, "alt"))
    return eio("write alt state key");
  if (imm_dp_pack(&prof->dp.alt, file))
    return RC_EFAIL;

  return RC_OK;
}
