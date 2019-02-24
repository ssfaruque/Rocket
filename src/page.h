#ifndef PAGE_H
#define PAGE_H

#define PAGE_SIZE 4096

typedef struct Page {unsigned char bytes[PAGE_SIZE];} Page;

#endif  // PAGE_H