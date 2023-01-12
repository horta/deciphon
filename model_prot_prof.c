#include "model_prot_prof.h"
#include "expect.h"
#include "imm/imm.h"
#include "lite_pack/file/file.h"
#include "lite_pack/lite_pack.h"
#include "logy.h"
#include "model_prof.h"
#include "model_prof_typeid.h"
#include "model_prot_model.h"
#include "model_prot_prof.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void del(struct prof *prof) {
  if (prof) {
    struct prot_prof *p = (struct prot_prof *)prof;
    free(p->alt.match_ndists);
    imm_del(&p->null.dp);
    imm_del(&p->alt.dp);
  }
}

static enum rc alloc_match_nuclt_dists(struct prot_prof *prof) {
  size_t size = prof->core_size * sizeof *prof->alt.match_ndists;
  void *ptr = realloc(prof->alt.match_ndists, size);
  if (!ptr && size > 0) {
    free(prof->alt.match_ndists);
    return enomem("alloc nuclt dists");
  }
  prof->alt.match_ndists = ptr;
  return RC_OK;
}

static enum rc unpack(struct prof *prof, struct lip_file *file) {
  struct prot_prof *p = (struct prot_prof *)prof;
  unsigned size = 0;
  if (!lip_read_map_size(file, &size))
    return RC_EFREAD;
  assert(size == 16);

  int rc = 0;

  if ((rc = expect_map_key(file, "accession")))
    return rc;
  if (!lip_read_cstr(file, PROFILE_ACC_SIZE, prof->accession))
    return RC_EFREAD;

  if ((rc = expect_map_key(file, "null")))
    return rc;
  if (imm_dp_unpack(&p->null.dp, file))
    return RC_EFAIL;

  if ((rc = expect_map_key(file, "alt")))
    return rc;
  if (imm_dp_unpack(&p->alt.dp, file))
    return RC_EFAIL;

  if ((rc = expect_map_key(file, "core_size")))
    return rc;
  if (!lip_read_int(file, &size))
    return RC_EFREAD;
  if (size > PROT_MODEL_CORE_SIZE_MAX)
    return RC_ELARGEPROFILE;
  p->core_size = size;

  if ((rc = expect_map_key(file, "consensus")))
    return rc;
  size = p->core_size;
  if (!lip_read_cstr(file, PROT_MODEL_CORE_SIZE_MAX, p->consensus))
    return RC_EFREAD;

  unsigned s = 0;

  if ((rc = expect_map_key(file, "R")))
    return rc;
  if (!lip_read_int(file, &s))
    return RC_EFREAD;
  p->null.R = (unsigned)s;

  if ((rc = expect_map_key(file, "S")))
    return rc;
  if (!lip_read_int(file, &s))
    return RC_EFREAD;
  p->alt.S = (unsigned)s;

  if ((rc = expect_map_key(file, "N")))
    return rc;
  if (!lip_read_int(file, &s))
    return RC_EFREAD;
  p->alt.N = (unsigned)s;

  if ((rc = expect_map_key(file, "B")))
    return rc;
  if (!lip_read_int(file, &s))
    return RC_EFREAD;
  p->alt.B = (unsigned)s;

  if ((rc = expect_map_key(file, "E")))
    return rc;
  if (!lip_read_int(file, &s))
    return RC_EFREAD;
  p->alt.E = (unsigned)s;

  if ((rc = expect_map_key(file, "J")))
    return rc;
  if (!lip_read_int(file, &s))
    return RC_EFREAD;
  p->alt.J = (unsigned)s;

  if ((rc = expect_map_key(file, "C")))
    return rc;
  if (!lip_read_int(file, &s))
    return RC_EFREAD;
  p->alt.C = (unsigned)s;

  if ((rc = expect_map_key(file, "T")))
    return rc;
  if (!lip_read_int(file, &s))
    return RC_EFREAD;
  p->alt.T = (unsigned)s;

  rc = alloc_match_nuclt_dists(p);
  if (rc)
    return rc;

  if ((rc = expect_map_key(file, "null_ndist")))
    return rc;
  if ((rc = nuclt_dist_unpack(&p->null.ndist, file)))
    return rc;

