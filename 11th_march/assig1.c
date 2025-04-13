#include <stdio.h>



#define BLOCK_SIZE 4096
#define TOTAL_FILES 2048
#define META_BLOCKS 8
#define META_SIZE 16  // 12 for name + 4 for size
#define MAX_NAME_LEN 12


typedef struct {
    char name[MAX_NAME_LEN];
    int size;
}
file_info[2048];


int find_file_index(char *name) {
    for (int i = 0; i <META_BLOCKS; i++) {
        if (strcmp(file_info[i].name, name) ==0) {
            return i;
        }
    }
    return -1;
}


// 1. mymkfs dd1 [makes dd1 ready for storing files]

void mymkfs(char *dd1) {
    FILE *fp = fopen(dd1, "wb+");
    
    
    if (fp == NULL) {
        printf("Error creating file system\n");
        return;
    }

    // Write metadata blocks
    char zero = 0;
    for (int i=0;i<2048;i++){
        fwrite(&zero, sizeof(file_info), 1,file_info[i]);
    }


    for (int i = 0; i < (TOTAL_FILES) * BLOCK_SIZE; i++) {
        fwrite(&zero, 1, 1,fp);
    }

    fclose(fp);
}


// 2. mycopyTo <linux file> dd1 [copies a linux file to dd1]

void mycopyTo(char *src,char *dest,char *dd1){
    
    //find src file and dest file  
    int start = find_file_index(src);
    if (start  -1) {
        printf("File doesnot exists\n");
        return;
    }
    int end = find_file_index(dest);
    if (end  -1) {
        printf("File doesnot exists\n");
        return;
    }

    // go to file and get the content fo source file and copy to dest file


    FILE *fp = fopen(dd1, "wb+");

    if (fp == NULL) {
        printf("Error opening source file\n");
        return;
    }

    lseek(fp, 0, SEEK_END); // Move to the end of the file

    char *buffer = malloc(BLOCK_SIZE);
    if (buffer == NULL) {
        printf("Error allocating memory\n");
        fclose(fp);
        return;
    }

    int bytesRead = fread(buffer, 1, BLOCK_SIZE, fp);
    if (bytesRead < 0) {
        printf("Error reading source file\n");
        free(buffer);
        fclose(fp);
        return;
    }

    fwrite(buffer, 1, bytesRead, dest);
    free(buffer);
    fclose(fp);
    printf("File copied successfully\n");
}

// 3. mycopyFrom <file name>@dd1 [copies a file from dd1 to a linux file of same name.]

void mycopyFrom(char *src,char *dest,char *dd1){
    
    //find src file and dest file  
    int start = find_file_index(src);
    if (start  -1) {
        printf("File doesnot exists\n");
        return;
    }
    int end = find_file_index(dest);
    if (end  -1) {
        printf("File doesnot exists\n");
        return;
    }

    // go to file and get the content fo source file and copy to dest file


    FILE *fp = fopen(src, "rb+");   
    

}




// 4.  myrm <file name>@dd1 [removes a file from dd1]




int main() {


//    printf("Hello, World!\n");
    return 0;
}