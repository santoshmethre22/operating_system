#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define META_SIZE 4096  // First 4096 bytes reserved for metadata

typedef struct Super {
    int n;      // total number of blocks
    int s;      // size of each block
    int ubn;    // number of used blocks
    int fbn;    // number of free blocks
    // followed by bitmap (ub), each bit represents if a block is used
} file_info;


typedef struct File{
    char content[2048]; 
}file;
// Function to write metadata and bitmap to file
int write_meta(int fd, FileMeta *meta, unsigned char *bitmap) {
    lseek(fd, 0, SEEK_SET);
    if (write(fd, meta, sizeof(FileMeta)) != sizeof(FileMeta)) return -1;
    int bitmap_size = (meta->n + 7) / 8;
    if (write(fd, bitmap, bitmap_size) != bitmap_size) return -1;
    return 0;
}

// Function to read metadata and bitmap from file
int read_meta(int fd, FileMeta *meta, unsigned char *bitmap) {
    lseek(fd, 0, SEEK_SET);
    if (read(fd, meta, sizeof(FileMeta)) != sizeof(FileMeta)) return -1;
    int bitmap_size = (meta->n + 7) / 8;
    if (read(fd, bitmap, bitmap_size) != bitmap_size) return -1;
    return 0;
}


//Write a function "int init_File_dd(const char *fname, int bsize, int bno)" that creates a file (if it does not exist) called fname of appropriate size (4096 + bsize*bno Bytes) in your folder. The function initializes the first 4096 Byes 
// putting proper values for n, s, ubn, fbn, and ub. If for some reason this functions fails, it returns -1. Otherwise it returns 0.


int init_File_dd(const char *fname, int bsize, int bno) {
   // int fd=open(fname,)
     int fd = open(fname, O_RDWR | O_CREAT, 0666);
     if (fd < 0) return -1;

    // int bitmap_size = (bno + 7) / 8;
    // unsigned char *bitmap = calloc(1, bitmap_size);
    // if (!bitmap) return -1;

    file_info.n=bno;
    file_info.s=bsize;
    file_info.ubn=0;
    file_info.fbn=bno;



    // if (write_meta(fd, &meta, bitmap) < 0) {
    //     free(bitmap);
    //     close(fd);
    //     return -1;
    // }

    // // Expand the file to full size (META_SIZE + bsize * bno)
    // lseek(fd, META_SIZE + bsize * bno - 1, SEEK_SET);
    // write(fd, "", 1);

    // free(bitmap);
    // close(fd);
    // return 0;
}

int get_freeblock(const char *fname) {
    int fd = open(fname, O_RDWR);
    if (fd < 0) return -1;

    FileMeta meta;
    unsigned char bitmap[(META_SIZE - sizeof(FileMeta))];
    if (read_meta(fd, &meta, bitmap) < 0) {
        close(fd);
        return -1;
    }

    for (int i = 0; i < meta.n; ++i) {
        if (!(bitmap[i / 8] & (1 << (i % 8)))) {
            // Mark block as used
            bitmap[i / 8] |= (1 << (i % 8));
            meta.ubn++;
            meta.fbn--;

            // Write 1's to the block
            unsigned char *buf = malloc(meta.s);
            memset(buf, 1, meta.s);
            lseek(fd, META_SIZE + i * meta.s, SEEK_SET);
            write(fd, buf, meta.s);
            free(buf);

            // Update metadata
            write_meta(fd, &meta, bitmap);
            close(fd);
            return i;
        }
    }

    close(fd);
    return -1; // no free block
}

int free_block(const char *fname, int bno) {
    int fd = open(fname, O_RDWR);
    if (fd < 0) return 0;

    FileMeta meta;
    unsigned char bitmap[(META_SIZE - sizeof(FileMeta))];
    if (read_meta(fd, &meta, bitmap) < 0) {
        close(fd);
        return 0;
    }

    if (bno < 0 || bno >= meta.n) {
        close(fd);
        return 0;
    }

    if (!(bitmap[bno / 8] & (1 << (bno % 8)))) {
        // block already free
        close(fd);
        return 0;
    }

    // Mark block as free
    bitmap[bno / 8] &= ~(1 << (bno % 8));
    meta.ubn--;
    meta.fbn++;

    // Write 0's to the block
    unsigned char *buf = calloc(1, meta.s);
    lseek(fd, META_SIZE + bno * meta.s, SEEK_SET);
    write(fd, buf, meta.s);
    free(buf);

    write_meta(fd, &meta, bitmap);
    close(fd);
    return 1;
}

int check_fs(const char *fname) {
    int fd = open(fname, O_RDONLY);
    if (fd < 0) return 1;

    FileMeta meta;
    unsigned char bitmap[(META_SIZE - sizeof(FileMeta))];
    if (read_meta(fd, &meta, bitmap) < 0) {
        close(fd);
        return 1;
    }

    int actual_used = 0;
    for (int i = 0; i < meta.n; ++i) {
        if (bitmap[i / 8] & (1 << (i % 8))) {
            actual_used++;
        }
    }

    if (meta.ubn + meta.fbn != meta.n || actual_used != meta.ubn) {
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}

int main() {
    const char *filename = "dd1";

    // Initialize file with 2048 blocks of 4096 bytes
    if (init_File_dd(filename, 4096, 2048) == 0)
        printf("File initialized successfully.\n");
    else {
        perror("Failed to initialize file");
        return 1;
    }

    // Allocate a block
    int blk = get_freeblock(filename);
    if (blk >= 0)
        printf("Allocated block: %d\n", blk);
    else
        printf("No free block available.\n");

    // Free that block
    if (free_block(filename, blk))
        printf("Block %d freed successfully.\n", blk);
    else
        printf("Failed to free block %d.\n", blk);

    // Check file system integrity
    if (check_fs(filename) == 0)
        printf("Filesystem integrity: OK.\n");
    else
        printf("Filesystem check failed!\n");

    return 0;
}
