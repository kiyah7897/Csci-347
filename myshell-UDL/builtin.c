#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "builtin.h"
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/sysmacros.h>


//Prototypes
static void exitProgram(char** args, int argcp);
static void cd(char** args, int argpcp);
static void pwd(char** args, int argcp);

//Group A
//static void cmdA_ls(char** args, int argc);
//static void cmdA_cp(char** args, int argc);
//static void cmdA_env(char** args, int argc);

//Group B
static void cmdB_stat(char** args, int argc);
static void cmdB_tail(char** args, int argc);
static void cmdB_touch(char** args, int argc);

static void cmdB_stat(char** args, int argc){
  if(argc < 2){
    fprintf(stderr, "stat: missing file operand\n");
    return;
  }
  for(int i = 1 ; i < argc; i++){
    struct stat sb;
    if (stat(args[i], &sb) == -1){
      perror(args[i]);
      continue;
    }
    printf("File Name: %s\n", args[i]);
    printf("Size: %lld \tBlocks: %lld \tIO Block: %lld" ,(long long)sb.st_size ,
    (long long)sb.st_blocks , (long long)sb.st_blksize);
    
    if(S_ISREG(sb.st_mode)){
      printf(" regular file\n");
    }else if( S_ISDIR(sb.st_mode)){
      printf(" directory\n");
    }else{
      printf(" other\n");
    }
    printf("Device %u,%u\tInode: %llu\tLinks: %llu\n" , 
    major(sb.st_dev), minor(sb.st_dev), (unsigned long long)sb.st_ino, (unsigned long long)sb.st_nlink);
    
    printf("Permissions: (%o/" , sb.st_mode & 0777);

    char permissions[11];
    permissions[0] = S_ISDIR(sb.st_mode) ? 'd' : '-';
    permissions[1] = (sb.st_mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (sb.st_mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (sb.st_mode & S_IXUSR) ? 'x' : '-';
    permissions[4] = (sb.st_mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (sb.st_mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (sb.st_mode & S_IXGRP) ? 'x' : '-';
    permissions[7] = (sb.st_mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (sb.st_mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (sb.st_mode & S_IXOTH) ? 'x' : '-';
    permissions[10] = '\0';
    printf("%s)\t" , permissions);

    struct passwd *pw = getpwuid(sb.st_uid);
    const char *userName = (pw != NULL) ? pw->pw_name : "UNKNOWN" ;
    
    struct group *gr = getgrgid(sb.st_gid);
    const char *groupName = (gr != NULL) ? gr->gr_name : "UNKNOWN";
    
    printf("UID: (%u, %s)\tGID: (%u, %s)\n",
    (unsigned)sb.st_uid, userName, (unsigned)sb.st_gid,groupName);
    printf("Access: %s", ctime(&sb.st_atime));
    printf("Modify: %s", ctime(&sb.st_mtime));
    printf("Change: %s", ctime(&sb.st_ctime));
  }

}

static void cmdB_tail(char** args, int argc){
  if (argc < 2){
    fprintf(stderr, "tail: missing file operand\n");
    return;
  }

  for (int i = 0; i < argc; i++){
    FILE *fp = fopen(args[i] , "r");
    if(fp == NULL){
      perror(args[i]);
      continue;
    }
    if (argc > 2){
      if(i > 1){
        printf("\n");
      }
      printf("==> %s <==\n", args[i]);
    }
    char *lines[10] = {NULL};
    size_t size[10] = {0};
    long count = 0;

    while(getline(&lines[count %10], &size[count % 10], fp) != -1){
      count++;
    }
    long n = (count < 10) ? count: 10;
    long start = (count < 10) ? 0 : count % 10;

    for(long j = 0; j < n; j++){
      fputs(lines[(start + j) % 10] , stdout);
    }
    for(int k = 0; k < 10; k++){
      free(lines[k]);
    }
    fclose(fp);
    }
  }

static void cmdB_touch(char** args, int argc){
  if (argc < 2){
    fprintf(stderr,  "touch: missing files operand\n");
    return;
  }
  struct stat sb;
  for (int i = 1; i < argc; i++){
   if (stat(args[i] , &sb) == -1){
    if (errno == ENOENT){
      int fd = open(args[i], O_CREAT | O_WRONLY, 0644);
      if (fd == -1){
        perror(args[i]);
      }else{
        close(fd);
      }
    }else{
      perror(args[i]);
    }
    }else{
      if(utimensat(AT_FDCWD , args[i], NULL , 0) == -1){
        perror(args[i]);
      }
   } 
  }
}
/*
* builtIn
*
* Checks if a command is a built-in shell command and executes it if so.
*
* Args:
*   args: array of strings containing command and arguments
*   argcp: number of elements in args array
*
* Returns:
*   1 if the command was a built-in, 0 otherwise.
*
* Built-in commands are executed directly by the shell process rather than
* being forked to a new process. This function compares the given command
* to each of the built-ins (exit, pwd, cd, and ls/cp/env or stat/tail/touch
* depending on group). If a match is found, the corresponding function is called.
*
* Hint: Refer to checklist for group specific examples
*/
int builtIn(char** args, int argcp)
{
    //write your code
    if(args == NULL || argcp == 0){
      return 0;
    }

    if(strcmp(args[0] , "stat" )== 0){
      cmdB_stat(args, argcp);
      return 1;
    }else if(strcmp(args[0] , "tail")==0){
      cmdB_tail(args, argcp);
      return 1;
    }else if(strcmp(args[0], "touch" )== 0){
      cmdB_touch(args, argcp);
      return 1;
    }else if(strcmp(args[0], "exit") == 0){
      exitProgram(args, argcp);
      return 1;
    }else if(strcmp(args[0], "pwd")== 0){
      pwd(args, argcp);
      return 1;
    }else if(strcmp(args[0], "cd") == 0){
        cd(args, argcp);
        return 1;
    }else{
      return 0;
    }
    }

/*
* exitProgram
*
* Terminates the shell with a given exit status.
* If no exit status is provided, exits with status 0.
* This function should use the exit(3) library call.
*
* Args:
*   args: array of strings containing "exit" and optionally an exit status
*   argcp: number of elements in args array
*/
static void exitProgram(char** args, int argcp){
 //write your code
 if(argcp >= 3 ){
  fprintf(stderr ," Error too many arguments\n");
  return;
 }
  if(argcp  == 1){
  exit(0);
}
  char *end;
  int value = strtol(args[1] ,&end , 10);
  exit(value);
}

/*
* pwd
*
* Prints the current working directory.
*
* Args:
*   args: array of strings containing "pwd"
*   argcp: number of elements in args array, should be 1
*
* Example Usage:
*   Command: $ pwd
*   Output: /some/path/to/directory
*/
static void pwd(char** args, int argpc){
  //write your code
  if(argpc != 1){
    fprintf(stderr , "Too many arguments\n");
    return;
  }
  char *cwd = getcwd(NULL , 0 );
  if(cwd == NULL){
    perror("pwd");
    return;
  }
  printf("%s\n" , cwd);
  free(cwd);
}

/*
* cd
*
* Changes the current working directory.
* When no parameters are provided, changes to the home directory.
* Supports . (current directory) and .. (parent directory).
*
* Args:
*   args: array of strings containing "cd" and optionally a directory path
*   argcp: number of elements in args array
*
* Example Usage:
*   Command: $ pwd
*   Output: /some/path/to/directory
*   Command: $ cd ..
*   Command: $ pwd
*   Output: /some/path/to
*
* Hint: Read the man page for chdir(2)
*/
static void cd(char** args, int argcp){
 //write your code
  if(argcp > 2){
    fprintf(stderr, "cd: too many arguments\n");
    return;
  }
  const char *path;
  if(argcp == 1){
  path = getenv("HOME");
  if(path == NULL){
    fprintf(stderr, "cd: HOME not set \n");
    return;
    }
  }else{
  path = args[1];
  }
  if(chdir(path) == -1){
    perror("cd");
  }
}
