#include <stdint.h>
#include <stdio.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef u8 bool;

#define ASSERT(EX) ((EX) ? 1 : (*(volatile int*)0, 0))
#define NOT_IMPLEMENTED ASSERT(!"NOT_IMPLEMENTED")

#define MEMORY_SIZE 0x100000 // 1 MB
#define MEMORY_MASK 0x0FFFFF

typedef struct Memory
{
  u8 mem[MEMORY_SIZE];
} Memory;

u8
ReadByte(Memory* memory, u32 address)
{
  return memory->mem[address & MEMORY_MASK];
}

u16
ReadWord(Memory* memory, u32 address)
{
  return ((u16)ReadByte(memory, address) << 8) | ReadByte(memory, address + 1);
}

typedef u8 Register_Kind;
enum
{
  Register_AL = 0,
  Register_CL,
  Register_DL,
  Register_BL,
  Register_AH,
  Register_CH,
  Register_DH,
  Register_BH,
  Register_AX,
  Register_CX,
  Register_DX,
  Register_BX,
  Register_SP,
  Register_BP,
  Register_SI,
  Register_DI,

  Register_ES,
  Register_CS,
  Register_SS,
  Register_DS,

  REGISTER_COUNT
};

Register_Kind
RegisterFromRegW(u8 reg, bool w)
{
  return (Register_Kind)(reg + (w ? 8 : 0));
}

Register_Kind
RegisterFromSr(u8 sr)
{
  return Register_ES + sr;
}

typedef u8 Instruction_Kind;
enum
{
  Instruction_Add = 1,
  Instruction_Push,
  Instruction_Pop,
  Instruction_Or,
  Instruction_Adc,
  Instruction_Sbb,
  Instruction_And,
  Instruction_Daa,
  Instruction_Sub,
  Instruction_Das,
  Instruction_Xor,
  Instruction_Aaa,
  Instruction_Cmp,
  Instruction_Aas,
  Instruction_Inc,
  Instruction_Dec,
  Instruction_Jo,
  Instruction_Jno,
  Instruction_Jb,
  Instruction_Jae,
  Instruction_Je,
  Instruction_Jne,
  Instruction_Jbe,
  Instruction_Ja,
  Instruction_Js,
  Instruction_Jns,
  Instruction_Jp,
  Instruction_Jnp,
  Instruction_Jl,
  Instruction_Jge,
  Instruction_Jle,
  Instruction_Jg,
  Instruction_Test,
  Instruction_Xchg,
  Instruction_Mov,
  Instruction_Lea,
  Instruction_Cbw,
  Instruction_Cwd,
  Instruction_Call,
  Instruction_Wait,
  Instruction_Pushf,
  Instruction_Popf,
  Instruction_Sahf,
  Instruction_Lahf,
  Instruction_Movs,
  Instruction_Cmps,
  Instruction_Stos,
  Instruction_Lods,
  Instruction_Scas,
  Instruction_Ret,
  Instruction_Les,
  Instruction_Lds,
  Instruction_Int,
  Instruction_Into,
  Instruction_Iret,
  Instruction_Rol,
  Instruction_Ror,
  Instruction_Rcl,
  Instruction_Rcr,
  Instruction_Shl,
  Instruction_Shr,
  Instruction_Sar,
  Instruction_Aam,
  Instruction_Aad,
  Instruction_Xlat,
  Instruction_Esc,
  Instruction_Loopnz,
  Instruction_Loopz,
  Instruction_Loop,
  Instruction_Jcxz,
  Instruction_In,
  Instruction_Out,
  Instruction_Jmp,
  Instruction_Hlt,
  Instruction_Cmc,
  Instruction_Not,
  Instruction_Neg,
  Instruction_Mul,
  Instruction_Imul,
  Instruction_Div,
  Instruction_Idiv,
  Instruction_Clc,
  Instruction_Stc,
  Instruction_Cli,
  Instruction_Sti,
  Instruction_Cld,
  Instruction_Std,
  INSTRUCTION_COUNT
};

typedef u8 Instruction_Prefix;
enum
{
  InstructionPrefix_Lock  = (1 << 0),
  InstructionPrefix_SegES = (1 << 1),
  InstructionPrefix_SegCS = (1 << 2),
  InstructionPrefix_SegSS = (1 << 3),
  InstructionPrefix_SegDS = (1 << 4),
  InstructionPrefix_Rep   = (1 << 5),
  InstructionPrefix_RepZ  = (1 << 6),
};

typedef u8 Instruction_Flags;
enum
{
  InstructionFlag_W = (1 << 0),
  InstructionFlag_S = (1 << 1),
  InstructionFlag_D = (1 << 2),
  InstructionFlag_V = (1 << 3),
  InstructionFlag_Z = (1 << 4),
};

typedef u8 Instruction_Operand_Format;
enum
{
  InstructionOperandFormat_RegMem = 1,
  InstructionOperandFormat_AccImmed,
  InstructionOperandFormat_ES,
  InstructionOperandFormat_CS,
  InstructionOperandFormat_SS,
  InstructionOperandFormat_DS,
  InstructionOperandFormat_AX,
  InstructionOperandFormat_CX,
  InstructionOperandFormat_DX,
  InstructionOperandFormat_BX,
  InstructionOperandFormat_SP,
  InstructionOperandFormat_BP,
  InstructionOperandFormat_SI,
  InstructionOperandFormat_DI,
  InstructionOperandFormat_IpInc8,
  InstructionOperandFormat_RegMemImmed,
};

typedef struct Instruction
{
  Instruction_Kind kind;
  Instruction_Prefix prefix;
  Instruction_Flags flags;
  Instruction_Operand_Format operand_format;
  u8 mod;
  u8 reg;
  u8 rm;
  u16 disp;
  u16 data;
} Instruction;

typedef struct Instruction_Details
{
  Instruction_Kind kind;
  Instruction_Flags flags;
  Instruction_Operand_Format operand_format;
} Instruction_Details;

