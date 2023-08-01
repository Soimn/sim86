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
          .register_file = {0},
          .flags         = 0,
          .ip            = 0,
          .memory        = memory,
        };

        while (cpu_state.ip < file_size)
        {
          CPU_State prev_state = cpu_state;
          Instruction instruction = DecodeInstruction(memory, &cpu_state.ip);
          ExecuteInstruction(&cpu_state, instruction);

          PrintInstruction(instruction, prev_state.ip, stdout);

          bool has_printed_intro = false;

          for (Register_Kind i = Register_AX; i < REGISTER_COUNT; ++i)
          {
            u16 old = GetRegister(&prev_state, i);
            u16 new = GetRegister(&cpu_state, i);

            if (old == new) continue;
            else
            {
              if (!has_printed_intro)
              {
                printf(" ; ");
                has_printed_intro = true;
              }

              printf("%s:0x%x->0x%x ", RegisterNames[i], old, new);
            }
          }

#define DISPLAY_IP 1
#if DISPLAY_IP
          if (!has_printed_intro)
          {
            printf(" ; ");
            has_printed_intro = true;
          }
          printf("ip:0x%x->0x%x ", prev_state.ip, cpu_state.ip);
#endif

          if (prev_state.flags != cpu_state.flags)
          {
            if (!has_printed_intro)
            {
              printf(" ; ");
              has_printed_intro = true;
            }

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

#if DISPLAY_IP
        printf("      ip: 0x%04x (%u)\n", cpu_state.ip, cpu_state.ip);
#endif

        if (cpu_state.flags != 0)
        {
          printf("   flags: ");

          for (uint i = 0; i < FLAG_COUNT; ++i) if (GetFlag(&cpu_state, i)) printf("%c", FlagNames[i]);

          printf("\n");
        }

#if 1
        FILE* im = fopen("image_out.data", "wb");
        fwrite(cpu_state.memory->mem, 1, 64*4 + 64*64*4, im);
        fclose(im);
#endif
      }

      fclose(file);
    }
  }
}
