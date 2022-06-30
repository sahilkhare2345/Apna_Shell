// C Program to design a shell in Linux
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <errno.h>

#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported

// Clearing the shell using escape sequences
#define clear() printf("\033[H\033[J")

// Greeting shell during startup
void init_shell()
{
    clear();
    printf("\n\n\n\n**"
           "**");
    printf("\n\n\n\t*Welcome to APNA SHELL*");
    printf("\n\n\t-USE AT YOUR OWN RISK-");
    printf("\n\n\n\n***"
           "*");
    char *username = getenv("USER");
    printf("\n\n\nUSER is: @%s", username);
    printf("\n");
    sleep(3);
    clear();
    printf("\n\n--Musibat mai ho toh `madad` mango :)");
}

// Function to take input
int takeInput(char *str)
{
    char *buf;

    buf = readline("\n>>> ");
    if (strlen(buf) != 0)
    {
        add_history(buf);
        strcpy(str, buf);
        return 0;
    }
    else
    {
        return 1;
    }
}

// Function to print Current Directory.
void printDir()
{
    char *username = getenv("USER");
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\n%s@: %s", username, cwd);
}

// Function where the system command is executed
void execArgs(char **parsed)
{
    printf("Ye command APNA SHELL me nhi h, UNIX SHELL execute krega ab ye\n");
    // Forking a child
    pid_t pid = fork();

    if (pid == -1)
    {
        printf("\nFailed forking child..");
        return;
    }
    else if (pid == 0)
    {
        if (execvp(parsed[0], parsed) < 0)
        {
            printf("\nCould not execute command..");
        }
        exit(0);
    }
    else
    {
        // waiting for child to terminate
        wait(NULL);
        return;
    }
}

// Function where the piped system commands is executed
void execArgsPiped(char **parsed, char **parsedpipe)
{
    printf("Ye command APNA SHELL me nhi h, UNIX SHELL execute krega ab ye\n");
    // 0 is read end, 1 is write end
    int pipefd[2];
    pid_t p1, p2;

    if (pipe(pipefd) < 0)
    {
        printf("\nPipe could not be initialized");
        return;
    }
    p1 = fork();
    if (p1 < 0)
    {
        printf("\nCould not fork");
        return;
    }

    if (p1 == 0)
    {
        // Child 1 executing..
        // It only needs to write at the write end
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execvp(parsed[0], parsed) < 0)
        {
            printf("\nCould not execute command 1..");
            exit(0);
        }
    }
    else
    {
        // Parent executing
        p2 = fork();

        if (p2 < 0)
        {
            printf("\nCould not fork");
            return;
        }

        // Child 2 executing..
        // It only needs to read at the read end
        if (p2 == 0)
        {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(parsedpipe[0], parsedpipe) < 0)
            {
                printf("\nCould not execute command 2..");
                exit(0);
            }
        }
        else
        {
            // parent executing, waiting for two children
            wait(NULL);
            wait(NULL);
        }
    }
}

// Help command builtin
void openHelp()
{
    puts("\n*WELCOME to Trahimam Service Center*"
         "\nList of Commands supported:"
         "\n>niklo"
         "\n>dir_badlo"
         "\n>madad"
         "\n>namaste"
         "\n>dikhao: samaan upar se neeche tk dikhayenge"
         "\n>dir_rasta"
         "\n>utha_patak: tasreef ek jagah se utha ke dusri jagah rakhenge"
         "\n>chaapo"
         "\n>hata_file"
         "\n>nayi_dir"
         "\n>nayi_file"
         "\n>Baaki saare commands jo UNIX Shell mein h wo bhi chalenge, Chinta mat kijiye"
         "\n>Pipe handling bhi chalti h idhar"
         "\n>Improper space handling bhi :)");

    return;
}

