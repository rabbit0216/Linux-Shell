#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#define MAX_CMD_ARG 10
#define BUFSIZ 256

const char *prompt = "myshell> ";
char *cmdvector[MAX_CMD_ARG];
char  cmdline[BUFSIZ];
char *cmdpipe1[3];
char *cmdpipe2[3];
char *cmdpipe3[3];
int check = 0;

void fatal(char *str){
	perror(str);
	exit(1);
}

int makelist(char *s, const char *delimiters, char** list, int MAX_LIST){	
  int i = 0;
  int numtokens = 0;
  char *snew = NULL;

  if( (s==NULL) || (delimiters==NULL) ) return -1;

  snew = s + strspn(s, delimiters);	/* Skip delimiters */
  if( (list[numtokens]=strtok(snew, delimiters)) == NULL )
    return numtokens;
	
  numtokens = 1;
  
  while(1){
     if( (list[numtokens]=strtok(NULL, delimiters)) == NULL)
	break;
     if(numtokens == (MAX_LIST-1)) return -1;
     numtokens++;
    
    
  }
  return numtokens;
}

//signal
void sig_handler(int sig){
  if(sig==SIGINT){
    printf("\n");
  }
  if(sig==SIGQUIT){
    printf("\n");
  }
  if(sig==SIGTSTP){
    printf("\n");
  }
}

void child_handler(int sig){
  int status;
  pid_t pid;
  while((pid=waitpid(-1, &status, WNOHANG)) > 0) {}
}

//redirection
int red_out(char **cmd){
  int i;
  int fd;
  for(i=0;cmd[i]!=NULL;i++){
    if(!strcmp(cmd[i], ">")) break;
  }
  if(cmd[i]){
    if(!cmd[i+1]) return -1;
    else{
      if((fd=open(cmd[i+1], O_RDWR|O_CREAT, 0644)) == -1){
        perror(cmd[i+1]);
        return -1;
      }
    }
    dup2(fd,1);
    close(fd);
    cmd[i]=NULL;
    cmd[i+1]=NULL;
    for(i=i;cmd[i]!=NULL;i++){
      cmd[i]=cmd[i+2];
    }
    cmd[i]=NULL;
  }
  return 0;
}

int red_in(char **cmd){
  int i;
  int fd;

  for(i=0;cmd[i]!=NULL;i++){
    if(!strcmp(cmd[i],"<")) break;
  }

  if(cmd[i]){
    if(!cmd[i+1]) return -1;
    else{
      if((fd=open(cmd[i+1],O_RDONLY)) == -1){
        perror(cmd[i+1]);
        return -1;
      }
    }
    dup2(fd,0);
    close(fd);
    for(i=i;cmd[i]!=NULL;i++){
      cmd[i]=cmd[i+2];
    }
    cmd[i]=NULL;
    }
  return 0;
}

//pipe
int my_pipe(char **cmd){
  int i;
  int n=0;
  pid_t pid1, pid2;
  int fd[2];

  //division by |
  for(i=0;cmd[i]!=NULL;i++){
    if(!strcmp(cmd[i],"|")){
      cmdpipe1[i] = NULL;
    }
    cmdpipe1[i] = cmd[i];
  }

  for(i=i+1;cmd[i]!=NULL;i++){
      cmdpipe2[n]=cmd[i];
      n++;
  }
  cmdpipe2[i] = NULL;

  pipe(fd);
  
  switch(pid1=fork()){
    case -1:
      perror("error!");
      break;
    case 0:
    if(check==1)
        red_in(cmdpipe1);

      dup2(fd[1],1);
      close(fd[1]);
      close(fd[0]);
      execvp(cmdpipe1[0], cmdpipe1);
      fatal("pid1");
      exit(1);
  }
  switch(pid2=fork()){
    case -1:
      perror("error!");
      break;
    case 0:
    if(check==2)
        red_out(cmdpipe2);
      dup2(fd[0],0);
      close(fd[1]);
      close(fd[0]);
      execvp(cmdpipe2[0],cmdpipe2);
      fatal("pid2");
      exit(1);
  }
  return 0;
}

int main(int argc, char**argv){
  int i=0;
  pid_t pid;
  char* menu;
  while (1) {
  fputs(prompt, stdout);
	fgets(cmdline, BUFSIZ, stdin);
	cmdline[strlen(cmdline) -1] = '\0';
  i = makelist(cmdline, " \t", cmdvector, MAX_CMD_ARG);
  int background = 0;


  //signal 처리
  signal(SIGCHLD, (void *)child_handler);
  signal(SIGINT, sig_handler); 
  signal(SIGQUIT, sig_handler); 
  signal(SIGTSTP, sig_handler); 

  //exit
  if(!strcmp("exit", cmdline))
    exit(1);

  //cd
  if(strcmp(cmdvector[0], "cd") == 0){
      if(chdir(cmdvector[1]) != 0)
        perror("cd");
      continue;
  }

  //background
  if(strcmp(cmdvector[i-1], "&") == 0){
    cmdvector[i-1] = NULL;
    background=1;
  }

  //redirection, pipe 
  for(int j=0;j<i;j++){
     if (strstr(cmdvector[j], "|") != NULL) {
       check = 3;
     }
  }
  
  if(check == 3){
    my_pipe(cmdvector);
    continue;
  }
  
  red_in(cmdvector);
  red_out(cmdvector);


  switch(pid=fork()){
    case 0:
      execvp(cmdvector[0], cmdvector);
      fatal("main()");
    case -1:
        fatal("main()");
    default:
      if(!background){
        wait(NULL);
      }
    }
  }
  return 0;
}

