#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

char* get_filename(char *path) {
    char *p;
    for(p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    return p;
}

void find(char *path, char *target, int exec_mode, char **exec_argv, int exec_argc) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){
        case T_FILE:
            if(strcmp(get_filename(path), target) == 0){
                if(exec_mode){
                    exec_argv[exec_argc] = path;
                    exec_argv[exec_argc + 1] = 0;
                    if(fork() == 0){
                        exec(exec_argv[0], exec_argv);
                        fprintf(2, "find: exec failed\n");
                        exit(1);
                    } else {
                        wait(0);
                    }
                } else {
                    printf("%s\n", path);
                }
            }
            break;

        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0)
                    continue;
                if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                    continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                find(buf, target, exec_mode, exec_argv, exec_argc);
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if(argc < 3){
        fprintf(2, "Usage: find path target [-exec cmd...]\n");
        exit(1);
    }

    char *exec_argv[MAXARG];
    int exec_mode = 0;
    int exec_argc = 0;

    if(argc >= 4 && strcmp(argv[3], "-exec") == 0){
        exec_mode = 1;
        for(int i = 4; i < argc; i++){
            exec_argv[exec_argc++] = argv[i];
        }
    }

    find(argv[1], argv[2], exec_mode, exec_argv, exec_argc);
    exit(0);
}