// Function to execute builtin commands
int ownCmdHandler(char **parsed)
{
    int ret = 0;
    char cwd[1024];
    char hostname[1024];
    int NoOfOwnCmds = 15, switchOwnArg = 0;
    char *ListOfOwnCmds[NoOfOwnCmds];
    char *username;
    int fd, i, fd1, fd2;
    char buf[2];
    char *file1, *file2;
    char *fn, *pat, *temp;
    FILE *fp;
    char line[100];

    ListOfOwnCmds[0] = "niklo";        // exit is renamed as niklo
    ListOfOwnCmds[1] = "dir_badlo";    // cd is renamed as directory_badlo
    ListOfOwnCmds[2] = "madad";        // help is renamed as madad
    ListOfOwnCmds[3] = "namaste";      // hello is renamed as namaste
    ListOfOwnCmds[4] = "dikhao";       // cat is renamed as nayi_file
    ListOfOwnCmds[5] = "dir_rasta";    // pwd is renamed as dir_ka_rasta
    ListOfOwnCmds[6] = "utha_patak";   // mv is renamed as utha_patak
    ListOfOwnCmds[7] = "chaapo";       // cp is renamed as chaapo
    ListOfOwnCmds[8] = "hata_file";    // rm is renamed as hata_file
    ListOfOwnCmds[9] = "nayi_dir";     // mkdir is renamed as nayi_dir
    ListOfOwnCmds[10] = "nayi_file";   // touch is renamed as nayi_file
    ListOfOwnCmds[11] = "saaf";        // clear
    ListOfOwnCmds[12] = "host_naam";   // hostname
    ListOfOwnCmds[13] = "hata_dir";    // rmdir
    ListOfOwnCmds[14] = "dhund_ke_aa"; // grep is renamed as dhund_ke_aa
    // ListOfOwnCmds[15] = "malik_koun";  // custom command to show system info

    // Code for implementing customized name commands is done to replicate the action of standard commands.

    for (int i = 0; i < NoOfOwnCmds; i++)
    {
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0)
        {
            switchOwnArg = i + 1;
            break;
        }
    }

    switch (switchOwnArg)
    {
    case 1:
        printf("\nChalo Bye yaar!\n");
        exit(0);
    case 2:
        chdir(parsed[1]);
        return 1;
    case 3:
        openHelp();
        return 1;
    case 4:
        username = getenv("USER");
        printf("\nNamaste Sir/Ma'am %s.\nIss jagah faltu masti nhi "
               "Kaam karo yaar."
               "\nAur janna h to madad likho\n",
               username);
        return 1;
    case 5:
        fd = open(parsed[1], O_RDONLY, 0777);
        if (fd == -1)
            printf("File name toh sahi do!!‍\n");
        else
        {
            while ((i = read(fd, buf, 1)) > 0)
                printf("%c", buf[0]);
            close(fd);
        }
        return 1;
    case 6:
        getcwd(cwd, sizeof(cwd));
        printf("%s\n", cwd);
        return 1;
    case 7:
        file1 = parsed[1];
        file2 = parsed[2];
        printf("Iss File se utha ke = %s \nIsme daal denge = %s", file1, file2);
        fd1 = open(file1, O_RDONLY, 0777);
        fd2 = creat(file2, 0777);
        while (i = read(fd1, buf, 1) > 0)
            write(fd2, buf, 1);
        remove(file1);
        close(fd1);
        close(fd2);
        return 1;
    case 8:
        file1 = parsed[1];
        file2 = parsed[2];
        printf("Iss waali file ko = %s \nIss file mein copy kar denge =%s", file1, file2);
        fd1 = open(file1, O_RDONLY, 0777);
        fd2 = creat(file2, 0777);
        while (i = read(fd1, buf, 1) > 0)
            write(fd2, buf, 1);
        close(fd1);
        close(fd2);
        return 1;
    case 9:
        file1 = parsed[1];
        remove(file1);
        return 1;
    case 10:
        if (mkdir(parsed[1], 0777) == -1)
            printf("%s\n", strerror(errno));
        else
            printf("Ho gya aapka Kaam!!\nDirectory ban gyi!!\n");
        return 1;
    case 11:
        file1 = parsed[1];
        fd1 = creat(file1, 0777);
        printf("File ban gyi!!\n");
        return 1;
    case 12:
        clear();
        return 1;
    case 13:
        gethostname(hostname, 1024);
        printf("%s\n", hostname);
        return 1;
    case 14:
        ret = rmdir(parsed[1]);
        if (ret == 0)
            printf("Given empty directory removed successfully\n");
        else
            printf("Unable to remove directory %s\n", parsed[1]);
        return 1;
    case 15:
        fp = fopen(parsed[2], "r");
        while (fscanf(fp, "%[^\n]\n", line) != EOF)
        {
            if (strstr(line, parsed[1]) != NULL)
            {
                printf("%s\n", line);
            }
            else
            {
                continue;
            }
        }
        fclose(fp);
        return 1;
    default:
        break;
    }

    return 0;
}

// function for finding pipe
int parsePipe(char *str, char **strpiped)
{
    for (int i = 0; i < 2; i++)
    {
        strpiped[i] = strsep(&str, "|");
        if (strpiped[i] == NULL)
            break;
        if (strlen(strpiped[i]) == 0)
            i--;
    }

    if (strpiped[1] == NULL)
        return 0; // returns zero if no pipe is found.
    else
    {
        return 1;
    }
}

// function for parsing command words
void parseSpace(char *str, char **parsed)
{
    for (int i = 0; i < MAXLIST; i++)
    {
        parsed[i] = strsep(&str, " ");

        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }
}

int processString(char *str, char **parsed, char **parsedpipe)
{

    char *strpiped[2];
    int piped = 0;

    piped = parsePipe(str, strpiped);

    if (piped)
    {
        parseSpace(strpiped[0], parsed);
        parseSpace(strpiped[1], parsedpipe);
    }
    else
    {
        parseSpace(str, parsed);
    }

    int tmp = 0;
    // if (ownCmdHandler(parsed))
    //     return 0;
    // else
    //     return 1 + piped;
    if (ownCmdHandler(parsed) == 0)
        tmp++;

    if (piped)
        if (ownCmdHandler(parsedpipe) == 0)
            tmp += 2;

    return tmp;
}

int main()
{
    char inputString[MAXCOM], *parsedArgs[MAXLIST];
    char *parsedArgsPiped[MAXLIST];
    int execFlag = 0;
    init_shell();

    while (1)
    {
        // print shell line
        printDir();
        // take input
        if (takeInput(inputString))
            continue;
        // process
        execFlag = processString(inputString, parsedArgs, parsedArgsPiped);

        // execute
        if (execFlag == 1)
            execArgs(parsedArgs);

        if (execFlag == 2)
            execArgs(parsedArgsPiped);

        if (execFlag == 3)
            execArgsPiped(parsedArgs, parsedArgsPiped);
    }
    return 0;
}