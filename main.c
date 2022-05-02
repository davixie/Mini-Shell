#include "main.h"

typedef int bool;

 /*****************************Private Function declaration******************************/
char *getcwd(char *buf, size_t size);//show the path Of the current folder

int main() {
    /*****************************************************************/
    numOfCmd = mmap(NULL, sizeof *numOfCmd, PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, RESET);
    cmdLength = mmap(NULL, sizeof *cmdLength, PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_ANONYMOUS, -1, RESET);
    /******************************************************************/
    (*numOfCmd)=RESET;
    (*cmdLength)=RESET;
    int sizeOfArray=RESET;

    char input[INPUT_SIZE]="";//A string array containing the input.
    DisplayPrompt();
    pid_t id; // pid_t use for process identifer
    char **argv;//A string array will containing the program name and command argument
    while (strcmp(input,ENDING_WORD)!=RESET)
    {   
        if(fgets(input,INPUT_SIZE,stdin)==RESET)
            printf(" ");
        //do nothing...countine regular
        char token[] = " ";

        int *exist_token;
        exist_token = (int*)malloc((5)*(sizeof(int*)));
        char inputCopy[INPUT_SIZE];
        strcpy(inputCopy,input);
        exist_token = countInput(inputCopy, cmdLength);

        argv=getArgv(input,argv,&sizeOfArray,cmdLength,exist_token[0]);

        if(exist_token[1] == true){
            executePipeRedirect(input, argv, sizeOfArray, exist_token);
        } else if(exist_token[2] == true){
            executeOut(input, argv, sizeOfArray, exist_token);
        } else if(exist_token[3] == true){
            printf("\n aqui temos < \n");
            executeIn(input, argv, sizeOfArray, exist_token);
        } else{
            // printando_aux(argv, sizeOfArray);
            executeCommand(argv, sizeOfArray);
        }

        if (strcmp(input,ENDING_WORD) != RESET){
            DisplayPrompt();
        }

    }
    return RESET;
}

void garbageCollector(char** argv,int size)
{
    int i=RESET;
    for (i = RESET; i < size; ++i) {
        free(argv[i]);
    }
    free(argv);
    argv=NULL;
}

int *countInput(char *input,int *cmdLength){
    int i=RESET, counter=RESET, separator=RESET;
    bool separator_bool = false;
    int *exist_token;
    exist_token = (int*)malloc((5)*(sizeof(int*)));

    char inputCopy[INPUT_SIZE];

    char com_redirection[] = pipe_redirection;
    char com_stdout[] = pipe_stdout;
    char com_stdin[] = pipe_stdin;

    strcpy(inputCopy,input);

    char* ptr= strtok(input,CUTTING_WORD);
    
    while(ptr!=NULL)
    {
        if(strcmp(ptr, com_redirection) == 0){
            exist_token[1] = true;
            separator_bool = true;
        } else if(strcmp(ptr, com_stdout) == 0){
            exist_token[2] = true;
            separator_bool = true;
        }else if(strcmp(ptr, com_stdin) == 0){
            exist_token[3] = true;
            separator_bool = true;
        }
        ptr=strtok(NULL,CUTTING_WORD);
        counter++;
        if(separator_bool == false)
            separator++;
    }
    exist_token[4] = separator;
    exist_token[0] = counter;

    return exist_token;
}

char** getArgv(char *input,char **argv,int *sizeOfArray,int *cmdLength, int counter)
{
    int i=RESET;

    argv = (char**)malloc((counter+1)*(sizeof(char*)));
    if(argv==NULL)
    {
        printf("error allocated");
        exit(RESET);
    }

    char* ptr = strtok(input,CUTTING_WORD);
    while(ptr !=NULL)
    {
        if (i==RESET)
            (*cmdLength)+=strlen(ptr);
        argv[i]=(char*)malloc((sizeof(char)+1)*strlen(ptr));
        if(argv[i]==NULL){
            printf("error allocated");
            for (int j = i-1; j >-1 ; j--) {
                free(argv[j]);
            }
            free(argv);
            exit(RESET);
        }
        strcpy(argv[i],ptr);
        argv[i][strlen(ptr)]='\0';
        ptr=strtok(NULL,CUTTING_WORD );
        i++;
    }
    argv[counter]=NULL;
    (*sizeOfArray)=counter;
    return argv;

}

