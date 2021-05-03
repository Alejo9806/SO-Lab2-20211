#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_SIZE 100
#define BUFFER_SIZE 1024

char messageError[28] = "Ha ocurrido un error\n";
char *system_path_commands[30] = {
    "./",
    "/usr/bin/",
    "/bin/",
    NULL};
typedef enum{cd,endup,path,not_command,} builtin_command;

const static struct
{
    builtin_command command;
    char *string_command;
} commands[] = {
    {cd, "cd"},
    {endup, "exit"},
    {path, "path"},
};


void stringParse(char str[], char *array[30], int flagsNumber);
void cdWish(char **args);
void error();
void run(char *cmd[]);
void wish();
void pathWish(char *system_path_commands[], char *cmd[]);
void batch(char *argv[]);
void lineParse(char line[], char *array[30], int flagsNumber);


builtin_command str_to_command(char *strcommand)
{
    for (int i = 0; i < 3; i++)
    {
        if (!strcmp(strcommand, commands[i].string_command))
        {
            return commands[i].command;
        }
    }
    return not_command;
}

//main del programa
int main(int argc, char *argv[])
{

    if (argc == 1)
    {

        wish();
    }
    else if (argc == 2)
    {

        batch(argv);
    }
    else
    {
        error();
    }
}


void run(char *cmd[])
{

    char *auxcmd[30];
    int index = 0;
    int auxIndex = 0;
    while (cmd[index] != NULL)
    {
        if (!strcmp(cmd[index], "&") || cmd[index + 1] == NULL)
        {
            int rc = fork();
            if (rc < 0)
            {
                fprintf(stderr, "Hay un error en el fork\n");
                exit(1);
            }
            else if (rc == 0)
            {
                if (cmd[index + 1] == NULL)
                {
                    auxcmd[auxIndex] = cmd[index];
                    auxIndex++;
                }
                auxcmd[auxIndex] = NULL;
                execvp(auxcmd[0], auxcmd);
            }
            else
            {
                wait(NULL);
                auxIndex = 0;
            }
        }
        else
        {
            auxcmd[auxIndex] = cmd[index];
            auxIndex++;
        }
        index++;
    }
}

void wish()
{
    char str[MAX_SIZE];
    do
    {
        printf("wish$");
        fgets(str, MAX_SIZE, stdin);
        char *p = str;
        while (*p != '\n')
        {
            p++;
        }
        *p = '\0';

        char *cmd[sizeof(str)];

        stringParse(str, cmd, 20);

        builtin_command command = str_to_command(str);

        if (command != not_command)
        {

            switch (command)
            {
            case cd:
                cdWish(cmd);
                break;
            case path:
                pathWish(system_path_commands, cmd);
                  int directions = 0;
                while (system_path_commands[directions] != NULL)
                {
                    printf("path: %s \n", system_path_commands[directions]);
                    directions++;
                }
                break;
            case endup:
                exit(0);
                break;
            default:
                error();
            }
        }
        else
        {

            int i = 0;
            char pathToFile[MAX_SIZE];
            int returnValue = -1;

            while (system_path_commands[i] != NULL && returnValue == -1)
            {

                strcpy(pathToFile, system_path_commands[i]);

                if (cmd[0] != NULL)
                {
                    strcat(pathToFile, cmd[0]);
                }

                returnValue = access(pathToFile, X_OK);

                i++;
            }
            if (returnValue != -1)
            {
                run(cmd);
            }
            else
            {
                error();
            }
        }

    } while (1);
}

void batch(char *argv[])
{
    char line[BUFFER_SIZE];

    FILE *file = NULL;
    char *batchFile = strdup(argv[1]);
    file = fopen(batchFile, "r");
    if (!file)
    {
        error();
        exit(1);
    }

    while (fgets(line, BUFFER_SIZE, file))
    {
        char *cmd[sizeof(line)];

        lineParse(line, cmd, 20);
        printf("Comando: %s\n", cmd[0]);
        builtin_command command = str_to_command(line);
        if (command != not_command)
        {
            switch (command)
            {
            case cd:
                printf("cd executed\n");
                cdWish(cmd);
                break;
            case path:
                pathWish(system_path_commands, cmd);
                  int directions = 0;
                while (system_path_commands[directions] != NULL)
                {
                    printf("path: %s \n", system_path_commands[directions]);
                    directions++;
                }
                break;
            case endup:
                exit(0);
                break;
            default:
                error();
            }
        }
        else
        {
            int i = 0;
            char pathToFile[MAX_SIZE];
            int returnValue = -1;

            while (system_path_commands[i] != NULL && returnValue == -1)
            {
                strcpy(pathToFile, system_path_commands[i]);
                strcat(pathToFile, cmd[0]);
                returnValue = access(pathToFile, X_OK);
                i++;
            }
            if (returnValue != -1)
            {
                run(cmd);
            }
            else
            {
                error();
            }
        }
    }
}

void cdWish(char **args)
{

    if (args[1] != NULL && args[2] == NULL)
    {
        {
            if (chdir(args[1]) == -1)
                printf("Hay un error en la ruta solicitada\n");

            char cwd[1024];

            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                fprintf(stdout, "Te encuentras en: %s\n", cwd);
            }
            else
            {
                perror("getcwd() error");
            }
        }
    }
    else
    {

        printf("Hay un error en la ruta solicitada\n");
    }
};


void pathWish(char *system_path_commands[], char *cmd[])
{
   int index = 1;
  while (cmd[index] != NULL)
  {    
    int y = 0;
    int equal = 0;
    while (system_path_commands[y] != NULL)
    {    
      equal = strcmp(cmd[index], system_path_commands[y]);

      if (equal == 0)
      {
        break;
      }
      if (system_path_commands[y + 1] == NULL && equal != 0)
      {
        system_path_commands[y + 1] = cmd[index];
        system_path_commands[y + 2] = NULL;
        equal = 0;
        break;
      }
      y++;
    }
    index++;
  }

}


void lineParse(char line[], char *array[30], int flagNumber)
{

    if (line[0] != '\0' && line[0] != '\n')
    {
        char *command[30];
        command[0] = strtok(line, " \t\n");
        int p = 0;
        while (command[p] != NULL)
        {
            p++;
            command[p] = strtok(NULL, " \n\t");
        }
        command[p + 1] = NULL;

        for (int loop = 0; loop <  flagNumber; loop++)
    {
        array[loop] = command[loop];
    }
    }
    
}




void stringParse(char str[], char *array[30], int flagsNumber)
{

    char *command[30];
    char *token = strtok(str, " ");
    int j = 0;

    while (token != NULL)
    {
        command[j] = token;
        token = strtok(NULL, " ");
        j++;
    }
    command[j] = NULL;
    for (int loop = 0; loop < flagsNumber; loop++)
    {
        array[loop] = command[loop];
    }
}


void error()
{
    write(STDERR_FILENO, messageError, strlen(messageError));//calculamos el numero de caracteres que apunta el mensaje de error y escribimos el mesanje de error
}