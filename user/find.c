#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find(char* cur_dir, char *target);

void
find(char* cur_dir, char *target)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(cur_dir, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", cur_dir);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", cur_dir);
        close(fd);
        return;
    }

    if (st.type != T_DIR) {
        fprintf(2, "find: %s is not a directory\n", cur_dir);
        close(fd);
        return;
    }

    if(strlen(cur_dir) + 1 + DIRSIZ + 1 > sizeof buf){
        printf("find: path too long\n");
    }
    strcpy(buf, cur_dir);
    p = buf + strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if (de.inum == 0)
            continue;

        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
            continue;
        }
            
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        
        if(stat(buf, &st) < 0){
            printf("find: cannot stat %s\n", buf);
            continue;
        }
        if (st.type == T_DIR) {
            find(buf, target);
        }
        if (strcmp(de.name, target) != 0)
            continue;
        printf("%s\n", buf);
    }
    close(fd);
}


int
main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3) {
        fprintf(2, "Usage: find Optional<directory> <target> \n");
        exit(1);
    }
    char *p;
    char cur_dir[512] = ".";
    char *target = argc == 3 ? argv[2] : argv[1];
    if(argc == 3 && strcmp(argv[1], ".") != 0){
        p = cur_dir + strlen(cur_dir);
        *p++ = '/';
        memmove(p, argv[1], strlen(argv[1]));
        p[strlen(argv[1])] = 0;
    } 
    find(cur_dir, target);
    exit(0);
}