void DisplayPrompt()
{
    long size;
    char *buf;
    char *ptr;

    size = pathconf(".", _PC_PATH_MAX);

    if ((buf = (char *)malloc((size_t)size)) != NULL)
        ptr = getcwd(buf, (size_t)size);

    //----------show the user name root------------------------

    struct passwd *getpwuid(uid_t uid);
    struct passwd *p;
    uid_t uid=0;
    if ((p = getpwuid(uid)) == NULL)
        perror("getpwuid() error");
    else {
        printf("%s@%s>", p->pw_name, ptr);
    }
    free(buf);
}

// -----------------------------------------------------------------

void executeCommand(char **argv,int sizeOfArray){
    pid_t id; 
    if (strcmp("cd",argv[RESET])==RESET){
        struct passwd *pwd;
        char* path=argv[1];

        if(path==NULL){
            pwd=getpwuid(getuid());
            path=pwd->pw_dir;
        }
        if(path[0]=='/')
            (path)=++(path);
        errno=chdir(path);
        if(errno!=RESET)
            printf("error changing directory");
    }

    else{
        id=fork();
        if (id<RESET){
            printf("fork failed");
            exit(RESET);
        }
        else if(id==RESET) {
            (*numOfCmd)++;
            // printf("Aqui temos argv[0] = %s\n", argv[RESET]);
            execvp(argv[RESET],argv);
            // printf("após o execvp\n");
            garbageCollector(argv,sizeOfArray);
            exit(1);
        }else {
            wait(&id);
        }
    }
}

void executePipeRedirect(char *input, char **argv, int sizeOfArray, int *exist_token){
    char inputCopy[INPUT_SIZE];
    strcpy(inputCopy,input);
    char **argv_1;
    argv_1 = (char**)malloc((exist_token[4]+1)*(sizeof(char*)));
    char **argv_2;
    argv_2 = (char**)malloc((exist_token[0]-exist_token[4])*(sizeof(char*)));
    for(int i = 0; i < exist_token[0]; i++){
        if(i < exist_token[4]){
            argv_1[i]=(char*)malloc((sizeof(char)+1)*strlen(argv[i]));
            strcpy(argv_1[i],argv[i]);
            argv_1[i][strlen(argv[i])]='\0';
        }else if(i != exist_token[4]){ // condition to escape the pipe character
            argv_2[i-exist_token[4]-1]=(char*)malloc((sizeof(char)+1)*strlen(argv[i]));
            strcpy(argv_2[i-exist_token[4]-1],argv[i]);
            argv_2[i-exist_token[4]-1][strlen(argv[i])]='\0';
        }
    }
    argv_1[exist_token[4]] = NULL;
    argv_2[exist_token[0]-exist_token[4]-1] = NULL;
    // printf("inicio\n");
    // printando_aux(argv_2, exist_token[0]-exist_token[4]-1);
    // executeCommand(argv_2, exist_token[0]-exist_token[4]-1);
    int pipefd[2];
    pipe(pipefd);
    // printf("aqui em -1\n");
    pid_t child_pid = fork();
    // printf("aqui em 0\n");
    if(child_pid == 0){ // child
        // printf("entrou em primeiro if\n");
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        execvp(argv_1[RESET],argv_1);
        // printf("erro aqui em cima\n");
        perror("excves\n");
    } else{ // parent
        // pi
        // printf("entrou em else para wait\n");
        wait(NULL);
        // printf("aqui em executando o 1 parent\n");
        pid_t child_child_pid = fork();
        // printf("aqui em 2\n");
        if(child_child_pid == 0){
            // printf("aqui em segundo if\n");
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            execvp(argv_2[RESET], argv_2);
            // printf("erro aqui embaixo\n");
            perror("excves\n");
        }else{
            // printf("entrou em else para wait 2\n");
            close(pipefd[0]);
            close(pipefd[1]);
            wait(NULL);
            // printf("aqui em executando o 2 parent\n");
            close(pipefd[0]);
            close(pipefd[1]);
        }
        close(pipefd[0]);
        close(pipefd[1]);
    }
    // printf("\n aqui pré final\n");
    garbageCollector(argv_1,exist_token[4]);
    garbageCollector(argv_2,exist_token[0]-exist_token[4]-1);

    // printf("\n aqui final\n");
}

