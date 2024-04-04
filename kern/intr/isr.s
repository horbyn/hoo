/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/

    .text
    .code32
    .globl isr_part1
    .extern isr_part2

.macro ISRNOERR id
isr_part1_\id:
    pushl $0
    pushl $\id
    jmp   isr_part2
.endm

.macro ISRERR id
isr_part1_\id:
    pushl $\id
    jmp   isr_part2
.endm

ISRNOERR 0
ISRNOERR 1
ISRNOERR 2
ISRNOERR 3
ISRNOERR 4
ISRNOERR 5
ISRNOERR 6
ISRNOERR 7
ISRERR   8
ISRNOERR 9
ISRERR   10
ISRERR   11
ISRERR   12
ISRERR   13
ISRERR   14
ISRNOERR 15
ISRNOERR 16
ISRERR   17
ISRNOERR 18
ISRNOERR 19
ISRNOERR 20
ISRNOERR 21
ISRNOERR 22
ISRNOERR 23
ISRNOERR 24
ISRNOERR 25
ISRNOERR 26
ISRNOERR 27
ISRNOERR 28
ISRNOERR 29
ISRNOERR 30
ISRNOERR 31
ISRNOERR 32
ISRNOERR 33
ISRNOERR 34
ISRNOERR 35
ISRNOERR 36
ISRNOERR 37
ISRNOERR 38
ISRNOERR 39
ISRNOERR 40
ISRNOERR 41
ISRNOERR 42
ISRNOERR 43
ISRNOERR 44
ISRNOERR 45
ISRNOERR 46
ISRNOERR 47
ISRNOERR 48
ISRNOERR 49
ISRNOERR 50
ISRNOERR 51
ISRNOERR 52
ISRNOERR 53
ISRNOERR 54
ISRNOERR 55
ISRNOERR 56
ISRNOERR 57
ISRNOERR 58
ISRNOERR 59
ISRNOERR 60
ISRNOERR 61
ISRNOERR 62
ISRNOERR 63
ISRNOERR 64
ISRNOERR 65
ISRNOERR 66
ISRNOERR 67
ISRNOERR 68
ISRNOERR 69
ISRNOERR 70
ISRNOERR 71
ISRNOERR 72
ISRNOERR 73
ISRNOERR 74
ISRNOERR 75
ISRNOERR 76
ISRNOERR 77
ISRNOERR 78
ISRNOERR 79
ISRNOERR 80
ISRNOERR 81
ISRNOERR 82
ISRNOERR 83
ISRNOERR 84
ISRNOERR 85
ISRNOERR 86
ISRNOERR 87
ISRNOERR 88
ISRNOERR 89
ISRNOERR 90
ISRNOERR 91
ISRNOERR 92
ISRNOERR 93
ISRNOERR 94
ISRNOERR 95
ISRNOERR 96
ISRNOERR 97
ISRNOERR 98
ISRNOERR 99
ISRNOERR 100
ISRNOERR 101
ISRNOERR 102
ISRNOERR 103
ISRNOERR 104
ISRNOERR 105
ISRNOERR 106
ISRNOERR 107
ISRNOERR 108
ISRNOERR 109
ISRNOERR 110
ISRNOERR 111
ISRNOERR 112
ISRNOERR 113
ISRNOERR 114
ISRNOERR 115
ISRNOERR 116
ISRNOERR 117
ISRNOERR 118
ISRNOERR 119
ISRNOERR 120
ISRNOERR 121
ISRNOERR 122
ISRNOERR 123
ISRNOERR 124
ISRNOERR 125
ISRNOERR 126
ISRNOERR 127
ISRNOERR 128
ISRNOERR 129
ISRNOERR 130
ISRNOERR 131
ISRNOERR 132
ISRNOERR 133
ISRNOERR 134
ISRNOERR 135
ISRNOERR 136
ISRNOERR 137
ISRNOERR 138
ISRNOERR 139
ISRNOERR 140
ISRNOERR 141
ISRNOERR 142
ISRNOERR 143
ISRNOERR 144
ISRNOERR 145
ISRNOERR 146
ISRNOERR 147
ISRNOERR 148
ISRNOERR 149
ISRNOERR 150
ISRNOERR 151
ISRNOERR 152
ISRNOERR 153
ISRNOERR 154
ISRNOERR 155
ISRNOERR 156
ISRNOERR 157
ISRNOERR 158
ISRNOERR 159
ISRNOERR 160
ISRNOERR 161
ISRNOERR 162
ISRNOERR 163
ISRNOERR 164
ISRNOERR 165
ISRNOERR 166
ISRNOERR 167
ISRNOERR 168
ISRNOERR 169
ISRNOERR 170
ISRNOERR 171
ISRNOERR 172
ISRNOERR 173
ISRNOERR 174
ISRNOERR 175
ISRNOERR 176
ISRNOERR 177
ISRNOERR 178
ISRNOERR 179
ISRNOERR 180
ISRNOERR 181
ISRNOERR 182
ISRNOERR 183
ISRNOERR 184
ISRNOERR 185
ISRNOERR 186
ISRNOERR 187
ISRNOERR 188
ISRNOERR 189
ISRNOERR 190
ISRNOERR 191
ISRNOERR 192
ISRNOERR 193
ISRNOERR 194
ISRNOERR 195
ISRNOERR 196
ISRNOERR 197
ISRNOERR 198
ISRNOERR 199
ISRNOERR 200
ISRNOERR 201
ISRNOERR 202
ISRNOERR 203
ISRNOERR 204
ISRNOERR 205
ISRNOERR 206
ISRNOERR 207
ISRNOERR 208
ISRNOERR 209
ISRNOERR 210
ISRNOERR 211
ISRNOERR 212
ISRNOERR 213
ISRNOERR 214
ISRNOERR 215
ISRNOERR 216
ISRNOERR 217
ISRNOERR 218
ISRNOERR 219
ISRNOERR 220
ISRNOERR 221
ISRNOERR 222
ISRNOERR 223
ISRNOERR 224
ISRNOERR 225
ISRNOERR 226
ISRNOERR 227
ISRNOERR 228
ISRNOERR 229
ISRNOERR 230
ISRNOERR 231
ISRNOERR 232
ISRNOERR 233
ISRNOERR 234
ISRNOERR 235
ISRNOERR 236
ISRNOERR 237
ISRNOERR 238
ISRNOERR 239
ISRNOERR 240
ISRNOERR 241
ISRNOERR 242
ISRNOERR 243
ISRNOERR 244
ISRNOERR 245
ISRNOERR 246
ISRNOERR 247
ISRNOERR 248
ISRNOERR 249
ISRNOERR 250
ISRNOERR 251
ISRNOERR 252
ISRNOERR 253
ISRNOERR 254
ISRNOERR 255

