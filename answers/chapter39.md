# Chapter 38. Interlude: Files and Directories
## 1.
```c++
#include <stdio.h>     // fprintf, perror
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>    // exit, EXIT_FAILURE, EXIT_SUCCESS
#include <time.h>      // ctime

int main(int argc, char *argv[]) {
    struct stat sb;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (stat(argv[1], &sb) == -1) {
        perror("stat call failed");
        exit(EXIT_FAILURE);
    }

    printf("File size:\t\t\t\t%lld bytes\n", (long long) sb.st_size);
    printf("Inode number:\t\t\t\t%ld\n", (long) sb.st_ino);
    printf("Link count:\t\t\t\t%ld\n", (long) sb.st_nlink);
    printf("Owner of the file:\t\t\tUID=%ld GID=%ld\n", (long) sb.st_uid, (long) sb.st_gid);
    printf("Block size:\t\t\t\t%ld bytes\n", (long) sb.st_blksize);
    printf("Blocks allocated:\t\t\t%lld\n", (long long) sb.st_blocks); /* block size */
    printf("Time of last file status change:\t%s", ctime(&sb.st_ctime));
    printf("time of last access:\t\t\t%s", ctime(&sb.st_atime));
    printf("time of last data modification:\t\t%s", ctime(&sb.st_mtime));

    exit(EXIT_SUCCESS);
}
```
```
froggagul@ubuntu:~/ostep/answers/39$ ./stat stat.c
File size:                              1155 bytes
Inode number:                           798027
Link count:                             1
Owner of the file:                      UID=1000 GID=1000
Block size:                             4096 bytes
Blocks allocated:                       8
Time of last file status change:        Sun Dec  6 23:23:13 2020
time of last access:                    Sun Dec  6 23:23:15 2020
time of last data modification:         Sun Dec  6 23:23:13 2020
```
## 3.
```c++
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    struct stat sb;
    int fd, offset, lines;
    char * pathname = "";

    if (argc != 3 || strlen(argv[1]) <= 1 || argv[1][0] != '-') {
        fprintf(stderr, "Usage: %s -<offset> <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    lines = -atoi(argv[1]) + 1;
    pathname = argv[2];

    if (stat(pathname, &sb) == -1) {
        perror("stat failed");
        exit(EXIT_FAILURE);
    }

    if ((fd = open(pathname, O_RDONLY)) == -1) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    if (lseek(fd, -1, SEEK_END) == -1) {
        perror("lseek failed");
        exit(EXIT_FAILURE);
    }

    char buff[sb.st_size];
    while (lines > 0) {
        if (read(fd, buff, 1) == -1) {
            fprintf(stderr, "read failed at %d\n", lines);
            exit(EXIT_FAILURE);
        }
        if (buff[0] == '\n')
            lines--;
        offset = lseek(fd, -2, SEEK_CUR);
        if (offset == -1)
            break;
    }

    if (offset > 0 || lines == 0) {
        if (lseek(fd, 2, SEEK_CUR) == -1) {
            perror("lseek failed");
            exit(EXIT_FAILURE);
        }
    } else {
        if (lseek(fd, 0, SEEK_SET) == -1){
            perror("lseek failed");
            exit(EXIT_FAILURE);
        }
    }

    memset(buff, 0, sb.st_size);
    if (read(fd, buff, sb.st_size) == -1) {
        perror("read failed");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", buff);
    close(fd);

    exit(EXIT_SUCCESS);
}
```
```
froggagul@ubuntu:~/ostep/answers/39$ ./mytail -5 mytail.c

    printf("%s\n", buff);
    close(fd);

    exit(EXIT_SUCCESS);
}
```