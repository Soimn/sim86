#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef u8 bool;

#define MEMORY_SIZE 0x400 // 1 MB
#define MEMORY_MASK 0x3FF

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
  Instruction_Js,
  Instruction_Jns,
  Instruction_Jp
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

typedef u8 Instruction_Flag;
enum
{
  InstructionFlag_W = (1 << 0),
  InstructionFlag_S = (1 << 1),
  InstructionFlag_D = (1 << 2),
  InstructionFlag_V = (1 << 3),
  InstructionFlag_Z = (1 << 4),
};

typedef struct Instruction
{
  Instruction_Kind kind;
  Instruction_Prefix prefix;
  Instruction_Flag flags;
  u8 mod;
  u8 reg;
  u16 rmdisp;
  u16 data;
} Instruction;

typedef struct Instruction_Details
{
  Instruction_Kind kind;
  Instruction_Flags flags;
} Instruction_Details;

Instruction_Kind InstructionKindFromFirstByte[256] = {
  [0x00] = { .kind = Instruction_Add,  .flags = 0 },
  [0x01] = { .kind = Instruction_Add,  .flags = InstructionFlag_W },
  [0x02] = { .kind = Instruction_Add,  .flags = InstructionFlag_D },
  [0x03] = { .kind = Instruction_Add,  .flags = InstructionFlag_D | InstructionFlag_W },
  [0x04] = { .kind = Instruction_Add,  .flags = 0 },
  [0x05] = { .kind = Instruction_Add,  .flags = InstructionFlag_W },
  [0x06] = { .kind = Instruction_Push, .flags = 0 },
  [0x07] = { .kind = Instruction_Pop,  .flags = 0 },
  [0x08] = { .kind = Instruction_Or,   .flags = 0 },
  [0x09] = { .kind = Instruction_Or,   .flags = InstructionFlag_W },
  [0x0A] = { .kind = Instruction_Or,   .flags = InstructionFlag_D },
  [0x0B] = { .kind = Instruction_Or,   .flags = InstructionFlag_D | InstructionFlag_W },
  [0x0C] = { .kind = Instruction_Or,   .flags = 0 },
  [0x0D] = { .kind = Instruction_Or,   .flags = InstructionFlag_W },
  [0x0E] = { .kind = Instruction_Push, .flags = 0 },

  [0x10] = { .kind = Instruction_Adc,  .flags = 0 },
  [0x11] = { .kind = Instruction_Adc,  .flags = InstructionFlag_W },
  [0x12] = { .kind = Instruction_Adc,  .flags = InstructionFlag_D },
  [0x13] = { .kind = Instruction_Adc,  .flags = InstructionFlag_D | InstructionFlag_W },
  [0x14] = { .kind = Instruction_Adc,  .flags = 0 },
  [0x15] = { .kind = Instruction_Adc,  .flags = InstructionFlag_W },
  [0x16] = { .kind = Instruction_Push, .flags = 0 },
  [0x17] = { .kind = Instruction_Pop,  .flags = 0 },
  [0x18] = { .kind = Instruction_Sbb,  .flags = 0 },
  [0x19] = { .kind = Instruction_Sbb,  .flags = InstructionFlag_W },
  [0x1A] = { .kind = Instruction_Sbb,  .flags = InstructionFlag_D },
  [0x1B] = { .kind = Instruction_Sbb,  .flags = InstructionFlag_D | InstructionFlag_W },
  [0x1C] = { .kind = Instruction_Sbb,  .flags = 0 },
  [0x1D] = { .kind = Instruction_Sbb,  .flags = InstructionFlag_W },
  [0x1E] = { .kind = Instruction_Push, .flags = 0 },
  [0x1F] = { .kind = Instruction_Pop,  .flags = 0 },

  [0x20] = { .kind = Instruction_And, .flags = 0 },
  [0x21] = { .kind = Instruction_And, .flags = InstructionFlag_W },
  [0x22] = { .kind = Instruction_And, .flags = InstructionFlag_D },
  [0x23] = { .kind = Instruction_And, .flags = InstructionFlag_D | InstructionFlag_W },
  [0x24] = { .kind = Instruction_And, .flags = 0 },
  [0x25] = { .kind = Instruction_And, .flags = InstructionFlag_W },
  [0x26] = {0},
  [0x27] = { .kind = Instruction_Daa, .flags = 0 },
  [0x28] = { .kind = Instruction_Sub, .flags = 0 },
  [0x29] = { .kind = Instruction_Sub, .flags = InstructionFlag_W },
  [0x2A] = { .kind = Instruction_Sub, .flags = InstructionFlag_D },
  [0x2B] = { .kind = Instruction_Sub, .flags = InstructionFlag_D | InstructionFlag_W },
  [0x2C] = { .kind = Instruction_Sub, .flags = 0 },
  [0x2D] = { .kind = Instruction_Sub, .flags = InstructionFlag_W },
  [0x2E] = {0},
  [0x2F] = { .kind = Instruction_Das, .flags = 0 },

  [0x30] = { .kind = Instruction_Xor, .flags = 0 },
  [0x31] = { .kind = Instruction_Xor, .flags = InstructionFlag_W },
  [0x32] = { .kind = Instruction_Xor, .flags = InstructionFlag_D },
  [0x33] = { .kind = Instruction_Xor, .flags = InstructionFlag_D | InstructionFlag_W },
  [0x34] = { .kind = Instruction_Xor, .flags = 0 },
  [0x35] = { .kind = Instruction_Xor, .flags = InstructionFlag_W },
  [0x36] = {0},
  [0x37] = { .kind = Instruction_Aaa, .flags = 0 },
  [0x38] = { .kind = Instruction_Cmp, .flags = 0 },
  [0x39] = { .kind = Instruction_Cmp, .flags = InstructionFlag_W },
  [0x3A] = { .kind = Instruction_Cmp, .flags = InstructionFlag_D },
  [0x3B] = { .kind = Instruction_Cmp, .flags = InstructionFlag_D | InstructionFlag_W },
  [0x3C] = { .kind = Instruction_Cmp, .flags = 0 },
  [0x3D] = { .kind = Instruction_Cmp, .flags = InstructionFlag_W },
  [0x3E] = {0},
  [0x3F] = { .kind = Instruction_Aas, .flags = 0 },

  [0x40] = { .kind = Instruction_Inc, .flags = 0 },
  [0x41] = { .kind = Instruction_Inc, .flags = 0 },
  [0x42] = { .kind = Instruction_Inc, .flags = 0 },
  [0x43] = { .kind = Instruction_Inc, .flags = 0 },
  [0x44] = { .kind = Instruction_Inc, .flags = 0 },
  [0x45] = { .kind = Instruction_Inc, .flags = 0 },
  [0x46] = { .kind = Instruction_Inc, .flags = 0 },
  [0x47] = { .kind = Instruction_Inc, .flags = 0 },
  [0x48] = { .kind = Instruction_Dec, .flags = 0 },
  [0x49] = { .kind = Instruction_Dec, .flags = 0 },
  [0x4A] = { .kind = Instruction_Dec, .flags = 0 },
  [0x4B] = { .kind = Instruction_Dec, .flags = 0 },
  [0x4C] = { .kind = Instruction_Dec, .flags = 0 },
  [0x4D] = { .kind = Instruction_Dec, .flags = 0 },
  [0x4E] = { .kind = Instruction_Dec, .flags = 0 },
  [0x4F] = { .kind = Instruction_Dec, .flags = 0 },

  [0x50] = { .kind = Instruction_Push, .flags = 0 },
  [0x51] = { .kind = Instruction_Push, .flags = 0 },
  [0x52] = { .kind = Instruction_Push, .flags = 0 },
  [0x53] = { .kind = Instruction_Push, .flags = 0 },
  [0x54] = { .kind = Instruction_Push, .flags = 0 },
  [0x55] = { .kind = Instruction_Push, .flags = 0 },
  [0x56] = { .kind = Instruction_Push, .flags = 0 },
  [0x57] = { .kind = Instruction_Push, .flags = 0 },
  [0x58] = { .kind = Instruction_Pop,  .flags = 0 },
  [0x59] = { .kind = Instruction_Pop,  .flags = 0 },
  [0x5A] = { .kind = Instruction_Pop,  .flags = 0 },
  [0x5B] = { .kind = Instruction_Pop,  .flags = 0 },
  [0x5C] = { .kind = Instruction_Pop,  .flags = 0 },
  [0x5D] = { .kind = Instruction_Pop,  .flags = 0 },
  [0x5E] = { .kind = Instruction_Pop,  .flags = 0 },
  [0x5F] = { .kind = Instruction_Pop,  .flags = 0 },

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

  [0x70] = { .kind = Instruction_Jo,  .flags = 0 },
  [0x71] = { .kind = Instruction_Jno, .flags = 0 },
  [0x72] = { .kind = Instruction_Jb,  .flags = 0 },
  [0x73] = { .kind = Instruction_Jae, .flags = 0 },
  [0x74] = { .kind = Instruction_Je,  .flags = 0 },
  [0x75] = { .kind = Instruction_Jne, .flags = 0 },
  [0x76] = { .kind = Instruction_Jbe, .flags = 0 },
  [0x77] = { .kind = Instruction_Ja,  .flags = 0 },
  [0x78] = { .kind = Instruction_Js,  .flags = 0 },
  [0x79] = { .kind = Instruction_Jns, .flags = 0 },
  [0x7A] = { .kind = Instruction_Jp,  .flags = 0 },
  [0x7B] = { .kind = Instruction_Jnp, .flags = 0 },
  [0x7C] = { .kind = Instruction_Jl,  .flags = 0 },
  [0x7D] = { .kind = Instruction_Jge, .flags = 0 },
  [0x7E] = { .kind = Instruction_Jle, .flags = 0 },
  [0x7F] = { .kind = Instruction_Jg,  .flags = 0 },

  [0x80] = {0},
  [0x81] = {0},
  [0x82] = {0},
  [0x83] = {0},
  [0x84] = { .kind = Instruction_Test, .flags = 0 },
  [0x85] = { .kind = Instruction_Test, .flags = 0 },
  [0x86] = { .kind = Instruction_Xchg, .flags = 0 },
  [0x87] = { .kind = Instruction_Xchg, .flags = 0 },
  [0x88] = { .kind = Instruction_Mov,  .flags = 0 },
  [0x89] = { .kind = Instruction_Mov,  .flags = 0 },
  [0x8A] = { .kind = Instruction_Mov,  .flags = 0 },
  [0x8B] = { .kind = Instruction_Mov,  .flags = 0 },
  [0x8C] = { .kind = Instruction_Mov,  .flags = 0 },
  [0x8D] = { .kind = Instruction_Lea,  .flags = 0 },
  [0x8E] = { .kind = Instruction_Mov,  .flags = 0 },
  [0x8F] = { .kind = Instruction_Pop,  .flags = 0 },

  [0x90] = { .kind = Instruction_Xchg,  .flags = 0 },
  [0x91] = { .kind = Instruction_Xchg,  .flags = 0 },
  [0x92] = { .kind = Instruction_Xchg,  .flags = 0 },
  [0x93] = { .kind = Instruction_Xchg,  .flags = 0 },
  [0x94] = { .kind = Instruction_Xchg,  .flags = 0 },
  [0x95] = { .kind = Instruction_Xchg,  .flags = 0 },
  [0x96] = { .kind = Instruction_Xchg,  .flags = 0 },
  [0x97] = { .kind = Instruction_Xchg,  .flags = 0 },
  [0x98] = { .kind = Instruction_Cbw,   .flags = 0 },
  [0x99] = { .kind = Instruction_Cwd,   .flags = 0 },
  [0x9A] = { .kind = Instruction_Call,  .flags = 0 },
  [0x9B] = { .kind = Instruction_Wait,  .flags = 0 },
  [0x9C] = { .kind = Instruction_Pushf, .flags = 0 },
  [0x9D] = { .kind = Instruction_Popf,  .flags = 0 },
  [0x9E] = { .kind = Instruction_Sahf,  .flags = 0 },
  [0x9F] = { .kind = Instruction_Lahf,  .flags = 0 },

  [0xA0] = { .kind = Instruction_Mov,  .flags = 0 },
  [0xA1] = { .kind = Instruction_Mov,  .flags = 0 },
  [0xA2] = { .kind = Instruction_Mov,  .flags = 0 },
  [0xA3] = { .kind = Instruction_Mov,  .flags = 0 },
  [0xA4] = { .kind = Instruction_Movs, .flags = 0 },
  [0xA5] = { .kind = Instruction_Movs, .flags = 0 },
  [0xA6] = { .kind = Instruction_Cmps, .flags = 0 },
  [0xA7] = { .kind = Instruction_Cmps, .flags = 0 },
  [0xA8] = { .kind = Instruction_Test, .flags = 0 },
  [0xA9] = { .kind = Instruction_Test, .flags = 0 },
  [0xAA] = { .kind = Instruction_Stos, .flags = 0 },
  [0xAB] = { .kind = Instruction_Stos, .flags = 0 },
  [0xAC] = { .kind = Instruction_Lods, .flags = 0 },
  [0xAD] = { .kind = Instruction_Lods, .flags = 0 },
  [0xAE] = { .kind = Instruction_Scas, .flags = 0 },
  [0xAF] = { .kind = Instruction_Scas, .flags = 0 },

  [0xB0] = { .kind = Instruction_Mov, .flags = 0 },
  [0xB1] = { .kind = Instruction_Mov, .flags = 0 },
  [0xB2] = { .kind = Instruction_Mov, .flags = 0 },
  [0xB3] = { .kind = Instruction_Mov, .flags = 0 },
  [0xB4] = { .kind = Instruction_Mov, .flags = 0 },
  [0xB5] = { .kind = Instruction_Mov, .flags = 0 },
  [0xB6] = { .kind = Instruction_Mov, .flags = 0 },
  [0xB7] = { .kind = Instruction_Mov, .flags = 0 },
  [0xB8] = { .kind = Instruction_Mov, .flags = 0 },
  [0xB9] = { .kind = Instruction_Mov, .flags = 0 },
  [0xBA] = { .kind = Instruction_Mov, .flags = 0 },
  [0xBB] = { .kind = Instruction_Mov, .flags = 0 },
  [0xBC] = { .kind = Instruction_Mov, .flags = 0 },
  [0xBD] = { .kind = Instruction_Mov, .flags = 0 },
  [0xBE] = { .kind = Instruction_Mov, .flags = 0 },
  [0xBF] = { .kind = Instruction_Mov, .flags = 0 },

  [0xC0] = {0},
  [0xC1] = {0},
  [0xC2] = { .kind = Instruction_Ret, .flags = 0 },
  [0xC3] = { .kind = Instruction_Ret, .flags = 0 },
  [0xC4] = { .kind = Instruction_Les, .flags = 0 },
  [0xC5] = { .kind = Instruction_Lds, .flags = 0 },
  [0xC6] = { .kind = Instruction_Mov, .flags = 0 },
  [0xC7] = { .kind = Instruction_Mov, .flags = 0 },
  [0xC8] = {0},
  [0xC9] = {0},
  [0xCA] = { .kind = Instruction_Ret,  .flags = 0 },
  [0xCB] = { .kind = Instruction_Ret,  .flags = 0 },
  [0xCC] = { .kind = Instruction_Int,  .flags = 0 },
  [0xCD] = { .kind = Instruction_Int,  .flags = 0 },
  [0xCE] = { .kind = Instruction_Into, .flags = 0 },
  [0xCF] = { .kind = Instruction_Iret, .flags = 0 },

  [0xD0] = {0},
  [0xD1] = {0},
  [0xD2] = {0},
  [0xD3] = {0},
  [0xD4] = { .kind = Instruction_Aam, .flags = 0 },
  [0xD5] = { .kind = Instruction_Aad, .flags = 0 },
  [0xD6] = {0},
  [0xD7] = { .kind = Instruction_Xlat, .flags = 0 },
  [0xD8] = { .kind = Instruction_Esc,  .flags = 0 },
  [0xD9] = { .kind = Instruction_Esc,  .flags = 0 },
  [0xDA] = { .kind = Instruction_Esc,  .flags = 0 },
  [0xDB] = { .kind = Instruction_Esc,  .flags = 0 },
  [0xDC] = { .kind = Instruction_Esc,  .flags = 0 },
  [0xDD] = { .kind = Instruction_Esc,  .flags = 0 },
  [0xDE] = { .kind = Instruction_Esc,  .flags = 0 },
  [0xDF] = { .kind = Instruction_Esc,  .flags = 0 },

  [0xE0] = { .kind = Instruction_Loopnz, .flags = 0 },
  [0xE1] = { .kind = Instruction_Loopz,  .flags = 0 },
  [0xE2] = { .kind = Instruction_Loop,   .flags = 0 },
  [0xE3] = { .kind = Instruction_Jcxz,   .flags = 0 },
  [0xE4] = { .kind = Instruction_In,     .flags = 0 },
  [0xE5] = { .kind = Instruction_In,     .flags = 0 },
  [0xE6] = { .kind = Instruction_Out,    .flags = 0 },
  [0xE7] = { .kind = Instruction_Out,    .flags = 0 },
  [0xE8] = { .kind = Instruction_Call,   .flags = 0 },
  [0xE9] = { .kind = Instruction_Jmp,    .flags = 0 },
  [0xEA] = { .kind = Instruction_Jmp,    .flags = 0 },
  [0xEB] = { .kind = Instruction_Jmp,    .flags = 0 },
  [0xEC] = { .kind = Instruction_In,     .flags = 0 },
  [0xED] = { .kind = Instruction_In,     .flags = 0 },
  [0xEE] = { .kind = Instruction_Out,    .flags = 0 },
  [0xEF] = { .kind = Instruction_Out,    .flags = 0 },

  [0xF0] = {0},
  [0xF1] = {0},
  [0xF2] = {0},
  [0xF3] = {0},
  [0xF4] = { .kind = Instruction_Hlt, .flags = 0 },
  [0xF5] = { .kind = Instruction_Cmc, .flags = 0 },
  [0xF6] = {0},
  [0xF7] = {0},
  [0xF8] = { .kind = Instruction_Clc, .flags = 0 },
  [0xF9] = { .kind = Instruction_Stc, .flags = 0 },
  [0xFA] = { .kind = Instruction_Cli, .flags = 0 },
  [0xFB] = { .kind = Instruction_Sti, .flags = 0 },
  [0xFC] = { .kind = Instruction_Cld, .flags = 0 },
  [0xFD] = { .kind = Instruction_Std, .flags = 0 },
  [0xFE] = {0},
  [0xFF] = {0},
};

