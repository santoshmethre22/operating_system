#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#define META_SIZE 4096

typedef struct {
    int n;    // total number of blocks
    int s;    // size of each block
    int ubn;  // number of used blocks
    int fbn;  // number of free blocks
    unsigned char ub[0]; // flexible array for bitmap
} Meta;

// Helper to calculate the number of bytes needed for bitmap
int bitmap_size(int n) {
    return (n + 7) / 8;
}

// Set and clear bits in bitmap
void set_bit(unsigned char *bitmap, int i) {
    bitmap[i/8] |= (1 << (i % 8));
}

void clear_bit(unsigned char *bitmap, int i) {
    bitmap[i/8] &= ~(1 << (i % 8));
}

int get_bit(unsigned char *bitmap, int i) {
    return (bitmap[i/8] >> (i % 8)) & 1;
}

// Function 1: Initialize file
// int init_File_dd(const char *fname, int bsize, int bno) {
//     int fd = open(fname, O_RDWR | O_CREAT | O_TRUNC, 0666);
//     if (fd < 0) return -1;
//     int ub_bytes = bitmap_size(bno);
//     int total_size = META_SIZE + bsize * bno;
//     // Allocate and initialize metadata
//     Meta *meta = (Meta *)calloc(1, META_SIZE);
//     meta->n = bno;
//     meta->s = bsize;
//     meta->ubn = 0;
//     meta->fbn = bno;
//     // Write metadata
//     memcpy(meta->ub, calloc(1, ub_bytes), ub_bytes);
//     if (write(fd, meta, META_SIZE) != META_SIZE) {
//         close(fd);
//         return -1;
//     }
//     // Fill the rest of file with 0s
//     char *buf = (char *)calloc(1, bsize);
//     for (int i = 0; i < bno; i++) {
//         if (write(fd, buf, bsize) != bsize) {
//             close(fd);
//             return -1;
//         }
//     }
//     close(fd);
//     return 0;
// }



int intit_File_dd(const char *fame,int bsize,int bno){
    
        int fd=open(fname, O_RDWR | O_CREAT | O_TRUNC, 0666);

        


}





// Function 2: Get a free block-----
int get_freeblock(const char *fname) {
    int fd = open(fname, O_RDWR);
    if (fd < 0) return -1;

    unsigned char buffer[META_SIZE];
    read(fd, buffer, META_SIZE);

    Meta *meta = (Meta *)buffer;
    int n = meta->n;
    int s = meta->s;
    int ub_bytes = bitmap_size(n);
    unsigned char *ub = buffer + sizeof(int) * 4;

    for (int i = 0; i < n; i++) {
        if (!get_bit(ub, i)) {
            // Mark used
            set_bit(ub, i);
            meta->ubn += 1;
            meta->fbn -= 1;

            // Rewrite metadata
            lseek(fd, 0, SEEK_SET);
            write(fd, buffer, META_SIZE);

            // Fill block with 1s
            unsigned char *ones = (unsigned char *)malloc(s);
            memset(ones, 1, s);
            lseek(fd, META_SIZE + i * s, SEEK_SET);
            write(fd, ones, s);
            close(fd);
            return i;
        }
    }

    close(fd);
    return -1; // no free block found
}

// Function 3: Free a used block
int free_block(const char *fname, int bno) {
    int fd = open(fname, O_RDWR);
    if (fd < 0) return 0;

    unsigned char buffer[META_SIZE];
    read(fd, buffer, META_SIZE);
    Meta *meta = (Meta *)buffer;

    int n = meta->n;
    int s = meta->s;
    if (bno < 0 || bno >= n) {
        close(fd);
        return 0;
    }

    unsigned char *ub = buffer + sizeof(int) * 4;
    if (!get_bit(ub, bno)) {
        close(fd);
        return 0; // Already free
    }

    clear_bit(ub, bno);
    meta->ubn -= 1;
    meta->fbn += 1;

    // Write metadata
    lseek(fd, 0, SEEK_SET);
    write(fd, buffer, META_SIZE);

    // Fill block with 0s
    unsigned char *zeros = (unsigned char *)calloc(1, s);
    lseek(fd, META_SIZE + bno * s, SEEK_SET);
    write(fd, zeros, s);
    close(fd);
    return 1;
}

// Function 4: Check file system integrity
int check_fs(const char *fname) {
    int fd = open(fname, O_RDONLY);
    if (fd < 0) return 1;

    unsigned char buffer[META_SIZE];
    read(fd, buffer, META_SIZE);
    Meta *meta = (Meta *)buffer;

    int n = meta->n;
    int ubn = meta->ubn;
    int fbn = meta->fbn;

    if (ubn + fbn != n) return 1;

    unsigned char *ub = buffer + sizeof(int) * 4;
    int counted = 0;
    for (int i = 0; i < n; i++) {
        if (get_bit(ub, i)) counted++;
    }

    if (counted != ubn) return 1;

    close(fd);
    return 0;
}

// Function 5: Main demo
int main() {
    const char *fname = "dd1";
    int n = 2048, s = 4096;

    if (init_File_dd(fname, s, n) != 0) {
        printf("File initialization failed.\n");
        return 1;
    }
    printf("File initialized.\n");

    int block = get_freeblock(fname);
    if (block != -1) printf("Allocated block: %d\n", block);

    int check = check_fs(fname);
    printf("Integrity check result: %d\n", check);

    if (free_block(fname, block)) {
        printf("Block %d freed.\n", block);
    }

    check = check_fs(fname);
    printf("Integrity check after free: %d\n", check);

    return 0;
}
