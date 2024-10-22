/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "bucket.h"

/**
 * @brief bucket manager initialization
 * 
 * @param mngr bucket manager
 * @param size bucket size
 * @param chain bucket chain
 * @param next next bucket manager
 */
void
buckmngr_init(buckx_mngr_t *mngr, uint32_t size, fmtlist_t *chain,
buckx_mngr_t *next) {
    if (mngr == null)    panic("buckmngr_init(): null pointer");
    mngr->size_ = size;
    mngr->chain_ = chain;
    mngr->next_ = next;
}
