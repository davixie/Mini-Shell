#include "ex1.h"

typedef int bool;

 /*****************************Private Function declaration******************************/
char *getcwd(char *buf, size_t size);//show the path Of the current folder
// void  DisplayPrompt();//Display Prompt : user@currect dir>
// char** getArgv(char *input,char **argv,int *sizeOfArray,int *cmdLength, int count);  //Preparation of a receiver input as an expense
// void garbageCollector(char** argv,int size); //Memory Release
// int *countInput(char *input,int *cmdLength);
// void executeCommand(char *input,char **argv,int sizeOfArray);

 /****************************/
// static int *numOfCmd;
// static int *cmdLength;
/****************************/
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
            printf("\n aqui temos | \n");
        } else if(exist_token[2] == true){
            printf("\n aqui temos > \n");
        } else if(exist_token[3] == true){
            printf("\n aqui temos < \n");
        } else{
            executeCommand(input, argv, sizeOfArray);
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
    int i=RESET, counter=RESET;
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
        } else if(strcmp(ptr, com_stdout) == 0){
            exist_token[2] = true;
        }else if(strcmp(ptr, com_stdin) == 0){
            exist_token[3] = true;
        }
        ptr=strtok(NULL,CUTTING_WORD);
        counter++;
    }
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

void executeCommand(char *input,char **argv,int sizeOfArray){
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
            execvp(argv[RESET],argv);
            garbageCollector(argv,sizeOfArray);
            exit(1);
        }else {
            wait(&id);
        }
    }
}
