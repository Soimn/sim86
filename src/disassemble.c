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
        printf("bits 16\n");
        for (u32 cursor = 0; cursor < file_size;)
        {
          u32 address = cursor;
          Instruction instruction = DecodeInstruction(memory, &cursor);
          PrintInstruction(instruction, address, stdout);
          printf("\n");
        }
      }

      fclose(file);
    }
  }
}