#define INSTRUCTION_DETAIL(KIND, FLAGS, OP_FORMAT) { .kind = (KIND), .flags = (FLAGS), .operand_format = (OP_FORMAT) }
Instruction_Details InstructionDetailsFromFirstByte[256] = {
  [0x00] = INSTRUCTION_DETAIL(Instruction_Add,    0,                                     InstructionOperandFormat_RegMem),
  [0x01] = INSTRUCTION_DETAIL(Instruction_Add,    InstructionFlag_W,                     InstructionOperandFormat_RegMem),
  [0x02] = INSTRUCTION_DETAIL(Instruction_Add,    InstructionFlag_D,                     InstructionOperandFormat_RegMem),
  [0x03] = INSTRUCTION_DETAIL(Instruction_Add,    InstructionFlag_D | InstructionFlag_W, InstructionOperandFormat_RegMem),
  [0x04] = INSTRUCTION_DETAIL(Instruction_Add,    0,                                     InstructionOperandFormat_AccImmed),
  [0x05] = INSTRUCTION_DETAIL(Instruction_Add,    InstructionFlag_W,                     InstructionOperandFormat_AccImmed),
  [0x06] = INSTRUCTION_DETAIL(Instruction_Push,   0,                                     InstructionOperandFormat_ES),
  [0x07] = INSTRUCTION_DETAIL(Instruction_Pop,    0,                                     InstructionOperandFormat_ES),
  [0x08] = INSTRUCTION_DETAIL(Instruction_Or,     0,                                     InstructionOperandFormat_RegMem),
  [0x09] = INSTRUCTION_DETAIL(Instruction_Or,     InstructionFlag_W,                     InstructionOperandFormat_RegMem),
  [0x0A] = INSTRUCTION_DETAIL(Instruction_Or,     InstructionFlag_D,                     InstructionOperandFormat_RegMem),
  [0x0B] = INSTRUCTION_DETAIL(Instruction_Or,     InstructionFlag_D | InstructionFlag_W, InstructionOperandFormat_RegMem),
  [0x0C] = INSTRUCTION_DETAIL(Instruction_Or,     0,                                     InstructionOperandFormat_AccImmed),
  [0x0D] = INSTRUCTION_DETAIL(Instruction_Or,     InstructionFlag_W,                     InstructionOperandFormat_AccImmed),
  [0x0E] = INSTRUCTION_DETAIL(Instruction_Push,   0,                                     InstructionOperandFormat_CS),

  [0x10] = INSTRUCTION_DETAIL(Instruction_Adc,    0,                                     InstructionOperandFormat_RegMem),
  [0x11] = INSTRUCTION_DETAIL(Instruction_Adc,    InstructionFlag_W,                     InstructionOperandFormat_RegMem),
  [0x12] = INSTRUCTION_DETAIL(Instruction_Adc,    InstructionFlag_D,                     InstructionOperandFormat_RegMem),
  [0x13] = INSTRUCTION_DETAIL(Instruction_Adc,    InstructionFlag_D | InstructionFlag_W, InstructionOperandFormat_RegMem),
  [0x14] = INSTRUCTION_DETAIL(Instruction_Adc,    0,                                     InstructionOperandFormat_AccImmed),
  [0x15] = INSTRUCTION_DETAIL(Instruction_Adc,    InstructionFlag_W,                     InstructionOperandFormat_AccImmed),
  [0x16] = INSTRUCTION_DETAIL(Instruction_Push,   0,                                     InstructionOperandFormat_SS),
  [0x17] = INSTRUCTION_DETAIL(Instruction_Pop,    0,                                     InstructionOperandFormat_SS),
  [0x18] = INSTRUCTION_DETAIL(Instruction_Sbb,    0,                                     InstructionOperandFormat_RegMem),
  [0x19] = INSTRUCTION_DETAIL(Instruction_Sbb,    InstructionFlag_W,                     InstructionOperandFormat_RegMem),
  [0x1A] = INSTRUCTION_DETAIL(Instruction_Sbb,    InstructionFlag_D,                     InstructionOperandFormat_RegMem),
  [0x1B] = INSTRUCTION_DETAIL(Instruction_Sbb,    InstructionFlag_D | InstructionFlag_W, InstructionOperandFormat_RegMem),
  [0x1C] = INSTRUCTION_DETAIL(Instruction_Sbb,    0,                                     InstructionOperandFormat_AccImmed),
  [0x1D] = INSTRUCTION_DETAIL(Instruction_Sbb,    InstructionFlag_W,                     InstructionOperandFormat_AccImmed),
  [0x1E] = INSTRUCTION_DETAIL(Instruction_Push,   0,                                     InstructionOperandFormat_DS),
  [0x1F] = INSTRUCTION_DETAIL(Instruction_Pop,    0,                                     InstructionOperandFormat_DS),

  [0x20] = INSTRUCTION_DETAIL(Instruction_And,    0,                                     InstructionOperandFormat_RegMem),
  [0x21] = INSTRUCTION_DETAIL(Instruction_And,    InstructionFlag_W,                     InstructionOperandFormat_RegMem),
  [0x22] = INSTRUCTION_DETAIL(Instruction_And,    InstructionFlag_D,                     InstructionOperandFormat_RegMem),
  [0x23] = INSTRUCTION_DETAIL(Instruction_And,    InstructionFlag_D | InstructionFlag_W, InstructionOperandFormat_RegMem),
  [0x24] = INSTRUCTION_DETAIL(Instruction_And,    0,                                     InstructionOperandFormat_AccImmed),
  [0x25] = INSTRUCTION_DETAIL(Instruction_And,    InstructionFlag_W,                     InstructionOperandFormat_AccImmed),
  [0x26] = {0},
  [0x27] = INSTRUCTION_DETAIL(Instruction_Daa,    0,                                     0),
  [0x28] = INSTRUCTION_DETAIL(Instruction_Sub,    0,                                     InstructionOperandFormat_RegMem),
  [0x29] = INSTRUCTION_DETAIL(Instruction_Sub,    InstructionFlag_W,                     InstructionOperandFormat_RegMem),
  [0x2A] = INSTRUCTION_DETAIL(Instruction_Sub,    InstructionFlag_D,                     InstructionOperandFormat_RegMem),
  [0x2B] = INSTRUCTION_DETAIL(Instruction_Sub,    InstructionFlag_D | InstructionFlag_W, InstructionOperandFormat_RegMem),
  [0x2C] = INSTRUCTION_DETAIL(Instruction_Sub,    0,                                     InstructionOperandFormat_AccImmed),
  [0x2D] = INSTRUCTION_DETAIL(Instruction_Sub,    InstructionFlag_W,                     InstructionOperandFormat_AccImmed),
  [0x2E] = {0},
  [0x2F] = INSTRUCTION_DETAIL(Instruction_Das,    0,                                     0),

  [0x30] = INSTRUCTION_DETAIL(Instruction_Xor,    0,                                     InstructionOperandFormat_RegMem),
  [0x31] = INSTRUCTION_DETAIL(Instruction_Xor,    InstructionFlag_W,                     InstructionOperandFormat_RegMem),
  [0x32] = INSTRUCTION_DETAIL(Instruction_Xor,    InstructionFlag_D,                     InstructionOperandFormat_RegMem),
  [0x33] = INSTRUCTION_DETAIL(Instruction_Xor,    InstructionFlag_D | InstructionFlag_W, InstructionOperandFormat_RegMem),
  [0x34] = INSTRUCTION_DETAIL(Instruction_Xor,    0,                                     InstructionOperandFormat_AccImmed),
  [0x35] = INSTRUCTION_DETAIL(Instruction_Xor,    InstructionFlag_W,                     InstructionOperandFormat_AccImmed),
  [0x36] = {0},
  [0x37] = INSTRUCTION_DETAIL(Instruction_Aaa,    0,                                     0),
  [0x38] = INSTRUCTION_DETAIL(Instruction_Cmp,    0,                                     InstructionOperandFormat_RegMem),
  [0x39] = INSTRUCTION_DETAIL(Instruction_Cmp,    InstructionFlag_W,                     InstructionOperandFormat_RegMem),
  [0x3A] = INSTRUCTION_DETAIL(Instruction_Cmp,    InstructionFlag_D,                     InstructionOperandFormat_RegMem),
  [0x3B] = INSTRUCTION_DETAIL(Instruction_Cmp,    InstructionFlag_D | InstructionFlag_W, InstructionOperandFormat_RegMem),
  [0x3C] = INSTRUCTION_DETAIL(Instruction_Cmp,    0,                                     InstructionOperandFormat_AccImmed),
  [0x3D] = INSTRUCTION_DETAIL(Instruction_Cmp,    InstructionFlag_W,                     InstructionOperandFormat_AccImmed),
  [0x3E] = {0},
  [0x3F] = INSTRUCTION_DETAIL(Instruction_Aas,    0,                                     0),

  [0x40] = INSTRUCTION_DETAIL(Instruction_Inc,    0,                                     InstructionOperandFormat_AX),
  [0x41] = INSTRUCTION_DETAIL(Instruction_Inc,    0,                                     InstructionOperandFormat_CX),
  [0x42] = INSTRUCTION_DETAIL(Instruction_Inc,    0,                                     InstructionOperandFormat_DX),
  [0x43] = INSTRUCTION_DETAIL(Instruction_Inc,    0,                                     InstructionOperandFormat_BX),
  [0x44] = INSTRUCTION_DETAIL(Instruction_Inc,    0,                                     InstructionOperandFormat_SP),
  [0x45] = INSTRUCTION_DETAIL(Instruction_Inc,    0,                                     InstructionOperandFormat_BP),
  [0x46] = INSTRUCTION_DETAIL(Instruction_Inc,    0,                                     InstructionOperandFormat_SI),
  [0x47] = INSTRUCTION_DETAIL(Instruction_Inc,    0,                                     InstructionOperandFormat_DI),
  [0x48] = INSTRUCTION_DETAIL(Instruction_Dec,    0,                                     InstructionOperandFormat_AX),
  [0x49] = INSTRUCTION_DETAIL(Instruction_Dec,    0,                                     InstructionOperandFormat_CX),
  [0x4A] = INSTRUCTION_DETAIL(Instruction_Dec,    0,                                     InstructionOperandFormat_DX),
  [0x4B] = INSTRUCTION_DETAIL(Instruction_Dec,    0,                                     InstructionOperandFormat_BX),
  [0x4C] = INSTRUCTION_DETAIL(Instruction_Dec,    0,                                     InstructionOperandFormat_SP),
  [0x4D] = INSTRUCTION_DETAIL(Instruction_Dec,    0,                                     InstructionOperandFormat_BP),
  [0x4E] = INSTRUCTION_DETAIL(Instruction_Dec,    0,                                     InstructionOperandFormat_SI),
  [0x4F] = INSTRUCTION_DETAIL(Instruction_Dec,    0,                                     InstructionOperandFormat_DI),

  [0x50] = INSTRUCTION_DETAIL(Instruction_Push,   0,                                     InstructionOperandFormat_AX),
  [0x51] = INSTRUCTION_DETAIL(Instruction_Push,   0,                                     InstructionOperandFormat_CX),
  [0x52] = INSTRUCTION_DETAIL(Instruction_Push,   0,                                     InstructionOperandFormat_DX),
  [0x53] = INSTRUCTION_DETAIL(Instruction_Push,   0,                                     InstructionOperandFormat_BX),
  [0x54] = INSTRUCTION_DETAIL(Instruction_Push,   0,                                     InstructionOperandFormat_SP),
  [0x55] = INSTRUCTION_DETAIL(Instruction_Push,   0,                                     InstructionOperandFormat_BP),
  [0x56] = INSTRUCTION_DETAIL(Instruction_Push,   0,                                     InstructionOperandFormat_SI),
  [0x57] = INSTRUCTION_DETAIL(Instruction_Push,   0,                                     InstructionOperandFormat_DI),
  [0x58] = INSTRUCTION_DETAIL(Instruction_Pop,    0,                                     InstructionOperandFormat_AX),
  [0x59] = INSTRUCTION_DETAIL(Instruction_Pop,    0,                                     InstructionOperandFormat_CX),
  [0x5A] = INSTRUCTION_DETAIL(Instruction_Pop,    0,                                     InstructionOperandFormat_DX),
  [0x5B] = INSTRUCTION_DETAIL(Instruction_Pop,    0,                                     InstructionOperandFormat_BX),
  [0x5C] = INSTRUCTION_DETAIL(Instruction_Pop,    0,                                     InstructionOperandFormat_SP),
  [0x5D] = INSTRUCTION_DETAIL(Instruction_Pop,    0,                                     InstructionOperandFormat_BP),
  [0x5E] = INSTRUCTION_DETAIL(Instruction_Pop,    0,                                     InstructionOperandFormat_SI),
  [0x5F] = INSTRUCTION_DETAIL(Instruction_Pop,    0,                                     InstructionOperandFormat_DI),

  [0x60] = {0},
  [0x61] = {0},
  [0x62] = {0},
  [0x63] = {0},
  [0x64] = {0},
  [0x65] = {0},
  [0x66] = {0},
  [0x67] = {0},
  [0x68] = {0},
  [0x69] = {0},
  [0x6A] = {0},
  [0x6B] = {0},
  [0x6C] = {0},
  [0x6D] = {0},
  [0x6E] = {0},
  [0x6F] = {0},

  [0x70] = INSTRUCTION_DETAIL(Instruction_Jo,     0,                                     InstructionOperandFormat_IpInc8),
  [0x71] = INSTRUCTION_DETAIL(Instruction_Jno,    0,                                     InstructionOperandFormat_IpInc8),
  [0x72] = INSTRUCTION_DETAIL(Instruction_Jb,     0,                                     InstructionOperandFormat_IpInc8),
  [0x73] = INSTRUCTION_DETAIL(Instruction_Jae,    0,                                     InstructionOperandFormat_IpInc8),
  [0x74] = INSTRUCTION_DETAIL(Instruction_Je,     0,                                     InstructionOperandFormat_IpInc8),
  [0x75] = INSTRUCTION_DETAIL(Instruction_Jne,    0,                                     InstructionOperandFormat_IpInc8),
  [0x76] = INSTRUCTION_DETAIL(Instruction_Jbe,    0,                                     InstructionOperandFormat_IpInc8),
  [0x77] = INSTRUCTION_DETAIL(Instruction_Ja,     0,                                     InstructionOperandFormat_IpInc8),
  [0x78] = INSTRUCTION_DETAIL(Instruction_Js,     0,                                     InstructionOperandFormat_IpInc8),
  [0x79] = INSTRUCTION_DETAIL(Instruction_Jns,    0,                                     InstructionOperandFormat_IpInc8),
  [0x7A] = INSTRUCTION_DETAIL(Instruction_Jp,     0,                                     InstructionOperandFormat_IpInc8),
  [0x7B] = INSTRUCTION_DETAIL(Instruction_Jnp,    0,                                     InstructionOperandFormat_IpInc8),
  [0x7C] = INSTRUCTION_DETAIL(Instruction_Jl,     0,                                     InstructionOperandFormat_IpInc8),
  [0x7D] = INSTRUCTION_DETAIL(Instruction_Jge,    0,                                     InstructionOperandFormat_IpInc8),
  [0x7E] = INSTRUCTION_DETAIL(Instruction_Jle,    0,                                     InstructionOperandFormat_IpInc8),
  [0x7F] = INSTRUCTION_DETAIL(Instruction_Jg,     0,                                     InstructionOperandFormat_IpInc8),

  [0x80] = {0},
  [0x81] = {0},
  [0x82] = {0},
  [0x83] = {0},
  [0x84] = INSTRUCTION_DETAIL(Instruction_Test,   0,                                     InstructionOperandFormat_RegMem),
  [0x85] = INSTRUCTION_DETAIL(Instruction_Test,   InstructionFlag_W,                     InstructionOperandFormat_RegMem),
  [0x86] = INSTRUCTION_DETAIL(Instruction_Xchg,   0,                                     InstructionOperandFormat_RegMem),
  [0x87] = INSTRUCTION_DETAIL(Instruction_Xchg,   InstructionFlag_W,                     InstructionOperandFormat_RegMem),
  [0x88] = INSTRUCTION_DETAIL(Instruction_Mov,    0,                                     InstructionOperandFormat_RegMem),
  [0x89] = INSTRUCTION_DETAIL(Instruction_Mov,    InstructionFlag_W,                     InstructionOperandFormat_RegMem),
  [0x8A] = INSTRUCTION_DETAIL(Instruction_Mov,    InstructionFlag_D,                     InstructionOperandFormat_RegMem),
  [0x8B] = INSTRUCTION_DETAIL(Instruction_Mov,    InstructionFlag_D | InstructionFlag_W, InstructionOperandFormat_RegMem),
  [0x8C] = INSTRUCTION_DETAIL(Instruction_Mov,    0,                                     0),
  [0x8D] = INSTRUCTION_DETAIL(Instruction_Lea,    0,                                     InstructionOperandFormat_RegMem),
  [0x8E] = INSTRUCTION_DETAIL(Instruction_Mov,    InstructionFlag_D,                     0),
  [0x8F] = INSTRUCTION_DETAIL(Instruction_Pop,    0,                                     0),

  [0x90] = INSTRUCTION_DETAIL(Instruction_Xchg,   0,                                     0),
  [0x91] = INSTRUCTION_DETAIL(Instruction_Xchg,   0,                                     0),
  [0x92] = INSTRUCTION_DETAIL(Instruction_Xchg,   0,                                     0),
  [0x93] = INSTRUCTION_DETAIL(Instruction_Xchg,   0,                                     0),
  [0x94] = INSTRUCTION_DETAIL(Instruction_Xchg,   0,                                     0),
  [0x95] = INSTRUCTION_DETAIL(Instruction_Xchg,   0,                                     0),
  [0x96] = INSTRUCTION_DETAIL(Instruction_Xchg,   0,                                     0),
  [0x97] = INSTRUCTION_DETAIL(Instruction_Xchg,   0,                                     0),
  [0x98] = INSTRUCTION_DETAIL(Instruction_Cbw,    0,                                     0),
  [0x99] = INSTRUCTION_DETAIL(Instruction_Cwd,    0,                                     0),
  [0x9A] = INSTRUCTION_DETAIL(Instruction_Call,   0,                                     0),
  [0x9B] = INSTRUCTION_DETAIL(Instruction_Wait,   0,                                     0),
  [0x9C] = INSTRUCTION_DETAIL(Instruction_Pushf,  0,                                     0),
  [0x9D] = INSTRUCTION_DETAIL(Instruction_Popf,   0,                                     0),
  [0x9E] = INSTRUCTION_DETAIL(Instruction_Sahf,   0,                                     0),
  [0x9F] = INSTRUCTION_DETAIL(Instruction_Lahf,   0,                                     0),

  [0xA0] = INSTRUCTION_DETAIL(Instruction_Mov,    0,                                     0),
  [0xA1] = INSTRUCTION_DETAIL(Instruction_Mov,    InstructionFlag_W,                     0),
  [0xA2] = INSTRUCTION_DETAIL(Instruction_Mov,    0,                                     0),
  [0xA3] = INSTRUCTION_DETAIL(Instruction_Mov,    InstructionFlag_W,                     0),
  [0xA4] = INSTRUCTION_DETAIL(Instruction_Movs,   0,                                     0),
  [0xA5] = INSTRUCTION_DETAIL(Instruction_Movs,   InstructionFlag_W,                     0),
  [0xA6] = INSTRUCTION_DETAIL(Instruction_Cmps,   0,                                     0),
  [0xA7] = INSTRUCTION_DETAIL(Instruction_Cmps,   InstructionFlag_W,                     0),
  [0xA8] = INSTRUCTION_DETAIL(Instruction_Test,   0,                                     InstructionOperandFormat_AccImmed),
  [0xA9] = INSTRUCTION_DETAIL(Instruction_Test,   InstructionFlag_W,                     InstructionOperandFormat_AccImmed),
  [0xAA] = INSTRUCTION_DETAIL(Instruction_Stos,   0,                                     0),
  [0xAB] = INSTRUCTION_DETAIL(Instruction_Stos,   InstructionFlag_W,                     0),
  [0xAC] = INSTRUCTION_DETAIL(Instruction_Lods,   0,                                     0),
  [0xAD] = INSTRUCTION_DETAIL(Instruction_Lods,   InstructionFlag_W,                     0),
  [0xAE] = INSTRUCTION_DETAIL(Instruction_Scas,   0,                                     0),
  [0xAF] = INSTRUCTION_DETAIL(Instruction_Scas,   InstructionFlag_W,                     0),

  [0xB0] = INSTRUCTION_DETAIL(Instruction_Mov,    0,                                     0),
  [0xB1] = INSTRUCTION_DETAIL(Instruction_Mov,    0,                                     0),
  [0xB2] = INSTRUCTION_DETAIL(Instruction_Mov,    0,                                     0),
  [0xB3] = INSTRUCTION_DETAIL(Instruction_Mov,    0,                                     0),
  [0xB4] = INSTRUCTION_DETAIL(Instruction_Mov,    0,                                     0),
  [0xB5] = INSTRUCTION_DETAIL(Instruction_Mov,    0,                                     0),
  [0xB6] = INSTRUCTION_DETAIL(Instruction_Mov,    0,                                     0),
  [0xB7] = INSTRUCTION_DETAIL(Instruction_Mov,    0,                                     0),
  [0xB8] = INSTRUCTION_DETAIL(Instruction_Mov,    InstructionFlag_W,                     0),
  [0xB9] = INSTRUCTION_DETAIL(Instruction_Mov,    InstructionFlag_W,                     0),
  [0xBA] = INSTRUCTION_DETAIL(Instruction_Mov,    InstructionFlag_W,                     0),
  [0xBB] = INSTRUCTION_DETAIL(Instruction_Mov,    InstructionFlag_W,                     0),
  [0xBC] = INSTRUCTION_DETAIL(Instruction_Mov,    InstructionFlag_W,                     0),
  [0xBD] = INSTRUCTION_DETAIL(Instruction_Mov,    InstructionFlag_W,                     0),
  [0xBE] = INSTRUCTION_DETAIL(Instruction_Mov,    InstructionFlag_W,                     0),
  [0xBF] = INSTRUCTION_DETAIL(Instruction_Mov,    InstructionFlag_W,                     0),

  [0xC0] = {0},
  [0xC1] = {0},
  [0xC2] = INSTRUCTION_DETAIL(Instruction_Ret,    0,                                     0),
  [0xC3] = INSTRUCTION_DETAIL(Instruction_Ret,    0,                                     0),
  [0xC4] = INSTRUCTION_DETAIL(Instruction_Les,    0,                                     InstructionOperandFormat_RegMem),
  [0xC5] = INSTRUCTION_DETAIL(Instruction_Lds,    0,                                     InstructionOperandFormat_RegMem),
  [0xC6] = INSTRUCTION_DETAIL(Instruction_Mov,    InstructionFlag_W,                     0),
  [0xC7] = INSTRUCTION_DETAIL(Instruction_Mov,    InstructionFlag_W,                     0),
  [0xC8] = {0},
  [0xC9] = {0},
  [0xCA] = INSTRUCTION_DETAIL(Instruction_Ret,    0,                                     0),
  [0xCB] = INSTRUCTION_DETAIL(Instruction_Ret,    0,                                     0),
  [0xCC] = INSTRUCTION_DETAIL(Instruction_Int,    0,                                     0),
  [0xCD] = INSTRUCTION_DETAIL(Instruction_Int,    0,                                     0),
  [0xCE] = INSTRUCTION_DETAIL(Instruction_Into,   0,                                     0),
  [0xCF] = INSTRUCTION_DETAIL(Instruction_Iret,   0,                                     0),

  [0xD0] = {0},
  [0xD1] = {0},
  [0xD2] = {0},
  [0xD3] = {0},
  [0xD4] = INSTRUCTION_DETAIL(Instruction_Aam,    0,                                     0),
  [0xD5] = INSTRUCTION_DETAIL(Instruction_Aad,    0,                                     0),
  [0xD6] = {0},
  [0xD7] = INSTRUCTION_DETAIL(Instruction_Xlat,   0,                                     0),
  [0xD8] = INSTRUCTION_DETAIL(Instruction_Esc,    0,                                     0),
  [0xD9] = INSTRUCTION_DETAIL(Instruction_Esc,    0,                                     0),
  [0xDA] = INSTRUCTION_DETAIL(Instruction_Esc,    0,                                     0),
  [0xDB] = INSTRUCTION_DETAIL(Instruction_Esc,    0,                                     0),
  [0xDC] = INSTRUCTION_DETAIL(Instruction_Esc,    0,                                     0),
  [0xDD] = INSTRUCTION_DETAIL(Instruction_Esc,    0,                                     0),
  [0xDE] = INSTRUCTION_DETAIL(Instruction_Esc,    0,                                     0),
  [0xDF] = INSTRUCTION_DETAIL(Instruction_Esc,    0,                                     0),

  [0xE0] = INSTRUCTION_DETAIL(Instruction_Loopnz, 0,                                     InstructionOperandFormat_IpInc8),
  [0xE1] = INSTRUCTION_DETAIL(Instruction_Loopz,  0,                                     InstructionOperandFormat_IpInc8),
  [0xE2] = INSTRUCTION_DETAIL(Instruction_Loop,   0,                                     InstructionOperandFormat_IpInc8),
  [0xE3] = INSTRUCTION_DETAIL(Instruction_Jcxz,   0,                                     InstructionOperandFormat_IpInc8),
  [0xE4] = INSTRUCTION_DETAIL(Instruction_In,     0,                                     InstructionOperandFormat_AccImmed),
  [0xE5] = INSTRUCTION_DETAIL(Instruction_In,     InstructionFlag_W,                     InstructionOperandFormat_AccImmed),
  [0xE6] = INSTRUCTION_DETAIL(Instruction_Out,    0,                                     InstructionOperandFormat_AccImmed),
  [0xE7] = INSTRUCTION_DETAIL(Instruction_Out,    InstructionFlag_W,                     InstructionOperandFormat_AccImmed),
  [0xE8] = INSTRUCTION_DETAIL(Instruction_Call,   0,                                     0),
  [0xE9] = INSTRUCTION_DETAIL(Instruction_Jmp,    0,                                     0),
  [0xEA] = INSTRUCTION_DETAIL(Instruction_Jmp,    0,                                     0),
  [0xEB] = INSTRUCTION_DETAIL(Instruction_Jmp,    0,                                     InstructionOperandFormat_IpInc8),
  [0xEC] = INSTRUCTION_DETAIL(Instruction_In,     0,                                     InstructionOperandFormat_AccImmed),
  [0xED] = INSTRUCTION_DETAIL(Instruction_In,     InstructionFlag_W,                     InstructionOperandFormat_AccImmed),
  [0xEE] = INSTRUCTION_DETAIL(Instruction_Out,    0,                                     InstructionOperandFormat_AccImmed),
  [0xEF] = INSTRUCTION_DETAIL(Instruction_Out,    InstructionFlag_W,                     InstructionOperandFormat_AccImmed),

  [0xF0] = {0},
  [0xF1] = {0},
  [0xF2] = {0},
  [0xF3] = {0},
  [0xF4] = INSTRUCTION_DETAIL(Instruction_Hlt,    0,                                     0),
  [0xF5] = INSTRUCTION_DETAIL(Instruction_Cmc,    0,                                     0),
  [0xF6] = {0},
  [0xF7] = {0},
  [0xF8] = INSTRUCTION_DETAIL(Instruction_Clc,    0,                                     0),
  [0xF9] = INSTRUCTION_DETAIL(Instruction_Stc,    0,                                     0),
  [0xFA] = INSTRUCTION_DETAIL(Instruction_Cli,    0,                                     0),
  [0xFB] = INSTRUCTION_DETAIL(Instruction_Sti,    0,                                     0),
  [0xFC] = INSTRUCTION_DETAIL(Instruction_Cld,    0,                                     0),
  [0xFD] = INSTRUCTION_DETAIL(Instruction_Std,    0,                                     0),
  [0xFE] = {0},
  [0xFF] = {0},
};

