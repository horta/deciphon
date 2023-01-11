#ifndef EXPECT_H
#define EXPECT_H

#include "compiler.h"
#include "imm/float.h"
#include "lite_pack.h"
#include <stdbool.h>
#include <stdint.h>

#include <stdbool.h>
#include <stdint.h>

bool expect_map_size(struct lip_file *file, unsigned size);
bool expect_map_key(struct lip_file *file, char const key[]);

#endif
