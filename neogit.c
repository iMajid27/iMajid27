#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <windows.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
// Majid Mohamadi 27

bool exist_neogitDIR()

{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    struct dirent *entry;
    while (1)
    {
        DIR *dir = opendir(".");
        if (dir == NULL)
        {
            fprintf(stdout, "Error opening current directory");
            return 1;
        }

        bool dirup = false;
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".neogit") == 0)
            {
                return true;
            }

            if (strcmp(entry->d_name, "..") == 0)
            {
                dirup = true;
            }
        }

        closedir(dir);
        if (!dirup)
        {
            return false;
        }
        chdir("..");
    }
}

bool folder_or_file(char *file_address) // file == false     ,     folder == true
{
    struct stat M;
    if (stat(file_address, &M) == 0)
    {
        if (M.st_mode & S_IFDIR)
        {
            return true;
        }
        else if (M.st_mode & S_IFREG)
        {
            return false;
        }
    }
}

bool isfolderempty(const char *folderPath)
{
    DIR *dir = opendir(folderPath);
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            return false;
            break;
        }
    }

    closedir(dir);
    return true;
}

int run_init(int argc, char *const argv[])
{
    char cwd[1024];
    if (GetCurrentDirectory(sizeof(cwd), cwd) == 0)
    {
        fprintf(stdout, "Error getting current directory");
        return 1;
    }

    if (exist_neogitDIR())
    {
        fprintf(stdout, ".neogit dir already exist");
    }
    else
    {
        chdir(cwd);
        mkdir(".neogit");
        SetFileAttributes(".neogit", FILE_ATTRIBUTE_HIDDEN);
        mkdir(".neogit\\stage");
        fprintf(stdout, "neogit repertory is added in : %s", cwd);
    }

    return 0;
}

int run_add(int argc, char *const argv[])
{
    char cwd1[1024];
    getcwd(cwd1, sizeof(cwd1));
    if (argc < 3)
    {
        fprintf(stdout, "please enter filename or filepath you want to add ");
        return 1;
    }

    else if (strcmp(argv[2], "-n") == 0)
    {
        chdir(cwd1);
        struct dirent *entry;
        DIR *dir = opendir(".");
        while ((entry = readdir(dir)) != NULL)
        {
            bool stageOrnot = false;
            struct dirent *stagedFiles;
            DIR *dir1 = opendir(".neogit\\stage");

            while ((stagedFiles = readdir(dir1)) != NULL)
            {
                if (!strcmp(entry->d_name, stagedFiles->d_name))
                {
                    stageOrnot = true;
                }
            }
            if (stageOrnot == true)
            {
                fprintf(stdout, "%s is -> stage\n", entry->d_name);
            }

            else
            {
                fprintf(stdout, "%s is -> unstage\n", entry->d_name);
            }
        }
    }
    else
    {
        chdir(".neogit");
        chdir("stage");
        int i = 2;
        if (strcmp(argv[2], "-f") == 0)
        {
            i = 3;
        }

        for (i; i < argc; i++)
        {
            chdir(cwd1);
            char file_address[272];

            sprintf(file_address, "%s\\%s", cwd1, argv[i]);
            if (folder_or_file(file_address))
            {
                struct dirent *entry;
                DIR *dir = opendir(argv[i]);
                if (dir == NULL)
                {
                    fprintf(stdout, "folder does not exist");
                }

                else
                {
                    while ((entry = readdir(dir)) != NULL)
                    {
                        if (entry->d_type == DT_REG)
                        {
                            char copy[72];
                            exist_neogitDIR();
                            sprintf(copy, "copy %s .neogit\\stage > nul", file_address);
                            system(copy);
                            fprintf(stdout, "%s added successfully\n", entry->d_name);
                        }
                    }
                    closedir(dir);
                }
            }
            else
            {
                FILE *file;
                file = fopen(argv[i], "r");
                if (file == NULL)
                {
                    printf("file does not exist\n");
                }
                else
                {
                    char copy[72];
                    sprintf(copy, "copy %s .neogit\\stage > nul", file_address);
                    system(copy);
                    fprintf(stdout, "%s added successfully\n", argv[i]);
                }
            }
        }
    }
}

