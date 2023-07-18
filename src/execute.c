#include "sim86.h"

#include <stdlib.h>
#include <stdio.h>

int
main(int argc, char** argv)
{
  if (argc != 2) fprintf(stderr, "Invalid number of arguments. Expected: disassemble <input_binary>\n");
  else
  {
    FILE* file;
    if (fopen_s(&file, argv[1], "rb") != 0) fprintf(stderr, "Failed to open input binary\n");
    else
    {
      fseek(file, 0, SEEK_END);
      u64 file_size = ftell(file);
      rewind(file);

      Memory* memory = malloc(sizeof(Memory));


      if      (file_size > sizeof(Memory))                                    fprintf(stderr, "Input binary is too large\n");
      else if (memory == 0 || fread(memory, 1, file_size, file) != file_size) fprintf(stderr, "Failed to read input binary\n");
      else
      {
        CPU_State cpu_state = {
          .memory = memory,
        };

        for (u32 cursor = 0; cursor < file_size; )
        {
          u32 address = cursor;
          Instruction instruction = DecodeInstruction(memory, &cursor);

          CPU_State prev_state = cpu_state;
          ExecuteInstruction(&cpu_state, instruction);

          PrintInstruction(instruction, address, stdout);
          for (Register_Kind i = Register_AX; i < REGISTER_COUNT; ++i)
          {
            u16 old = GetRegister(&prev_state, i);
            u16 new = GetRegister(&cpu_state, i);

            if (old == new) continue;
            else
            {
              if (i <= Register_BX)
              {
                if (old >> 8 == new >> 8)
                {
                  old &= 0xFF;
                  new &= 0xFF;
                }
                else if ((old & 0xFF) == (new & 0xFF))
                {
                  old >>= 8;
                  new >>= 8;
                }
              }

              printf(" ; %s:0x%X->0x%X ", RegisterNames[i], old, new);
            }
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

          Register_ES,
          Register_SS,
          Register_DS,
        };
        for (uint i = 0; i < sizeof(print_registers)/sizeof(0[print_registers]); ++i)
        {
          Register_Kind reg_kind = print_registers[i];
          printf("      %s: 0x%04X (%u)\n", RegisterNames[reg_kind], cpu_state.register_file[reg_kind], cpu_state.register_file[reg_kind]);
        }
      }

      fclose(file);
    }
  }
}