Instruction
DecodeInstruction(Memory* memory, u32* cursor)
{
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
        InstructionPrefix_SegEs,
        InstructionPrefix_SegSs,
        InstructionPrefix_SegCs,
        InstructionPrefix_SegDs,
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

    instruction.kind  = kind_from_op[(second_byte >> 3) & 0x7];
    instruction.flags = (first_byte & 0x1 ? InstructionFlag_W : 0);
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

    instruction.kind  = kind_from_op[(second >> 3) & 0x7];
    instruction.flags = (first_byte & 0x1 ? InstructionFlag_W : 0) | (first_byte & 0x2 ? InstructionFlag_V : 0);
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

    instruction.kind  = kind_from_op[(second >> 3) & 0x7];
    instruction.flags = (first_byte & 0x1 ? InstructionFlag_W : 0);
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

    u8 op = (second >> 3) & 0x7;
    instruction.kind  = kind_from_op[op];
    instruction.flags = (op < 2 && (first_byte & 0x1 != 0) ? InstructionFlag_W : 0);
  }
  else
  {
    Instruction_Details details = InstructionDetailsFromFirstByte[first_byte];
    instruction.kind  = details.kind;
    instruction.flags = details.flags;
  }

  if ()
  {
    if (mod == 0 && rm == 6 || mod == 2)
    {
      instruction.disp = ReadWord(memory, *cursor);
      *cursor += 2;
    }
    else if (mod == 1)
    {
      // TODO: Sign extend?
      instruction.disp = ReadByte(memory, *cursor);
      *cursor += 1;
    }
  }

  if ()
  {
    if (w)
    {
      if (s)
      {
        instruction.data = (i16)(i8)ReadByte(memory, *cursor);
        *cursor += 1;
      }
      else
      {
        instruction.data = ReadWord(memory, *cursor);
        *cusor += 2;
      }
    }
    else
    {
      instruction.data = ReadByte(memory, *cursor);
      *cursor += 1;
    }
  }
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
}
