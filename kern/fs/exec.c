/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "exec.h"

/**
 * @brief change the control flow to execute the specific program
 * 
 * @param filename the specific program filename
 * @retval 0: success
 * @retval -1: command not found
 */
int
exec(const char *filename) {
    if (filename == null)
        panic("exec(): null pointer");

    // formatting the filename
    static char absolute_path[MAXSIZE_PATH];
    bzero(absolute_path, MAXSIZE_PATH);
    if (filename[0] != '/') {
        uint32_t size = strlen(DIR_LOADER);
        memmove(absolute_path, DIR_LOADER, size);
        memmove(absolute_path + size, filename, strlen(filename));
    } else
        memmove(absolute_path, filename, strlen(filename));

    fd_t fd = files_open(absolute_path);
    if (fd == -1)    return -1;
    uint32_t file_size = PGUP(files_get_size(fd), PGSIZE);
    pcb_t *cur_pcb = get_current_pcb();
    cur_pcb->break_ = file_size;

    // setup page tables
    uint32_t amount_pgdir = file_size / MB4;
    if (file_size % MB4)    ++amount_pgdir;
    uint32_t vaddr_program = 0;
    for (uint32_t i = 0; i < amount_pgdir; ++i) {
        // handle the page directory
        void *pgtbl_pa = phy_alloc_page();
        pgelem_t flag = PGENT_US | PGENT_RW | PGENT_PS;
        pgelem_t *pde = (pgelem_t *)GET_PDE(i * MB4);
        *pde = (pgelem_t)pgtbl_pa | flag;

        // handle the page table
        for (uint32_t j = 0; vaddr_program < file_size && j < MB4;) {
            void *program_pa = phy_alloc_page();
            set_mapping((void *)vaddr_program, program_pa, flag);
            j += PGSIZE;
            vaddr_program += PGSIZE;
        } // end for(j)
    } // end for(i)

    // data reading from the file to memory
    builtin_t program = 0;
    files_read(fd, (char *)program, file_size);

    // change the control flows in ring3
    __asm__ ("pushl %1\n\t"
        "pushl %0\n\t"
        "call mode_ring3" : : "r"(cur_pcb->stack3_), "r"(program));

    files_close(fd);
    return 0;
}