int run_config(int argc, char *const argv[])
{
    if (argc < 4)
    {
        fprintf(stdout, "please enter the right command !");
    }
    if (strcmp(argv[2], "-global") == 0)
    {
        chdir(".neogit");
        mkdir("config");
        chdir("config");

        if (strcmp(argv[3], "user.name") == 0)
        {
            FILE *file = fopen("name-global.txt", "w");
            if (file == NULL)
                return 1;
            fprintf(file, "username : %s\n", argv[4]);
            fprintf(stdout, "Done !");
        }
        else if (strcmp(argv[3], "user.email") == 0)
        {
            FILE *file = fopen("email-global.txt", "w");
            if (file == NULL)
                return 1;
            fprintf(file, "user.email : %s\n", argv[4]);
            fprintf(stdout, "Done !");
        }
    }
    if (strncmp(argv[2], "alias.", 6))
    {
    }
}
int run_reset(int argc, char *const argv[])
{
    if (argc < 3)
    {
        fprintf(stdout, "please enter valid command !");
    }
    if (strcmp(argv[2], "-undo") == 0)
    {
    }
    else
    {
        char cwd2[1024];
        getcwd(cwd2, sizeof(cwd2));

        int i = 2;
        if (strcmp(argv[2], "-f") == 0)
        {
            i = 3;
        }
        for (i; i < argc; i++)
        {
            chdir(cwd2);
            char file_address[100];

            sprintf(file_address, "%s\\%s", cwd2, argv[i]);
            if (folder_or_file(file_address))
            {
                struct dirent *entry;
                DIR *dir = opendir(argv[i]);
                if (dir == NULL)
                {
                    fprintf(stdout, "folder does not exist");
                }
                else
                {
                    exist_neogitDIR();
                    while ((entry = readdir(dir)) != NULL)
                    {
                        if (entry->d_type == DT_REG)
                        {
                            bool stageOrnot = false;
                            struct dirent *stage;
                            chdir(".neogit\\stage");
                            DIR *dir2 = opendir(".");
                            while ((stage = readdir(dir2)) != NULL)
                            {
                                if (strcmp(entry->d_name, stage->d_name) == 0)
                                {
                                    stageOrnot = true;
                                    char move[72];
                                    sprintf(move, "move %s unstage > nul", stage->d_name);
                                    system(move);
                                    fprintf(stdout, "%s unstaged successfully\n", entry->d_name);
                                }
                            }
                            if (stageOrnot == false)
                            {
                                fprintf(stdout, "%s is already unstage\n", entry->d_name);
                            }
                        }
                    }
                    closedir(dir);
                }
            }
            else
            {
                /* FILE *file;
                file = fopen(argv[i], "r");
                if (file == NULL)
                {
                    fprintf(stdout, "file does not exist\n");
                } */
                // fclose(file);
                struct dirent *stage;
                chdir(".neogit\\stage");
                DIR *dir1 = opendir(".");
                char *filename = strrchr(argv[i], "\\");
                fprintf(stdout, "%s", filename);
                bool stageOrnot;
                while ((stage = readdir(dir1)) != NULL)
                {
                    if (strcmp(filename, stage->d_name) == 0)
                    {
                        char move[50];
                        stageOrnot = true;
                        sprintf(move, "move %s unstage > nul", filename);
                        system(move);
                    }
                }
                if (stageOrnot == true)
                {
                    fprintf(stdout, "%s unstaged successfully\n", filename);
                }

                else
                {
                    fprintf(stdout, "%s is already unstage\n", filename);
                }
            }
        }
    }
}
int run_status(int argc, char *const argv[])
{
    if (argc > 2)
    {
        fprintf(stdout, "ha !");
    }
    else
    {
    }
}

int run_commit(int argc, char *const argv[])
{

    if (strcmp(argv[2], "-m") == 0)
    {
        if (argc < 4)
        {
            fprintf(stdout, "enter the massege please !");
        }
        else if (strlen(argv[3]) > 72)
        {
            fprintf(stdout, "your massage is too long !");
        }
        else
        {
           if(isfolderempty(".neogit\\stage")) 
           {
            fprintf(stdout , "stage dir is empty");
           }
           else 
           {
           } 
        }
    }
    else
    {
        fprintf(stdout, "wrong command !");
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *const argv[])
{
    if (argc < 2)
    {
        fprintf(stdout, "please enter a valid command\n");
        return 1;
    }

    if (strcmp(argv[1], "init") == 0)
    {
        run_init(argc, argv);
    }

    else if (strcmp(argv[1], "add") == 0)
    {
        if (exist_neogitDIR)
        {
            run_add(argc, argv);
        }
        else
        {
            fprintf(stdout, "you dont have any neogit repertory !");
        }
    }

    else if (strcmp(argv[1], "reset") == 0)
    {
        if (exist_neogitDIR())
        {
            run_reset(argc, argv);
        }

        else
        {
            fprintf(stdout, "you dont have any neogit repertory !");
        }
    }

    else if (strcmp(argv[1], "config") == 0)
    {
        if (exist_neogitDIR())
        {
            run_config(argc, argv);
        }

        else
        {
            fprintf(stdout, "you dont have any neogit repertory !");
        }
    }

    else if (strcmp(argv[1], "commit") == 0)
    {
        if (exist_neogitDIR())
        {
            run_commit(argc, argv);
        }

        else
        {
            fprintf(stdout, "you dont have any neogit repertory !");
        }
    }

    else if (strcmp(argv[1], "status") == 0)
    {
        if (exist_neogitDIR())
        {
            run_status(argc, argv);
        }

        else
        {
            fprintf(stdout, "you dont have any neogit repertory !");
        }
    }

    else if (strcmp(argv[1], "checkout") == 0)
    {
    }

    else
    {
        fprintf(stdout, "WTF");
    }
    return 0;
}