  if ((rc = expect_map_key(file, "alt_insert_ndist")))
    return rc;
  if ((rc = nuclt_dist_unpack(&p->alt.insert_ndist, file)))
    return rc;

  if ((rc = expect_map_key(file, "alt_match_ndist")))
    return rc;
  if (!lip_read_array_size(file, &size))
    return RC_EFREAD;
  assert(size == p->core_size);
  for (unsigned i = 0; i < p->core_size; ++i) {
    if ((rc = nuclt_dist_unpack(p->alt.match_ndists + i, file)))
      return rc;
    nuclt_dist_init(p->alt.match_ndists + i, p->code->nuclt);
  }
  return RC_OK;
}

static struct imm_dp const *null_dp(struct prof const *prof) {
  struct prot_prof *p = (struct prot_prof *)prof;
  return &p->null.dp;
}

static struct imm_dp const *alt_dp(struct prof const *prof) {
  struct prot_prof *p = (struct prot_prof *)prof;
  return &p->alt.dp;
}

static struct prof_vtable vtable = {PROF_PROT, del, unpack, null_dp, alt_dp};

void prot_prof_init(struct prot_prof *p, char const *accession,
                    struct imm_amino const *amino,
                    struct imm_nuclt_code const *code, struct prot_cfg cfg) {
  struct imm_nuclt const *nuclt = code->nuclt;
  prof_init(&p->super, accession, &code->super, vtable, prot_state_name);
  p->code = code;
  p->amino = amino;
  p->cfg = cfg;
  p->eps = imm_frame_epsilon(cfg.epsilon);
  p->core_size = 0;
  p->consensus[0] = '\0';
  imm_dp_init(&p->null.dp, &code->super);
  imm_dp_init(&p->alt.dp, &code->super);
  nuclt_dist_init(&p->null.ndist, nuclt);
  nuclt_dist_init(&p->alt.insert_ndist, nuclt);
  p->alt.match_ndists = NULL;
}

enum rc prot_prof_setup(struct prot_prof *prof, unsigned seq_size,
                        bool multi_hits, bool hmmer3_compat) {
  if (seq_size == 0)
    return einval("sequence cannot be empty");

  imm_float L = (imm_float)seq_size;

  imm_float q = 0.0;
  imm_float log_q = IMM_LPROB_ZERO;

  if (multi_hits) {
    q = 0.5;
    log_q = imm_log(0.5);
  }

  imm_float lp = imm_log(L) - imm_log(L + 2 + q / (1 - q));
  imm_float l1p = imm_log(2 + q / (1 - q)) - imm_log(L + 2 + q / (1 - q));
  imm_float lr = imm_log(L) - imm_log(L + 1);

  struct prot_xtrans t;

  t.NN = t.CC = t.JJ = lp;
  t.NB = t.CT = t.JB = l1p;
  t.RR = lr;
  t.EJ = log_q;
  t.EC = imm_log(1 - q);

  if (hmmer3_compat) {
    t.NN = t.CC = t.JJ = imm_log(1);
  }

  struct imm_dp *dp = &prof->null.dp;
  unsigned R = prof->null.R;
  imm_dp_change_trans(dp, imm_dp_trans_idx(dp, R, R), t.RR);

  dp = &prof->alt.dp;
  unsigned S = prof->alt.S;
  unsigned N = prof->alt.N;
  unsigned B = prof->alt.B;
  unsigned E = prof->alt.E;
  unsigned J = prof->alt.J;
  unsigned C = prof->alt.C;
  unsigned T = prof->alt.T;

  imm_dp_change_trans(dp, imm_dp_trans_idx(dp, S, B), t.NB);
  imm_dp_change_trans(dp, imm_dp_trans_idx(dp, S, N), t.NN);
  imm_dp_change_trans(dp, imm_dp_trans_idx(dp, N, N), t.NN);
  imm_dp_change_trans(dp, imm_dp_trans_idx(dp, N, B), t.NB);

  imm_dp_change_trans(dp, imm_dp_trans_idx(dp, E, T), t.EC + t.CT);
  imm_dp_change_trans(dp, imm_dp_trans_idx(dp, E, C), t.EC + t.CC);
  imm_dp_change_trans(dp, imm_dp_trans_idx(dp, C, C), t.CC);
  imm_dp_change_trans(dp, imm_dp_trans_idx(dp, C, T), t.CT);

  imm_dp_change_trans(dp, imm_dp_trans_idx(dp, E, B), t.EJ + t.JB);
  imm_dp_change_trans(dp, imm_dp_trans_idx(dp, E, J), t.EJ + t.JJ);
  imm_dp_change_trans(dp, imm_dp_trans_idx(dp, J, J), t.JJ);
  imm_dp_change_trans(dp, imm_dp_trans_idx(dp, J, B), t.JB);
  return RC_OK;
}

