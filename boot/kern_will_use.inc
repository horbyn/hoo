
# ##################################################################
# ############## the following layout used for boot ################
#                                                                  #
# 0x77ffc                    0x78200  0x78204  0x78400     0x80000 #
# ──────┼────────────────┼─────────┼────────┼────────┼───────────┤ #
#       │DIED INSTRUCTION    MBR   │ARDS NUM│  ARDS  │   STACK   │ #
# ──────┼────────────────┼─────────┼────────┼────────┼───────────┤ #
#                                                                  #
# ##################################################################

# kernel stack
.set SEG_KSTACK,  0x8000

# here sets the `jmp .` instruction
.set SEG_DIED,    0x77f0
.set OFF_DIED,    0xe0

# here record the ARDS struct temporary
.set SEG_ARDS,    0x7820
.set OFF_ARDS_CR, 0
.set OFF_ARDS,    4

