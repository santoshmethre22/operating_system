#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define META_SIZE 4096  // Metadata size in bytes

// Helper function to read metadata
int read_metadata(int fd, int *n, int *s, int *ubn, int *fbn, unsigned char *ub, int bitmap_size) {
    char meta[META_SIZE];
    if (lseek(fd, 0, SEEK_SET) < 0 || read(fd, meta, META_SIZE) != META_SIZE) {
        perror("Failed to read metadata");
        return -1;
    }

    *n = *(int *)(meta);
    *s = *(int *)(meta + 4);
    *ubn = *(int *)(meta + 8);
    *fbn = *(int *)(meta + 12);
    memcpy(ub, meta + 16, bitmap_size);
    return 0;
}

// Helper function to write metadata
int write_metadata(int fd, int n, int s, int ubn, int fbn, unsigned char *ub, int bitmap_size) {
    char meta[META_SIZE];
    memset(meta, 0, META_SIZE);
    *(int *)(meta) = n;
    *(int *)(meta + 4) = s;
    *(int *)(meta + 8) = ubn;
    *(int *)(meta + 12) = fbn;
    memcpy(meta + 16, ub, bitmap_size);

    if (lseek(fd, 0, SEEK_SET) < 0 || write(fd, meta, META_SIZE) != META_SIZE) {
        perror("Failed to write metadata");
        return -1;
    }
    return 0;
}

// Initialize file
int init_File_dd(const char *fname, int bsize, int bno) {
    int fd = open(fname, O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror("File open failed");
        return -1;
    }

    off_t total_size = META_SIZE + (off_t)bsize * bno;
    if (ftruncate(fd, total_size) < 0) {
        perror("File truncate failed");
        close(fd);
        return -1;
    }

    int bitmap_size = (bno + 7) / 8;
    unsigned char bitmap[bitmap_size];
    memset(bitmap, 0, bitmap_size);

    int res = write_metadata(fd, bno, bsize, 0, bno, bitmap, bitmap_size);
    close(fd);
    return res;
}

// Get a free block and mark it used
int get_freeblock(const char *fname) {
    int fd = open(fname, O_RDWR);
    if (fd < 0) {
        perror("Failed to open file");
        return -1;
    }

    int n, s, ubn, fbn;
    int bitmap_size;
    unsigned char *bitmap;

    // First read just enough to get n and s
    char meta_temp[16];
    read(fd, meta_temp, 16);
    n = *(int *)(meta_temp);
    s = *(int *)(meta_temp + 4);
    ubn = *(int *)(meta_temp + 8);
    fbn = *(int *)(meta_temp + 12);
    bitmap_size = (n + 7) / 8;

    bitmap = malloc(bitmap_size);
    if (!bitmap) {
        close(fd);
        return -1;
    }

    lseek(fd, 0, SEEK_SET);
    if (read_metadata(fd, &n, &s, &ubn, &fbn, bitmap, bitmap_size) != 0) {
        free(bitmap);
        close(fd);
        return -1;
    }

    // Find first free block
    for (int i = 0; i < n; ++i) {
        int byte = i / 8;
        int bit = i % 8;
        if (!(bitmap[byte] & (1 << bit))) {
            // Mark as used
            bitmap[byte] |= (1 << bit);
            ubn++;
            fbn--;

            // Fill the block with 1's
            char *data = malloc(s);
            memset(data, 1, s);
            lseek(fd, META_SIZE + (off_t)i * s, SEEK_SET);
            write(fd, data, s);
            free(data);

            write_metadata(fd, n, s, ubn, fbn, bitmap, bitmap_size);
            free(bitmap);
            close(fd);
            return i;
        }
    }

    free(bitmap);
    close(fd);
    return -1; // No free block found
}

// Free a specific block
int free_block(const char *fname, int bno) {
    int fd = open(fname, O_RDWR);
    if (fd < 0) {
        perror("Failed to open file");
        return 0;
    }

    int n, s, ubn, fbn;
    int bitmap_size;
    bitmap_size = (bno + 7) / 8;
    unsigned char bitmap[bitmap_size];

    if (read_metadata(fd, &n, &s, &ubn, &fbn, bitmap, bitmap_size) != 0) {
        close(fd);
        return 0;
    }

    if (bno < 0 || bno >= n) {
        close(fd);
        return 0;
    }

    int byte = bno / 8;
    int bit = bno % 8;

    if (!(bitmap[byte] & (1 << bit))) {
        close(fd); // Block is already free
        return 0;
    }

    // Mark as free
    bitmap[byte] &= ~(1 << bit);
    ubn--;
    fbn++;

    // Fill with 0s
    char *data = malloc(s);
    memset(data, 0, s);
    lseek(fd, META_SIZE + (off_t)bno * s, SEEK_SET);
    write(fd, data, s);
    free(data);

    write_metadata(fd, n, s, ubn, fbn, bitmap, bitmap_size);
    close(fd);
    return 1;
}

// Check file system consistency
int check_fs(const char *fname) {
    int fd = open(fname, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open file");
        return 1;
    }

    int n, s, ubn, fbn;
    int bitmap_size;
    unsigned char *bitmap;

    read(fd, &n, sizeof(int));
    read(fd, &s, sizeof(int));
    read(fd, &ubn, sizeof(int));
    read(fd, &fbn, sizeof(int));
    bitmap_size = (n + 7) / 8;
    bitmap = malloc(bitmap_size);
    if (!bitmap) {
        close(fd);
        return 1;
    }

    lseek(fd, 16, SEEK_SET);
    read(fd, bitmap, bitmap_size);

    int count = 0;
    for (int i = 0; i < n; ++i) {
        int byte = i / 8;
        int bit = i % 8;
        if (bitmap[byte] & (1 << bit)) count++;
    }

    free(bitmap);
    close(fd);

    if (ubn + fbn != n || count != ubn) return 1;
    return 0;
}

// Main demonstration
int main() {
    const char *filename = "dd1";
    int block_size = 4096;
    int block_count = 2048;

    if (init_File_dd(filename, block_size, block_count) == 0)
        printf("Initialized file successfully.\n");

    int blk = get_freeblock(filename);
    if (blk >= 0)
        printf("Allocated block: %d\n", blk);
    else
        printf("No free block available.\n");

    if (free_block(filename, blk))
        printf("Freed block: %d\n", blk);
    else
        printf("Failed to free block.\n");

    if (check_fs(filename) == 0)
        printf("File system is consistent.\n");
    else
        printf("File system inconsistency detected!\n");

    return 0;
}
