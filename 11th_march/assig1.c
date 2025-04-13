#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 4096
#define TOTAL_FILES 2048
#define META_BLOCKS 8
#define META_SIZE 16
#define MAX_NAME_LEN 12

typedef struct {
    char name[MAX_NAME_LEN];
    int size;
} FileInfo;

FileInfo file_info[TOTAL_FILES];

int find_file_index(char *name) {
    for (int i = 0; i < TOTAL_FILES; i++) {
        if (strcmp(file_info[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void mymkfs(char *dd1) {
    FILE *fp = fopen(dd1, "wb+");
    if (fp == NULL) {
        printf("Error creating file system\n");
        return;
    }

    for (int i = 0; i < TOTAL_FILES; i++) {
        file_info[i].name[0] = '\0';
        file_info[i].size = 0;
    }

    fwrite(file_info, sizeof(FileInfo), TOTAL_FILES, fp);

    char zero_block[BLOCK_SIZE] = {0};
    for (int i = 0; i < TOTAL_FILES; i++) {
        fwrite(zero_block, 1, BLOCK_SIZE, fp);
    }

    fclose(fp);
    printf("File system created successfully\n");
}

void mycopyTo(char *src, char *dd1) {
    FILE *src_fp = fopen(src, "rb");
    if (src_fp == NULL) {
        printf("Source file does not exist\n");
        return;
    }

    FILE *dd_fp = fopen(dd1, "rb+");
    if (dd_fp == NULL) {
        printf("File system not found\n");
        fclose(src_fp);
        return;
    }

    fread(file_info, sizeof(FileInfo), TOTAL_FILES, dd_fp);

    int index = -1;
    for (int i = 0; i < TOTAL_FILES; i++) {
        if (file_info[i].name[0] == '\0') {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("No space available in file system\n");
        fclose(src_fp);
        fclose(dd_fp);
        return;
    }

    char buffer[BLOCK_SIZE] = {0};
    int bytes_read = fread(buffer, 1, BLOCK_SIZE, src_fp);
    if (bytes_read < 0) {
        printf("Error reading source file\n");
        fclose(src_fp);
        fclose(dd_fp);
        return;
    }

    strncpy(file_info[index].name, src, MAX_NAME_LEN);
    file_info[index].size = bytes_read;

    fseek(dd_fp, 0, SEEK_SET);
    fwrite(file_info, sizeof(FileInfo), TOTAL_FILES, dd_fp);

    fseek(dd_fp, META_BLOCKS * BLOCK_SIZE + index * BLOCK_SIZE, SEEK_SET);
    fwrite(buffer, 1, BLOCK_SIZE, dd_fp);

    fclose(src_fp);
    fclose(dd_fp);
    printf("File copied to dd1 successfully\n");
}

void mycopyFrom(char *name, char *dd1) {
    FILE *dd_fp = fopen(dd1, "rb");
    if (dd_fp == NULL) {
        printf("File system not found\n");
        return;
    }

    fread(file_info, sizeof(FileInfo), TOTAL_FILES, dd_fp);

    int index = find_file_index(name);
    if (index == -1) {
        printf("File not found in dd1\n");
        fclose(dd_fp);
        return;
    }

    FILE *dest_fp = fopen(name, "wb");
    if (dest_fp == NULL) {
        printf("Error creating destination file\n");
        fclose(dd_fp);
        return;
    }

    char buffer[BLOCK_SIZE] = {0};
    fseek(dd_fp, META_BLOCKS * BLOCK_SIZE + index * BLOCK_SIZE, SEEK_SET);
    fread(buffer, 1, file_info[index].size, dd_fp);
    fwrite(buffer, 1, file_info[index].size, dest_fp);

    fclose(dest_fp);
    fclose(dd_fp);
    printf("File copied from dd1 successfully\n");
}

void myrm(char *filename, char *dd1) {
    FILE *fp = fopen(dd1, "rb+");
    if (fp == NULL) {
        printf("Error opening file system\n");
        return;
    }

    fread(file_info, sizeof(FileInfo), TOTAL_FILES, fp);

    int fileIndex = find_file_index(filename);
    if (fileIndex == -1) {
        printf("File does not exist\n");
        fclose(fp);
        return;
    }

    file_info[fileIndex].name[0] = '\0';
    file_info[fileIndex].size = 0;

    fseek(fp, 0, SEEK_SET);
    fwrite(file_info, sizeof(FileInfo), TOTAL_FILES, fp);

    char zero[BLOCK_SIZE] = {0};
    fseek(fp, META_BLOCKS * BLOCK_SIZE + fileIndex * BLOCK_SIZE, SEEK_SET);
    fwrite(zero, 1, BLOCK_SIZE, fp);

    fclose(fp);
    printf("File removed successfully\n");
}

int main() {
    char command[50], arg1[50], arg2[50];

    while (1) {
        printf("Enter command: ");
        scanf("%s", command);

        if (strcmp(command, "mymkfs") == 0) {
            scanf("%s", arg1);
            mymkfs(arg1);
        } else if (strcmp(command, "mycopyTo") == 0) {
            scanf("%s %s", arg1, arg2);
            mycopyTo(arg1, arg2);
        } else if (strcmp(command, "mycopyFrom") == 0) {
            scanf("%s %s", arg1, arg2);
            mycopyFrom(arg1, arg2);
        } else if (strcmp(command, "myrm") == 0) {
            scanf("%s %s", arg1, arg2);
            myrm(arg1, arg2);
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            printf("Invalid command\n");
        }
    }

    return 0;
}