enum rc prot_prof_absorb(struct prot_prof *p, struct prot_model const *m) {
  if (p->code->nuclt != prot_model_nuclt(m))
    return einval("Different nucleotide alphabets.");

  if (p->amino != prot_model_amino(m))
    return einval("Different amino alphabets.");

  struct prot_model_summary s = prot_model_summary(m);

  if (imm_hmm_reset_dp(s.null.hmm, imm_super(s.null.R), &p->null.dp))
    return efail("failed to hmm_reset");

  if (imm_hmm_reset_dp(s.alt.hmm, imm_super(s.alt.T), &p->alt.dp))
    return efail("failed to hmm_reset");

  p->core_size = m->core_size;
  memcpy(p->consensus, m->consensus, m->core_size + 1);
  enum rc rc = alloc_match_nuclt_dists(p);
  if (rc)
    return rc;

  p->null.ndist = m->null.nucltd;

  for (unsigned i = 0; i < m->core_size; ++i)
    p->alt.match_ndists[i] = m->alt.nodes[i].match.nucltd;

  p->alt.insert_ndist = m->alt.insert.nucltd;

  p->null.R = imm_state_idx(imm_super(s.null.R));

  p->alt.S = imm_state_idx(imm_super(s.alt.S));
  p->alt.N = imm_state_idx(imm_super(s.alt.N));
  p->alt.B = imm_state_idx(imm_super(s.alt.B));
  p->alt.E = imm_state_idx(imm_super(s.alt.E));
  p->alt.J = imm_state_idx(imm_super(s.alt.J));
  p->alt.C = imm_state_idx(imm_super(s.alt.C));
  p->alt.T = imm_state_idx(imm_super(s.alt.T));
  return RC_OK;
}

enum rc prot_prof_sample(struct prot_prof *p, unsigned seed,
                         unsigned core_size) {
  assert(core_size >= 2);
  p->core_size = core_size;
  struct imm_rnd rnd = imm_rnd(seed);

  imm_float lprobs[IMM_AMINO_SIZE];

  imm_lprob_sample(&rnd, IMM_AMINO_SIZE, lprobs);
  imm_lprob_normalize(IMM_AMINO_SIZE, lprobs);

  struct prot_model model;
  prot_model_init(&model, p->amino, p->code, p->cfg, lprobs);

  enum rc rc = RC_OK;

  if ((rc = prot_model_setup(&model, core_size)))
    goto cleanup;

  for (unsigned i = 0; i < core_size; ++i) {
    imm_lprob_sample(&rnd, IMM_AMINO_SIZE, lprobs);
    imm_lprob_normalize(IMM_AMINO_SIZE, lprobs);
    if ((rc = prot_model_add_node(&model, lprobs, '-')))
      goto cleanup;
  }

  for (unsigned i = 0; i < core_size + 1; ++i) {
    struct prot_trans t;
    imm_lprob_sample(&rnd, PROT_TRANS_SIZE, t.data);
    if (i == 0)
      t.DD = IMM_LPROB_ZERO;
    if (i == core_size) {
      t.MD = IMM_LPROB_ZERO;
      t.DD = IMM_LPROB_ZERO;
    }
    imm_lprob_normalize(PROT_TRANS_SIZE, t.data);
    if ((rc = prot_model_add_trans(&model, t)))
      goto cleanup;
  }

  rc = prot_prof_absorb(p, &model);

cleanup:
  prot_model_del(&model);
  return rc;
}