Instruction
DecodeInstruction(Memory* memory, u32* cursor)
{
  Instruction instruction = {0};

  u8 first_byte;

  for (;;)
  {
    first_byte = ReadByte(memory, *cursor);
    *cursor += 1;

    if (first_byte == 0xF0)
    {
      instruction.prefix |= InstructionPrefix_Lock;
      continue;
    }
    else if ((first_byte & 0xFE) == 0xF2)
    {
      instruction.prefix |= (first_byte == 0xF2 ? InstructionPrefix_Rep : InstructionPrefix_RepZ);
      continue;
    }
    else if ((first_byte & 0xF7) == 0x26)
    {
      Instruction_Prefix seg_prefixes[4] = {
        InstructionPrefix_SegES,
        InstructionPrefix_SegSS,
        InstructionPrefix_SegCS,
        InstructionPrefix_SegDS,
      };

      instruction.prefix |= seg_prefixes[((first_byte & 0x10) >> 4) | (first_byte & 0x02)];
      continue;
    }
    else break;
  }

  if ((first_byte & 0xFC) == 0x8C) // Immed
  {
    u8 second_byte = ReadByte(memory, *cursor);
    *cursor += 1;

    Instruction_Kind kind_from_op[8] = {
      [0] = Instruction_Add,
      [1] = Instruction_Or,
      [2] = Instruction_Adc,
      [3] = Instruction_Sbb,
      [4] = Instruction_And,
      [5] = Instruction_Sub,
      [6] = Instruction_Xor,
      [7] = Instruction_Cmp,
    };

    instruction.kind           = kind_from_op[(second_byte >> 3) & 0x7];
    instruction.flags          = (first_byte & 0x1 ? InstructionFlag_W : 0);
    instruction.operand_format = InstructionOperandFormat_RegMemImmed;

    instruction.mod = second_byte >> 6;
    instruction.rm  = second_byte & 0x7;
  }
  else if ((first_byte & 0xFC) == 0xDC) // Shift
  {
    u8 second_byte = ReadByte(memory, *cursor);
    *cursor += 1;

    Instruction_Kind kind_from_op[8] = {
      [0] = Instruction_Rol,
      [1] = Instruction_Ror,
      [2] = Instruction_Rcl,
      [3] = Instruction_Rcr,
      [4] = Instruction_Shl,
      [5] = Instruction_Shr,
      [6] = 0,
      [7] = Instruction_Sar,
    };

    instruction.kind           = kind_from_op[(second_byte >> 3) & 0x7];
    instruction.flags          = (first_byte & 0x1 ? InstructionFlag_W : 0) | (first_byte & 0x2 ? InstructionFlag_V : 0);
    //instruction.operand_format = ;
  }
  else if ((first_byte & 0xFE) == 0xF6) // Grp 1
  {
    u8 second_byte = ReadByte(memory, *cursor);
    *cursor += 1;

    Instruction_Kind kind_from_op[8] = {
      [0] = Instruction_Test,
      [1] = 0,
      [2] = Instruction_Not,
      [3] = Instruction_Neg,
      [4] = Instruction_Mul,
      [5] = Instruction_Imul,
      [6] = Instruction_Div,
      [7] = Instruction_Idiv,
    };

    instruction.kind           = kind_from_op[(second_byte >> 3) & 0x7];
    instruction.flags          = (first_byte & 0x1 ? InstructionFlag_W : 0);
    //instruction.operand_format = ;
  }
  else if ((first_byte & 0xFE) == 0xFE) // Grp 2
  {
    u8 second_byte = ReadByte(memory, *cursor);
    *cursor += 1;

    Instruction_Kind kind_from_op[8] = {
      [0] = Instruction_Inc,
      [1] = Instruction_Dec,
      [2] = Instruction_Call,
      [3] = Instruction_Call,
      [4] = Instruction_Jmp,
      [5] = Instruction_Jmp,
      [6] = Instruction_Push,
      [7] = 0,
    };

    u8 op = (second_byte >> 3) & 0x7;
    instruction.kind           = kind_from_op[op];
    instruction.flags          = (first_byte & 0x1 ? InstructionFlag_W : 0);
    //instruction.operand_format = ;
  }
  else
  {
    Instruction_Details details = InstructionDetailsFromFirstByte[first_byte];
    instruction.kind           = details.kind;
    instruction.flags          = details.flags;
    instruction.operand_format = details.operand_format;

    if (instruction.operand_format == InstructionOperandFormat_RegMem)
    {
      u8 second_byte = ReadByte(memory, *cursor);
      *cursor += 1;

      instruction.mod = second_byte >> 6;
      instruction.reg = (second_byte >> 3) & 0x7;
      instruction.rm  = second_byte & 0x7;
    }
  }

  if (instruction.operand_format == InstructionOperandFormat_RegMem || instruction.operand_format == InstructionOperandFormat_RegMemImmed) // TODO:
  {
    if (instruction.mod == 0 && instruction.rm == 6 || instruction.mod == 2)
    {
      instruction.disp = ReadWord(memory, *cursor);
      *cursor += 2;
    }
    else if (instruction.mod == 1)
    {
      // TODO: Sign extend?
      instruction.disp = ReadByte(memory, *cursor);
      *cursor += 1;
    }
  }

  if (instruction.operand_format == InstructionOperandFormat_AccImmed || instruction.operand_format == InstructionOperandFormat_RegMemImmed) // TODO:
  {
    if (instruction.flags & InstructionFlag_W)
    {
      if (instruction.flags & InstructionFlag_S)
      {
        instruction.data = (i16)(i8)ReadByte(memory, *cursor);
        *cursor += 1;
      }
      else
      {
        instruction.data = ReadWord(memory, *cursor);
        *cursor += 2;
      }
    }
    else
    {
      instruction.data = ReadByte(memory, *cursor);
      *cursor += 1;
    }
  }

  return instruction;
}

