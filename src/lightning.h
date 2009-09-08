#ifndef __lightning_h
#define __lightning_h

#ifdef __cplusplus
extern "C" {
#endif

#include "asm-common.h"

#ifndef LIGHTNING_DEBUG
#include "asm.h"
#endif

#include "core.h"
#include "core-common.h"
#include "funcs-common.h"
#include "funcs.h"
#include "fp.h"
#include "fp-common.h"

#ifndef JIT_R0
#error GNU lightning does not support the current target
#endif

#ifdef __cplusplus
}
#endif

#endif /* __lightning_h */
