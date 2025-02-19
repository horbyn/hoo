#include "bucket.h"
#include "kern/panic.h"

/**
 * @brief bucket 管理器初始化
 * 
 * @param mngr  bucket 管理器
 * @param size  bucket 大小
 * @param chain bucket 链
 * @param next  下一个 bucket 管理器
 */
void
buckmngr_init(buckx_mngr_t *mngr, uint32_t size, fmtlist_t *chain,
buckx_mngr_t *next) {
    if (mngr == null)    panic("buckmngr_init(): null pointer");
    mngr->size_ = size;
    mngr->chain_ = chain;
    mngr->next_ = next;
}
