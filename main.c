#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/signal.h>
#include <time.h>

#define nLine printf("\n")
#define LOG(X, Y) fprintf (f, #X ": Time:%s, File:%s(%d) " #Y  "\n", __TIMESTAMP__, __FILE__, __LINE__)
#define separator printf("-----------------------------------\n")
#define MAXCOM 1000 // max number of letters to be supported 
#define MAXLIST 100 // max number of commands to be supported 

char command[MAXCOM]; 
char* args[MAXLIST];
int background = 0;
FILE *f;

void handler(int sig) {
        int pid;
        pid = wait(NULL);
        if (pid > 0)
            LOG("INFO", "Child process was terminated");
}

void parser() { 
    char *token = strtok(command, " "); 
    int lArgs = 0;
    while(token[lArgs]!='\0'){
        if(token[lArgs++] == '&'){
            background = 1;
            token[--lArgs] = '\0';
            break;
        }
    }
    lArgs = 0;
    while (token != NULL) { 
        args[lArgs++] = token;
        token = strtok(NULL, " "); 
    } 
    args[lArgs] = NULL;
} 

void initiate() {
    if (system("clear"))
        printf("Eroor clearing terminal");
    f = fopen("logger.log", "w+");

    if (f == NULL) printf("Eroor creating file");
    LOG("INFO", "Shell initiated");

    signal(SIGCHLD,handler);
}

void execute(){

    pid_t pid = fork();

    if(pid < 0){
        printf("Error forking");
    }     
    else if(pid == 0){
        if (execvp(command, args) < 0) { 
            printf("*** ERROR: exec failed\n");
            fclose(f);
            exit(0);
        }
    }
    else {
        if (!background){
            waitpid(pid,0,0);
            LOG("INFO","Child process was terminated");
        }
        background = 0;
    }
}

int main() {
   
   initiate();

    while (1) {
        char cwd[1024]; 
        char* dir = getcwd(cwd, sizeof(cwd)); 
        printf("%s -> ", cwd);  

        while(!scanf("%[^\n]%*c", command)); 
        parser();

        if (!strcmp(args[0], "exit")){
            fclose(f);
            exit(0);
        }

        if (!strcmp(args[0], "cd")){
            char* to;
            if (args[1] == NULL){
                char* ptr = strrchr(dir ,'/');
                *ptr = '\0';  
                to = dir;
            }
            else to = strcat(strcat(dir, "/"), args[1]);
            if (chdir(to))
                printf("Error changing directory");
            continue;
        }        

        execute();

        separator;
    }

    return 0;
}
