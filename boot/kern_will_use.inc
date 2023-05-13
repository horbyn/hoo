#       Copyright (C)    horbyn, 2023
#             (hoRbyn4zZ@outlook.com)
#
# ##################################################################
# ############## the following layout used for boot ################
#                                                                  #
# 0x77ffc          0x78000   0x78200  0x78204  0x78400     0x80000 #
# ──────┼────────────────┼─────────┼────────┼────────┼───────────┤ #
#       │DIED INSTRUCTION    MBR   │ARDS NUM│  ARDS  │   STACK   │ #
# ──────┼────────────────┼─────────┼────────┼────────┼───────────┤ #
#                                                                  #
# ##################################################################

# kernel stack
#     Althrough it looks like the stack is set to 0x70000,
#     the sp(esp) is subtracted first you know.
#     So if sp(esp) was 0 and then push stack, you will get
#     ss:sp(esp) that 0x7000:0xfffe(0xfffc)
#
#     This is why 0x7000 below
.set SEG_KSTACK,  0x7000

# here sets the `jmp .` instruction
.set SEG_DIED,    0x77f0
.set OFF_DIED,    0xfc

# here record the ARDS struct temporary
.set SEG_ARDS,    0x7820
.set OFF_ARDS_CR, 0
.set OFF_ARDS,    4