enum rc prot_prof_decode(struct prot_prof const *prof,
                         struct imm_seq const *seq, unsigned state_id,
                         struct imm_codon *codon) {
  assert(!prot_state_is_mute(state_id));

  struct nuclt_dist const *nucltd = NULL;
  if (prot_state_is_insert(state_id)) {
    nucltd = &prof->alt.insert_ndist;
  } else if (prot_state_is_match(state_id)) {
    unsigned idx = prot_state_idx(state_id);
    nucltd = prof->alt.match_ndists + idx;
  } else
    nucltd = &prof->null.ndist;

  struct imm_frame_cond cond = {prof->eps, &nucltd->nucltp, &nucltd->codonm};

  if (imm_lprob_is_nan(imm_frame_cond_decode(&cond, seq, codon)))
    return einval("failed to decode sequence");

  return RC_OK;
}

void prot_prof_write_dot(struct prot_prof const *p, FILE *fp) {
  imm_dp_write_dot(&p->alt.dp, fp, prot_state_name);
}

enum rc prot_prof_pack(struct prot_prof const *prof, struct lip_file *file) {
  if (!lip_write_map_size(file, 16))
    return RC_EFWRITE;

  if (!lip_write_cstr(file, "accession"))
    return RC_EFWRITE;
  if (!lip_write_cstr(file, prof->super.accession))
    return RC_EFWRITE;

  if (!lip_write_cstr(file, "null"))
    return RC_EFWRITE;
  if (imm_dp_pack(&prof->null.dp, file))
    return RC_EFAIL;

  if (!lip_write_cstr(file, "alt"))
    return RC_EFWRITE;
  if (imm_dp_pack(&prof->alt.dp, file))
    return RC_EFAIL;

  if (!lip_write_cstr(file, "core_size"))
    return RC_EFWRITE;
  if (!lip_write_int(file, prof->core_size))
    return RC_EFWRITE;

  if (!lip_write_cstr(file, "consensus"))
    return RC_EFWRITE;
  if (!lip_write_cstr(file, prof->consensus))
    return RC_EFWRITE;

  if (!lip_write_cstr(file, "R"))
    return RC_EFWRITE;
  if (!lip_write_int(file, prof->null.R))
    return RC_EFWRITE;

  if (!lip_write_cstr(file, "S"))
    return RC_EFWRITE;
  if (!lip_write_int(file, prof->alt.S))
    return RC_EFWRITE;

  if (!lip_write_cstr(file, "N"))
    return RC_EFWRITE;
  if (!lip_write_int(file, prof->alt.N))
    return RC_EFWRITE;

  if (!lip_write_cstr(file, "B"))
    return RC_EFWRITE;
  if (!lip_write_int(file, prof->alt.B))
    return RC_EFWRITE;

  if (!lip_write_cstr(file, "E"))
    return RC_EFWRITE;
  if (!lip_write_int(file, prof->alt.E))
    return RC_EFWRITE;

  if (!lip_write_cstr(file, "J"))
    return RC_EFWRITE;
  if (!lip_write_int(file, prof->alt.J))
    return RC_EFWRITE;

  if (!lip_write_cstr(file, "C"))
    return RC_EFWRITE;
  if (!lip_write_int(file, prof->alt.C))
    return RC_EFWRITE;

  if (!lip_write_cstr(file, "T"))
    return RC_EFWRITE;
  if (!lip_write_int(file, prof->alt.T))
    return RC_EFWRITE;

  if (!lip_write_cstr(file, "null_ndist"))
    return RC_EFWRITE;
  enum rc rc = nuclt_dist_pack(&prof->null.ndist, file);
  if (rc)
    return rc;

  if (!lip_write_cstr(file, "alt_insert_ndist"))
    return RC_EFWRITE;
  if ((rc = nuclt_dist_pack(&prof->alt.insert_ndist, file)))
    return rc;

  if (!lip_write_cstr(file, "alt_match_ndist"))
    return RC_EFWRITE;
  if (!lip_write_array_size(file, prof->core_size))
    return RC_EFWRITE;
  for (unsigned i = 0; i < prof->core_size; ++i) {
    if ((rc = nuclt_dist_pack(prof->alt.match_ndists + i, file)))
      return rc;
  }
  return RC_OK;
}