void printando_aux(char **argv, int size){
    int i=RESET;
    printf("aqui temos %d argumentos", size);
    for (i = RESET; i < size; ++i) {
        printf("\n%d) %s\n", i, argv[i]);
    }
    printf("\n%d) %s\n", i, argv[size]);
    argv=NULL;
}

void executeOut(char *input, char ** argv, int sizeOfArray, int *exist_token){
    char inputCopy[INPUT_SIZE];
    strcpy(inputCopy,input);
    char **argv_1;
    char **argv_2;
    int token_position = -1;
    for(int i = 0; i < exist_token[0]; i++){
        if(strstr(argv[i], ">") != 0)
            token_position = i;
    }
    argv_1 = (char**)malloc((token_position+1)*(sizeof(char*)));
    argv_2 = (char**)malloc((exist_token[0]-exist_token[4])*(sizeof(char*)));
    for(int i = 0; i < exist_token[0]; i++){
        if(i < token_position){
            argv_1[i]=(char*)malloc((sizeof(char)+1)*strlen(argv[i]));
            strcpy(argv_1[i],argv[i]);
            argv_1[i][strlen(argv[i])]='\0';
        }else if(i > token_position){ // condition to escape the pipe character
            argv_2[i-token_position-1]=(char*)malloc((sizeof(char)+1)*strlen(argv[i]));
            strcpy(argv_2[i-token_position-1],argv[i]);
            argv_2[i-token_position-1][strlen(argv[i])]='\0';
        }
    }
    argv_1[token_position] = NULL;
    argv_2[exist_token[0]-token_position-1] = NULL;

    pid_t child_pid = fork();
    int fd = open(argv_2[0], O_CREAT | O_RDWR, 0777);
    if(child_pid == 0){ // child
        dup2(fd, 1);
        execvp(argv_1[RESET], argv_1);
        perror("excves\n");
    } else{
        wait(NULL);
    }

    garbageCollector(argv_1,token_position);
    garbageCollector(argv_2,exist_token[0]-token_position-1);
}

void executeIn(char *input, char ** argv, int sizeOfArray, int *exist_token){
    char inputCopy[INPUT_SIZE];
    strcpy(inputCopy,input);
    char **argv_1;
    char **argv_2;
    int token_position = -1;
    for(int i = 0; i < exist_token[0]; i++){
        if(strstr(argv[i], "<") != 0)
            token_position = i;
    }
    argv_1 = (char**)malloc((token_position+1)*(sizeof(char*)));
    argv_2 = (char**)malloc((exist_token[0]-exist_token[4])*(sizeof(char*)));
    for(int i = 0; i < exist_token[0]; i++){
        if(i < token_position){
            argv_1[i]=(char*)malloc((sizeof(char)+1)*strlen(argv[i]));
            strcpy(argv_1[i],argv[i]);
            argv_1[i][strlen(argv[i])]='\0';
        }else if(i > token_position){ // condition to escape the pipe character
            argv_2[i-token_position-1]=(char*)malloc((sizeof(char)+1)*strlen(argv[i]));
            strcpy(argv_2[i-token_position-1],argv[i]);
            argv_2[i-token_position-1][strlen(argv[i])]='\0';
        }
    }
    argv_1[token_position] = NULL;
    argv_2[exist_token[0]-token_position-1] = NULL;

    // printf("--------------inicio\n");
    // printando_aux(argv_1,token_position);
    // printando_aux(argv_2,exist_token[0]-token_position-1);
    // printf("--------------fim\n");

    pid_t child_pid = fork();
    int fd = open(argv_2[0], O_CREAT | O_RDWR, 0777);
    if(child_pid == 0){ // child
        dup2(fd, 0);
        execvp(argv_1[RESET], argv_1);
        perror("excves\n");
    } else{
        wait(NULL);
    }

    garbageCollector(argv_1,token_position);
    garbageCollector(argv_2,exist_token[0]-token_position-1);
}