char* InstructionNames[INSTRUCTION_COUNT] = {
  [Instruction_Add]    = "add",
  [Instruction_Push]   = "push",
  [Instruction_Pop]    = "pop",
  [Instruction_Or]     = "or",
  [Instruction_Adc]    = "adc",
  [Instruction_Sbb]    = "sbb",
  [Instruction_And]    = "and",
  [Instruction_Daa]    = "daa",
  [Instruction_Sub]    = "sub",
  [Instruction_Das]    = "das",
  [Instruction_Xor]    = "xor",
  [Instruction_Aaa]    = "aaa",
  [Instruction_Cmp]    = "cmp",
  [Instruction_Aas]    = "aas",
  [Instruction_Inc]    = "inc",
  [Instruction_Dec]    = "dec",
  [Instruction_Jo]     = "jo",
  [Instruction_Jno]    = "jno",
  [Instruction_Jb]     = "jb",
  [Instruction_Jae]    = "jae",
  [Instruction_Je]     = "je",
  [Instruction_Jne]    = "jne",
  [Instruction_Jbe]    = "jbe",
  [Instruction_Ja]     = "ja",
  [Instruction_Js]     = "js",
  [Instruction_Jns]    = "jns",
  [Instruction_Jp]     = "jp",
  [Instruction_Jnp]    = "jnp",
  [Instruction_Jl]     = "jl",
  [Instruction_Jge]    = "jge",
  [Instruction_Jle]    = "jle",
  [Instruction_Jg]     = "jg",
  [Instruction_Test]   = "test",
  [Instruction_Xchg]   = "xchg",
  [Instruction_Mov]    = "mov",
  [Instruction_Lea]    = "lea",
  [Instruction_Cbw]    = "cbw",
  [Instruction_Cwd]    = "cwd",
  [Instruction_Call]   = "call",
  [Instruction_Wait]   = "wait",
  [Instruction_Pushf]  = "pushf",
  [Instruction_Popf]   = "popf",
  [Instruction_Sahf]   = "sahf",
  [Instruction_Lahf]   = "lahf",
  [Instruction_Movs]   = "movs",
  [Instruction_Cmps]   = "cmps",
  [Instruction_Stos]   = "stos",
  [Instruction_Lods]   = "lods",
  [Instruction_Scas]   = "scas",
  [Instruction_Ret]    = "ret",
  [Instruction_Les]    = "les",
  [Instruction_Lds]    = "lds",
  [Instruction_Int]    = "int",
  [Instruction_Into]   = "into",
  [Instruction_Iret]   = "iret",
  [Instruction_Rol]    = "rol",
  [Instruction_Ror]    = "ror",
  [Instruction_Rcl]    = "rcl",
  [Instruction_Rcr]    = "rcr",
  [Instruction_Shl]    = "shl",
  [Instruction_Shr]    = "shr",
  [Instruction_Sar]    = "sar",
  [Instruction_Aam]    = "aam",
  [Instruction_Aad]    = "aad",
  [Instruction_Xlat]   = "xlat",
  [Instruction_Esc]    = "esc",
  [Instruction_Loopnz] = "loopnz",
  [Instruction_Loopz]  = "loopz",
  [Instruction_Loop]   = "loop",
  [Instruction_Jcxz]   = "jcxz",
  [Instruction_In]     = "in",
  [Instruction_Out]    = "out",
  [Instruction_Jmp]    = "jmp",
  [Instruction_Hlt]    = "hlt",
  [Instruction_Cmc]    = "cmc",
  [Instruction_Not]    = "not",
  [Instruction_Neg]    = "neg",
  [Instruction_Mul]    = "mul",
  [Instruction_Imul]   = "imul",
  [Instruction_Div]    = "div",
  [Instruction_Idiv]   = "idiv",
  [Instruction_Clc]    = "clc",
  [Instruction_Stc]    = "stc",
  [Instruction_Cli]    = "cli",
  [Instruction_Sti]    = "sti",
  [Instruction_Cld]    = "cld",
  [Instruction_Std]    = "std",
};

