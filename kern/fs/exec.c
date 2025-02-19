#include "exec.h"
#include "kern/panic.h"
#include "kern/mem/pm.h"
#include "kern/module/io.h"
#include "kern/page/page_stuff.h"
#include "kern/sched/pcb.h"
#include "kern/sched/tasks.h"
#include "user/lib.h"

/**
 * @brief 将控制流切换至一个指定的二进制文件
 * @note 不会返回原控制流
 * @param filename 指定一个二进制文件
 */
void
exec(const char *filename) {
    if (filename == 0)    panic("exec(): null pointer");

    static char param[MAXSIZE_PATH], cmd[MAXSIZE_PATH];
    bzero(param, MAXSIZE_PATH);
    bzero(cmd, MAXSIZE_PATH);
    uint32_t flen = strlen(filename);
    // 判断参数
    int i = 0;
    for (; i < flen; ++i) {
        if (filename[i] == ' ') {
            memmove(cmd, filename, i);
            memmove(param, filename + i + 1, flen - i - 1);
            break;
        }
    }
    if (i == flen)    memmove(cmd, filename, flen);

    // 格式化文件名
    static char absolute_path[MAXSIZE_PATH * 2];
    bzero(absolute_path, MAXSIZE_PATH * 2);
    if (cmd[0] != '/') {
        uint32_t size = strlen(DIR_LOADER);
        memmove(absolute_path, DIR_LOADER, size);
        memmove(absolute_path + size, cmd, flen);
    } else    memmove(absolute_path, cmd, flen);

    // 处理 argc, argv (TODO: argv 需要支持更多参数)
    static char *argv[MAX_ARGV];
    bzero(argv, MAX_ARGV * sizeof(char *));
    argv[0] = absolute_path;
    argv[1] = param;
    uint32_t argc = param[0] == 0 ? 1 : 2;

    // 打开二进制文件
    fd_t fd = files_open(absolute_path);
    if (fd == -1) {
        kprintf("Command: \"%s\" not found\n", absolute_path);
        exit();
    }
    uint32_t file_size = files_get_size(fd);
    uint32_t file_pages = PGUP(file_size, PGSIZE);
    pcb_t *cur_pcb = get_current_pcb();
    // 要使用动态内存分配必须先赋值二进制边界
    cur_pcb->break_ = file_pages;

    // 设置页表
    uint32_t amount_pgdir = file_pages / MB4;
    if (file_pages % MB4)    ++amount_pgdir;
    uint32_t vaddr_program = 0;
    for (i = 0; i < amount_pgdir; ++i) {
        // 处理页目录表的 PDE
        void *pgtbl_pa = phy_alloc_page();
        pgelem_t flag = PGFLAG_US | PGFLAG_RW | PGFLAG_PS;
        pgelem_t *pde = (pgelem_t *)GET_PDE(i * MB4);
        *pde = (pgelem_t)pgtbl_pa | flag;

        // 处理页表的 PTE
        for (int j = 0; vaddr_program < file_pages && j < MB4;) {
            void *program_pa = phy_alloc_page();
            set_mapping((void *)vaddr_program, program_pa, flag);
            j += PGSIZE;
            vaddr_program += PGSIZE;
        } // end for(j)
    } // end for(i)

    // 将文件的数据读取到内存
    builtin_t program = (builtin_t)0;
    files_read(fd, program, file_size);

    // 切换控制流
    // 注意：再跳转 `mode_ring3()` 后会使用一个全新的 ring3 栈，
    //      所以在跳转之前 ring3 栈要先设置好
    __asm__ ("movl %0, %%eax\n\t"
        "movl %2, -0x4(%%eax)\n\t"
        "movl %3, -0x8(%%eax)\n\t"
        "movl %4, -0xc(%%eax)\n\t"
        "movl $next_insc, -0x10(%%eax)\n\t"
        "subl $0x10, %%eax\n\t"
        "pushl %1\n\t"
        "pushl %%eax\n\t"
        "jmp mode_ring3\n\t"
        "next_insc:\n\t"
        "addl $0x8, %%esp\n\t"
        "movl %%esp, %%ebp\n\t"
        "call sys_close\n\t"
        "call sys_exit"
        : : "c"(cur_pcb->stack3_), "d"(program), "b"(fd), "S"(argv), "D"(argc));
}
