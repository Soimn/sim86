#include "sim86.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int
main(int argc, char** argv)
{
  if (argc != 3) fprintf(stderr, "Invalid number of arguments. Expected: estimate <input_binary> <8086|8088>\n");
  else if (strcmp(argv[2], "8086") != 0 && strcmp(argv[2], "8088") != 0) fprintf(stderr, "Invalid second argument, expected 8086 or 8088, not '%s'.", argv[2]);
  else
  {
    bool is_8088 = (strcmp(argv[2], "8088") == 0);

    FILE* file;
    if (fopen_s(&file, argv[1], "rb") != 0) fprintf(stderr, "Failed to open input binary\n");
    else
    {
      fseek(file, 0, SEEK_END);
      u64 file_size = ftell(file);
      rewind(file);

      Memory* memory = calloc(1, sizeof(Memory));


      if      (file_size > sizeof(Memory))                                    fprintf(stderr, "Input binary is too large\n");
      else if (memory == 0 || fread(memory, 1, file_size, file) != file_size) fprintf(stderr, "Failed to read input binary\n");
      else
      {
        CPU_State cpu_state = {
          .register_file = {0},
          .flags         = 0,
          .ip            = 0,
          .memory        = memory,
        };

        uint clocks = 0;

        while (cpu_state.ip < file_size)
        {
          CPU_State prev_state = cpu_state;
          Instruction instruction = DecodeInstruction(memory, &cpu_state.ip);
          ExecuteInstruction(&cpu_state, instruction);

          PrintInstruction(instruction, prev_state.ip, stdout);

          uint base_clocks = 0;
          bool ea          = false;
          uint transfers   = 0;

          if (instruction.kind == Instruction_Add || instruction.kind == Instruction_Sub)
          {
            if (instruction.operand_format == InstructionOperandFormat_RMRM)
            {
              if      (instruction.mod == 3)                  base_clocks =  3, ea = false, transfers = 0;
              else if (instruction.flags & InstructionFlag_D) base_clocks =  9, ea = true,  transfers = 1;
              else                                            base_clocks = 16, ea = true, transfers = 2;
            }
            else
            {
              ASSERT(instruction.operand_format == InstructionOperandFormat_RMImmed);
              if (instruction.mod == 3) base_clocks =  4, ea = false, transfers = 0;
              else                      base_clocks = 17, ea = true,  transfers = 2;
            }
          }
          else if (instruction.kind == Instruction_Cmp)
          {
            if (instruction.operand_format == InstructionOperandFormat_RMRM)
            {
              if      (instruction.mod == 3)                  base_clocks = 3, ea = false, transfers = 0;
              else if (instruction.flags & InstructionFlag_D) base_clocks = 9, ea = true,  transfers = 1;
              else                                            base_clocks = 9, ea = true, transfers = 1;
            }
            else
            {
              ASSERT(instruction.operand_format == InstructionOperandFormat_RMImmed);
              if (instruction.mod == 3) base_clocks =  4, ea = false, transfers = 0;
              else                      base_clocks = 10, ea = true,  transfers = 1;
            }
          }
          else if (instruction.kind == Instruction_Mov)
          {
            if (instruction.operand_format == InstructionOperandFormat_RegImmed)
            {
              base_clocks = 4, ea = false, transfers = 0;
            }
            else if (instruction.operand_format == InstructionOperandFormat_RMRM)
            {
              if      (instruction.mod == 3)                  base_clocks = 2, ea = false, transfers = 0;
              else if (instruction.flags & InstructionFlag_D) base_clocks = 8, ea = true,  transfers = 1;
              else                                            base_clocks = 9, ea = true,  transfers = 1;
            }
            else
            {
              ASSERT(instruction.operand_format == InstructionOperandFormat_RMImmed);
              if (instruction.mod == 3) base_clocks =  4, ea = false, transfers = 0;
              else                      base_clocks = 10, ea = true,  transfers = 1;
            }
          }
          else
          {
            ASSERT(instruction.kind == Instruction_Je  || instruction.kind == Instruction_Jl  ||
                   instruction.kind == Instruction_Jle || instruction.kind == Instruction_Jb  ||
                   instruction.kind == Instruction_Jbe || instruction.kind == Instruction_Jp  ||
                   instruction.kind == Instruction_Jo  || instruction.kind == Instruction_Js  ||
                   instruction.kind == Instruction_Jne || instruction.kind == Instruction_Jge ||
                   instruction.kind == Instruction_Jg  || instruction.kind == Instruction_Jae ||
                   instruction.kind == Instruction_Ja  || instruction.kind == Instruction_Jnp ||
                   instruction.kind == Instruction_Jno || instruction.kind == Instruction_Jns);

            bool branch_taken = (cpu_state.ip == (u32)((int)prev_state.ip + (int)instruction.byte_size + (i16)instruction.disp));

            base_clocks = (branch_taken ? 16 : 4), ea = false, transfers = 0;
          }

          uint ea_clocks = 0;
          if (ea)
          {
            if      (instruction.mod == 0 && instruction.rm == 6)                          ea_clocks = 6;
            else if (instruction.mod == 0 && instruction.rm >= 4)                          ea_clocks = 5;
            else if (instruction.mod != 0 && instruction.rm == 6 && instruction.disp == 0) ea_clocks = 5;
            else if (instruction.mod != 0 && instruction.rm >= 4)                          ea_clocks = 9;
            else if (instruction.mod == 0)                                                 ea_clocks = 7;
            else                                                                           ea_clocks = 11;

            if (instruction.rm == 1 || instruction.rm == 2) ea_clocks += 1;

            bool seg = ((instruction.prefix & InstructionPrefix_SegES) || 
                        (instruction.prefix & InstructionPrefix_SegSS) ||
                        (instruction.prefix & InstructionPrefix_SegCS) ||
                        (instruction.prefix & InstructionPrefix_SegDS));
            ea_clocks += (seg ? 2 : 0);
          }

          uint penalty = 0;
          if (ea && (instruction.flags & InstructionFlag_W))
          {
            u32 address = EffectiveAddress(&prev_state, instruction.prefix, instruction.mod, instruction.rm, instruction.disp);
            if (address%2 != 0 || is_8088) penalty = 4*transfers;
          }

          uint dclocks = base_clocks + ea_clocks + penalty;

          clocks += dclocks;

          printf(" ; Clocks: +%llu = %llu ", dclocks, clocks);

          if (ea)
          {
            printf("(%llu + %lluea", base_clocks, ea_clocks);
            if (penalty) printf(" + %llup", penalty);
            printf(") ");
          }

          printf("| ");

          for (Register_Kind i = Register_AX; i < REGISTER_COUNT; ++i)
          {
            u16 old = GetRegister(&prev_state, i);
            u16 new = GetRegister(&cpu_state, i);

            if (old == new) continue;
            else            printf("%s:0x%x->0x%x ", RegisterNames[i], old, new);
          }

          printf("ip:0x%x->0x%x ", prev_state.ip, cpu_state.ip);

          if (prev_state.flags != cpu_state.flags)
          {
            printf("flags:");
            for (uint i = 0; i < FLAG_COUNT; ++i) if (GetFlag(&prev_state, i)) printf("%c", FlagNames[i]);
            printf("->");
            for (uint i = 0; i < FLAG_COUNT; ++i) if (GetFlag(&cpu_state, i)) printf("%c", FlagNames[i]);
            printf(" ");
          }

          printf("\n");
        }

        printf("\nFinal registers:\n");
        Register_Kind print_registers[12] = {
          Register_AX,
          Register_BX,
          Register_CX,
          Register_DX,
          Register_SP,
          Register_BP,
          Register_SI,
          Register_DI,

          Register_CS,
          Register_ES,
          Register_SS,
          Register_DS,
        };
        for (uint i = 0; i < sizeof(print_registers)/sizeof(0[print_registers]); ++i)
        {
          Register_Kind reg_kind = print_registers[i];
          if (cpu_state.register_file[reg_kind] != 0)
          {
            printf("      %s: 0x%04x (%u)\n", RegisterNames[reg_kind], cpu_state.register_file[reg_kind], cpu_state.register_file[reg_kind]);
          }
        }

        printf("      ip: 0x%04x (%u)\n", cpu_state.ip, cpu_state.ip);

        if (cpu_state.flags != 0)
        {
          printf("   flags: ");

          for (uint i = 0; i < FLAG_COUNT; ++i) if (GetFlag(&cpu_state, i)) printf("%c", FlagNames[i]);

          printf("\n");
        }
      }

      fclose(file);
    }
  }
}