# isr array (256)
isr_part1:
    .long isr_part1_0,  isr_part1_1,  isr_part1_2
    .long isr_part1_3,  isr_part1_4,  isr_part1_5
    .long isr_part1_6,  isr_part1_7,  isr_part1_8
    .long isr_part1_9,  isr_part1_10, isr_part1_11
    .long isr_part1_12, isr_part1_13, isr_part1_14
    .long isr_part1_15, isr_part1_16, isr_part1_17
    .long isr_part1_18, isr_part1_19, isr_part1_20
    .long isr_part1_21, isr_part1_22, isr_part1_23
    .long isr_part1_24, isr_part1_25, isr_part1_26
    .long isr_part1_27, isr_part1_28, isr_part1_29
    .long isr_part1_30, isr_part1_31
    .long isr_part1_32, isr_part1_33    # timer, keyboard
    .long isr_part1_34, isr_part1_35    # cascade, com2
    .long isr_part1_36, isr_part1_37    # com1, lpt2
    .long isr_part1_38, isr_part1_38    # floppy, lpt1
    .long isr_part1_40, isr_part1_41    # cmos, acpi
    .long isr_part1_42, isr_part1_43    # scsi nic1, scsi nic2
    .long isr_part1_44, isr_part1_45    # mouse, math
    .long isr_part1_46, isr_part1_47    # ata1, ata2
    .long isr_part1_48                  # default
    .long isr_part1_49, isr_part1_50, isr_part1_51
    .long isr_part1_52, isr_part1_53, isr_part1_54
    .long isr_part1_55, isr_part1_56, isr_part1_57
    .long isr_part1_58, isr_part1_59, isr_part1_60
    .long isr_part1_61, isr_part1_62, isr_part1_63
    .long isr_part1_64, isr_part1_65, isr_part1_66
    .long isr_part1_67, isr_part1_68, isr_part1_69
    .long isr_part1_70, isr_part1_71, isr_part1_72
    .long isr_part1_73, isr_part1_74, isr_part1_75
    .long isr_part1_76, isr_part1_77, isr_part1_78
    .long isr_part1_79, isr_part1_80, isr_part1_81
    .long isr_part1_82, isr_part1_83, isr_part1_84
    .long isr_part1_85, isr_part1_86, isr_part1_87
    .long isr_part1_88, isr_part1_89, isr_part1_90
    .long isr_part1_91, isr_part1_92, isr_part1_93
    .long isr_part1_94, isr_part1_95, isr_part1_96
    .long isr_part1_97, isr_part1_98, isr_part1_99
    .long isr_part1_100, isr_part1_101, isr_part1_102
    .long isr_part1_103, isr_part1_104, isr_part1_105
    .long isr_part1_106, isr_part1_107, isr_part1_108
    .long isr_part1_109, isr_part1_110, isr_part1_111
    .long isr_part1_112, isr_part1_113, isr_part1_114
    .long isr_part1_115, isr_part1_116, isr_part1_117
    .long isr_part1_118, isr_part1_119, isr_part1_120
    .long isr_part1_121, isr_part1_122, isr_part1_123
    .long isr_part1_124, isr_part1_125, isr_part1_126
    .long isr_part1_127
    .long isr_part1_128 # #128 system call
    .long isr_part1_129, isr_part1_130, isr_part1_131
    .long isr_part1_132, isr_part1_133, isr_part1_134
    .long isr_part1_135, isr_part1_136, isr_part1_137
    .long isr_part1_138, isr_part1_139, isr_part1_140
    .long isr_part1_141, isr_part1_142, isr_part1_143
    .long isr_part1_144, isr_part1_145, isr_part1_146
    .long isr_part1_147, isr_part1_148, isr_part1_149
    .long isr_part1_150, isr_part1_151, isr_part1_152
    .long isr_part1_153, isr_part1_154, isr_part1_155
    .long isr_part1_156, isr_part1_157, isr_part1_158
    .long isr_part1_159, isr_part1_160, isr_part1_161
    .long isr_part1_162, isr_part1_163, isr_part1_164
    .long isr_part1_165, isr_part1_166, isr_part1_167
    .long isr_part1_168, isr_part1_169, isr_part1_170
    .long isr_part1_171, isr_part1_172, isr_part1_173
    .long isr_part1_174, isr_part1_175, isr_part1_176
    .long isr_part1_177, isr_part1_178, isr_part1_179
    .long isr_part1_180, isr_part1_181, isr_part1_182
    .long isr_part1_183, isr_part1_184, isr_part1_185
    .long isr_part1_186, isr_part1_187, isr_part1_188
    .long isr_part1_189, isr_part1_190, isr_part1_191
    .long isr_part1_192, isr_part1_193, isr_part1_194
    .long isr_part1_195, isr_part1_196, isr_part1_197
    .long isr_part1_198, isr_part1_199, isr_part1_200
    .long isr_part1_201, isr_part1_202, isr_part1_203
    .long isr_part1_204, isr_part1_205, isr_part1_206
    .long isr_part1_207, isr_part1_208, isr_part1_209
    .long isr_part1_210, isr_part1_211, isr_part1_212
    .long isr_part1_213, isr_part1_214, isr_part1_215
    .long isr_part1_216, isr_part1_217, isr_part1_218
    .long isr_part1_219, isr_part1_220, isr_part1_221
    .long isr_part1_222, isr_part1_223, isr_part1_224
    .long isr_part1_225, isr_part1_226, isr_part1_227
    .long isr_part1_228, isr_part1_229, isr_part1_230
    .long isr_part1_231, isr_part1_232, isr_part1_233
    .long isr_part1_234, isr_part1_235, isr_part1_236
    .long isr_part1_237, isr_part1_238, isr_part1_239
    .long isr_part1_240, isr_part1_241, isr_part1_242
    .long isr_part1_243, isr_part1_244, isr_part1_245
    .long isr_part1_246, isr_part1_247, isr_part1_248
    .long isr_part1_249, isr_part1_250, isr_part1_251
    .long isr_part1_252, isr_part1_253, isr_part1_254
    .long isr_part1_255
