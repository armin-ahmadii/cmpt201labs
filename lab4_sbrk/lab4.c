#define _DEFAULT_SOURCE
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define BUF_SIZE 256

struct header {
  uint64_t size;
  struct header *next;
};

void handle_error(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

void print_out(char *format, void *data, size_t data_size) {
  char buf[BUF_SIZE];
  ssize_t len = snprintf(buf, BUF_SIZE, format,
                         data_size == sizeof(uint64_t) ? *(uint64_t *)data
                                                       : *(void **)data);
  if (len < 0) {
    handle_error("snprintf");
  }
  write(STDOUT_FILENO, buf, len);
}

void iterate(struct header *block) {
  if (block == NULL)
    return;

  unsigned char *start = (unsigned char *)block + sizeof(struct header);
  size_t data_size = block->size - sizeof(struct header);

  for (size_t i = 0; i < data_size; i++) {
    uint64_t val = start[i];
    print_out("%lu\n", &val, sizeof(uint64_t));
  }
}

int main(void) {
  /*Incr Heap Size by 256*/
  void *new_memory_block;
  new_memory_block = sbrk(256);

  struct header *block1 = (struct header *)new_memory_block;
  void *memory_block_2 = (char *)new_memory_block + 128;
  struct header *block2 = (struct header *)memory_block_2;

  memset((char *)block1 + sizeof(struct header), 0,
         128 - sizeof(struct header));
  memset((char *)block2 + sizeof(struct header), 1,
         128 - sizeof(struct header));

  block1->next = NULL;
  block2->next = block1;
  block1->size = 128;
  block2->size = 128;

  /* PRINT TIMEEEEE */
  print_out("Block 1 Address: %p\n", &block1, sizeof(&block1));
  print_out("Block 2 Address: %p\n", &block2, sizeof(&block2));
  print_out("Block 1 Size: %lu\n", &block1->size, sizeof(&block1->size));
  print_out("Block 1 Next: %p\n", &block1->next, sizeof(&block1->next));
  print_out("Block 2 Size: %lu\n", &block2->size, sizeof(&block2->size));
  print_out("Block 2 Next: %p\n", &block2->next, sizeof(&block2->next));
  iterate(block1);
  iterate(block2);
  return 0;
}
