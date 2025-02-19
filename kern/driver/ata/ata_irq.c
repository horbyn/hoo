#include "ata_irq.h"
#include "kern/panic.h"
#include "kern/dyn/dynamic.h"
#include "kern/sched/pcb.h"
#include "kern/sched/tasks.h"
#include "kern/utilities/queue.h"
#include "kern/utilities/spinlock.h"
#include "user/lib.h"

static node_t *__mdata_node;
// 表示 FIFO 磁盘请求
static queue_t __queue_ata;
static spinlock_t __slqueue, __slata;

/**
 * @brief IRQ 方式的 ATA 初始化
 */
void
ata_irq_init() {
    __mdata_node = dyn_alloc(MAX_TASKS_AMOUNT * sizeof(node_t));
    bzero(__mdata_node, sizeof(__mdata_node));
    queue_init(&__queue_ata);
    spinlock_init(&__slqueue);
    spinlock_init(&__slata);
}

/**
 * @brief ATA ISR
 */
void
ata_irq_intr() {

    // 1. 检查队头
    wait(&__slqueue);
    node_t *done = queue_pop(&__queue_ata);
    signal(&__slqueue);
    if (done != null) {
        atabuff_t *buff = done->data_;
        if (!buff)    panic("ata_irq_intr(): idebuff not set");

        if (buff->cmd_ == ATA_CMD_IO_READ) {
            ataspc_t *ata_space = get_ataspace();
            insw(buff->buff_, buff->len_ / sizeof(uint16_t),
                ata_space->device_info_[ata_space->current_select_].io_port_
                + ATA_IO_RW_OFFSET_DATA);
        }
        buff->finish_ = true;
    }

    // 2. 唤醒睡眠进程
    wakeup(done);
}

/**
 * @brief IRQ 方式的 ATA 读写
 * 
 * @param buff ATA buffer
 */
void
ata_irq_rw(atabuff_t *buff) {
    if (buff == null)    panic("ata_irq_rw(): null pointer");
    pcb_t *cur_pcb = get_current_pcb();
    ataspc_t *ata_space = get_ataspace();

    // 1. 缓存 buffer
    node_t *node = __mdata_node + cur_pcb->tid_;
    bzero(node, sizeof(node_t));
    node->data_ = buff;
    wait(&__slqueue);
    queue_push(&__queue_ata, node, TAIL);
    signal(&__slqueue);

    // 2. 设置寄存器
    ata_set_cmd(ata_space->current_select_, buff->lba_,
        buff->len_ / BYTES_SECTOR, buff->cmd_);
    if (buff->cmd_ == ATA_CMD_IO_WRITE)
        outsw(buff->buff_, buff->len_ / sizeof(uint16_t),
            ata_space->device_info_[ata_space->current_select_].io_port_
            + ATA_IO_RW_OFFSET_DATA);

    // 3. 进入睡眠
    wait(&__slata);
    while (buff->finish_ == false)
        sleep(node, &__slata);
    signal(&__slata);
}