char* RegisterNames[REGISTER_COUNT] = {
  [Register_AL] = "al",
  [Register_CL] = "cl",
  [Register_DL] = "dl",
  [Register_BL] = "bl",
  [Register_AH] = "ah",
  [Register_CH] = "ch",
  [Register_DH] = "dh",
  [Register_BH] = "bh",
  [Register_AX] = "ax",
  [Register_CX] = "cx",
  [Register_DX] = "dx",
  [Register_BX] = "bx",
  [Register_SP] = "sp",
  [Register_BP] = "bp",
  [Register_SI] = "si",
  [Register_DI] = "di",

  [Register_ES] = "es",
  [Register_CS] = "cs",
  [Register_SS] = "ss",
  [Register_DS] = "ds",
};

void
PrintInstruction__PrintMemoryRef(u8 mod, u8 rm, bool w, u16 disp, FILE* file)
{
  ASSERT(mod != 3);

  char* effective_address_patterns[8] = {
    "bx + si",
    "bx + di",
    "bp + si",
    "bp + di",
    "si",
    "di",
    "bp",
    "bx"
  };

  fprintf(file, "[");

  if (mod == 0)
  {
    if (rm == 6) fprintf(file, "%u", disp);
    else         fprintf(file, "%s", effective_address_patterns[rm]);
  }
  else fprintf(file, "%s%+d", effective_address_patterns[rm], (i16)disp);

  fprintf(file, "]");
}

