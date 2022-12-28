#include <common.h>

#if DEV_BUILD
char* command_tokenize(char* str, unsigned int strbufsz) {
    char* bound = &str[strbufsz];
    while (*str) {
        if (str > bound) return NULL;
        if (*str == ' ') {
            *str++ = 0;
            while (*str) {
                if (str > bound) return NULL;
                if (*str == ' ') {
                    str++;
                } else
                    break;
            }
            if (str > bound) return NULL;
            if (!*str) return "";
            return str;
        }
        str++;
    }
    return "";
}

/* ChatGPT FTW! */
void hexdump(void *addr, my_size_t len) {
  // The pointer to the memory location
  char *p = (char*)addr;
  
  // Loop through the memory, 16 bytes at a time
  for (my_size_t i = 0; i < len; i += 16) {
    // Print the address at the beginning of the line
    printf("%p: ", p + i);

    // Loop through the current 16 bytes and print their hexadecimal values
    for (my_size_t j = 0; j < 16; j++) {
      // If this is the first byte, print a leading space
      if (j == 0) {
        printf(" ");
      }

      // If we have reached the end of the memory, stop
      if (i + j >= len) {
        break;
      }

      // Print the hexadecimal value of the current byte, padded with zeros
      printf("%02X ", p[i + j]);
    }

    // Print some padding to align the ASCII representation
    printf("\n                                               ");

    // Loop through the current 16 bytes and print their ASCII values
    for (my_size_t j = 0; j < 16; j++) {
      // If we have reached the end of the memory, stop
      if (i + j >= len) {
        break;
      }

      // Print the ASCII value of the current byte, or a dot if it's not printable
      char c = p[i + j];
      if (c >= 32 && c <= 126) {
        printf("%c", c);
      } else {
        printf(".");
      }
    }

    // End the line
    printf("\n");
  }
}

void peek(char* addr_str, char* size_str) {
  void* addr = (void*)strtoull(addr_str, NULL, 0);
  my_size_t size = (my_size_t)strtoull(size_str, NULL, 0);
  printf("addr = 0x%p, size = %llu\n", addr, size);
  hexdump(addr, size);
  return;
}

void poke(char* addr_str, char* u64_data) {
  uint64_t* addr = (uint64_t*)strtoull(addr_str, NULL, 0);
  uint64_t data = (uint64_t)strtoull(u64_data, NULL, 0);
  *addr = data;
  printf("*0x%p = %llu\n", addr, data);
  return;
}

void bzero_command(char* addr_str, char* size_str) {
  void* addr = (void*)strtoull(addr_str, NULL, 0);
  my_size_t size = (my_size_t)strtoull(size_str, NULL, 0);
  printf("addr = 0x%p, size = %llu\n", addr, size);
  bzero(size, addr);
  return;
}
#endif