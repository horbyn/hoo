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
    uint32_t file_size = files_get_size(fd);
    uint32_t file_pages = PGUP(file_size, PGSIZE);
    pcb_t *cur_pcb = get_current_pcb();
    cur_pcb->break_ += file_pages;

    // setup page tables
    uint32_t amount_pgdir = file_pages / MB4;
    if (file_pages % MB4)    ++amount_pgdir;
    uint32_t vaddr_program = VIR_BASE_IDLE;
    for (uint32_t i = 0; i < amount_pgdir; ++i) {
        // handle the page directory
        void *pgtbl_pa = phy_alloc_page();
        pgelem_t flag = PGENT_US | PGENT_RW | PGENT_PS;
        pgelem_t *pde = (pgelem_t *)GET_PDE(i * MB4);
        *pde = (pgelem_t)pgtbl_pa | flag;

        // the first page table we start at 0x1000, the rests start at 0
        uint32_t j = i == 0 ? VIR_BASE_IDLE : 0;

        // handle the page table
        for (; vaddr_program < (VIR_BASE_IDLE + file_pages) && j < MB4;) {
            void *program_pa = phy_alloc_page();
            set_mapping((void *)vaddr_program, program_pa, flag);
            j += PGSIZE;
            vaddr_program += PGSIZE;
        } // end for(j)
    } // end for(i)

    // data reading from the file to memory
    builtin_t program = (builtin_t)VIR_BASE_IDLE;
    files_read(fd, (char *)program, file_size);

    // change the control flows in ring3
    __asm__ ("pushl %1\n\t"
        "pushl %0\n\t"
        "jmp mode_ring3" : : "r"(cur_pcb->stack3_), "r"(program));

    files_close(fd);
    return 0;
}