void
PrintInstruction(Instruction instruction, FILE* file)
{
  if (instruction.prefix & InstructionPrefix_Lock)
  {
    fprintf(file, "lock ");
  }

  if (instruction.prefix & InstructionPrefix_Rep)
  {
    fprintf(file, "rep ");
  }

  if (instruction.prefix & InstructionPrefix_RepZ)
  {
    fprintf(file, "repz ");
  }
  // TODO
  fprintf(file, InstructionNames[instruction.kind]);

  fprintf(file, " ");

  bool w = instruction.flags & InstructionFlag_W;
  bool d = instruction.flags & InstructionFlag_D;
  bool s = instruction.flags & InstructionFlag_S;
  bool v = instruction.flags & InstructionFlag_V;
  bool z = instruction.flags & InstructionFlag_Z;

  switch (instruction.operand_format)
  {
    case InstructionOperandFormat_RegMem:
    {
      char* reg_name = RegisterNames[RegisterFromRegW(instruction.reg, w)];

      if (instruction.mod == 3) fprintf(file, "%s, %s", reg_name, RegisterNames[RegisterFromRegW(instruction.reg, w)]);
      else
      {
        if (d) fprintf(file, "%s, ", reg_name);

        PrintInstruction__PrintMemoryRef(instruction.mod, instruction.rm, w, instruction.disp, file);

        if (!d) fprintf(file, ", %s", reg_name);
      }

    } break;

    case InstructionOperandFormat_AccImmed:
    {
      fprintf(file, "%s, %u", RegisterNames[RegisterFromRegW(instruction.reg, w)], instruction.data);
    } break;

    case InstructionOperandFormat_ES: fprintf(file, "%s", RegisterNames[Register_ES]); break;
    case InstructionOperandFormat_CS: fprintf(file, "%s", RegisterNames[Register_CS]); break;
    case InstructionOperandFormat_SS: fprintf(file, "%s", RegisterNames[Register_SS]); break;
    case InstructionOperandFormat_DS: fprintf(file, "%s", RegisterNames[Register_DS]); break;
    case InstructionOperandFormat_AX: fprintf(file, "%s", RegisterNames[Register_AX]); break;
    case InstructionOperandFormat_CX: fprintf(file, "%s", RegisterNames[Register_CX]); break;
    case InstructionOperandFormat_DX: fprintf(file, "%s", RegisterNames[Register_DX]); break;
    case InstructionOperandFormat_BX: fprintf(file, "%s", RegisterNames[Register_BX]); break;
    case InstructionOperandFormat_SP: fprintf(file, "%s", RegisterNames[Register_SP]); break;
    case InstructionOperandFormat_BP: fprintf(file, "%s", RegisterNames[Register_BP]); break;
    case InstructionOperandFormat_SI: fprintf(file, "%s", RegisterNames[Register_SI]); break;
    case InstructionOperandFormat_DI: fprintf(file, "%s", RegisterNames[Register_DI]); break;

    case InstructionOperandFormat_IpInc8:
    {
      NOT_IMPLEMENTED;
    } break;

    case InstructionOperandFormat_RegMemImmed:
    {
      PrintInstruction__PrintMemoryRef(instruction.mod, instruction.rm, w, instruction.disp, file);
      fprintf(file, ", %s %u", (w ? "word" : "byte"), instruction.data);
    } break;
  }
}
