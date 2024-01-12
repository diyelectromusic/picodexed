#ifndef _ARM_MATH_H
#define _ARM_MATH_H

#include <pico/float.h>

#ifdef   __cplusplus
extern "C"
{
#endif

typedef float float32_t;
typedef int16_t q15_t;
typedef int32_t q31_t;

#if   defined ( __CC_ARM )
  #define __ASM            __asm                                      /*!< asm keyword for ARM Compiler */
  #define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler */
  #define __STATIC_INLINE  static __inline

#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #define __ASM            __asm                                      /*!< asm keyword for ARM Compiler */
  #define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler */
  #define __STATIC_INLINE  static __inline

#elif defined ( __GNUC__ )
  #define __ASM            __asm                                      /*!< asm keyword for GNU Compiler */
  #define __INLINE         inline                                     /*!< inline keyword for GNU Compiler */
  #define __STATIC_INLINE  static inline

#else
  #error Unknown compiler
#endif

static __INLINE q31_t __SSAT(q31_t x, uint32_t y) {
  int32_t posMax, negMin;
  uint32_t i;

  posMax = 1;
  for (i = 0; i < (y - 1); i++) {
    posMax = posMax * 2;
  }

  if(x > 0) {
    posMax = (posMax - 1);

    if(x > posMax) {
      x = posMax;
    }
  }
  else {
    negMin = -posMax;

    if(x < negMin) {
      x = negMin;
    }
  }
  return (x);
}

void arm_float_to_q15(
  const float32_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);

void arm_fill_f32(
  float32_t value,
  float32_t * pDst,
  uint32_t blockSize);

void arm_sub_f32(
  const float32_t * pSrcA,
  const float32_t * pSrcB,
  float32_t * pDst,
  uint32_t blockSize);

void arm_scale_f32(
  const float32_t * pSrc,
  float32_t scale,
  float32_t * pDst,
  uint32_t blockSize);

void arm_offset_f32(
  const float32_t * pSrc,
  float32_t offset,
  float32_t * pDst,
  uint32_t blockSize);

void arm_mult_f32(
  const float32_t * pSrcA,
  const float32_t * pSrcB,
  float32_t * pDst,
  uint32_t blockSize);

typedef struct
{
  uint32_t numStages;      /**< number of 2nd order stages in the filter.  Overall order is 2*numStages. */
  float32_t *pState;       /**< Points to the array of state coefficients.  The array is of length 4*numStages. */
  float32_t *pCoeffs;      /**< Points to the array of coefficients.  The array is of length 5*numStages. */
} arm_biquad_casd_df1_inst_f32;

void arm_biquad_cascade_df1_f32(
  const arm_biquad_casd_df1_inst_f32 * S,
  const float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);

void arm_biquad_cascade_df1_init_f32(
  arm_biquad_casd_df1_inst_f32 * S,
  uint8_t numStages,
  const float32_t * pCoeffs,
  float32_t * pState);

  
#ifdef   __cplusplus
}
#endif

#endif
