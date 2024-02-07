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

int countfiles(const char *path)
{
    struct dirent *entry;
    DIR *dir = opendir(path);
    int count = 0;

    if (dir == NULL)
    {
        fprintf(stdout, "Error opening directory");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            count++;
        }
    }

    closedir(dir);
    return count;
}

bool word_check_in_file(const char *filePath, const char *word)
{
    FILE *file = fopen(filePath, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    char buffer[1000];
    int found = false;

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        if (strstr(buffer, word) != NULL)
        {
            found = true;
            break;
        }
    }

    fclose(file);

    return found;
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
        mkdir(".neogit\\stage\\unstage");
        mkdir(".neogit\\commit");

        FILE *file;
        file = fopen(".neogit\\commit\\currentbranch.txt", "w");
        fprintf(file, "master");
        fclose(file);

        FILE *file1;
        file1 = fopen(".neogit\\commit\\branches.txt", "w");
        fprintf(file1, "master");
        fclose(file1);
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
    if (strcmp(argv[2], "-global") != 0)
    {
        chdir(".neogit");
        mkdir("config");
        chdir("config");

        if (strcmp(argv[2], "user.name") == 0)
        {
            FILE *file = fopen("name-local.txt", "w");
            if (file == NULL)
                return 1;
            fprintf(file, "username : %s\n", argv[3]);
            fprintf(stdout, "Done !");
        }
        else if (strcmp(argv[2], "user.email") == 0)
        {
            FILE *file = fopen("email-local.txt", "w");
            if (file == NULL)
                return 1;
            fprintf(file, "user.email : %s\n", argv[3]);
            fprintf(stdout, "Done !");
        }
    }
    if (strncmp(argv[2], "alias.", 6) == 0) // alias errors
    {
        FILE *file;
        file = fopen("D:\\FOP\\alias.txt", "r");

        bool aexist = false;
        char line[272];
        fgets(line, 272, file);
        char *tokenPtr = strtok(line, "$");
        while (tokenPtr != NULL)
        {
            if (strncmp(tokenPtr, argv[3], strlen(tokenPtr)) == 0)
            {
                aexist = 1;
                break;
            }
            tokenPtr = strtok(NULL, "$");
        }
        if (aexist)
        {
            fprintf(stdout, "right command");
        }
        else
        {
            fprintf(stdout, "wrong !");
        }
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
                chdir(cwd2);
                FILE *file;
                file = fopen(argv[i], "r");
                if (file == NULL)
                {
                    fprintf(stdout, "file does not exist\n");
                }
                else
                {
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
                            char move[72];
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
            if (isfolderempty(".neogit\\stage"))
            {
                fprintf(stdout, "stage dir is empty");
            }
            else
            {
                int count = countfiles(".neogit\\stage");

                char name[27];
                char email[72];
                if (isfolderempty(".neogit\\config"))
                {
                    fprintf(stdout, "you should config your local name and email first");
                }

                FILE *username;
                username = fopen(".neogit\\config\\name-local.txt", "r");
                fgets(name, sizeof(name), username);
                fclose(username);

                FILE *useremail;
                useremail = fopen(".neogit\\config\\email-local.txt", "r");
                fgets(email, sizeof(email), useremail);
                fclose(useremail);

                fprintf(stdout, "%s%s", name, email);

                chdir(".neogit\\commit");
                FILE *file;
                file = fopen("currentbranch.txt", "r");
                char curbranch[20];
                fgets(curbranch, sizeof(curbranch), file);
                fprintf(stdout, "%s\n", curbranch);
                int shomarande = 0;
                char commited[10];
                sprintf(commited, "%d", shomarande + 1);
                CreateDirectory(commited, NULL);

                time_t t = time(NULL);
                struct tm zaman = *localtime(&t);

                chdir(commited);

                FILE *commitinfo;
                commitinfo = fopen("commit_info.txt", "w");
                fprintf(commitinfo, "id: %s\nbranch: %s\nmessage: %s\nuser.name: %suser.email: %snumber of files: %d\ntime: %d-%02d-%02d %02d:%02d:%02d", commited, curbranch, argv[3], name, email, count, zaman.tm_year + 1900, zaman.tm_mon + 1, zaman.tm_mday, zaman.tm_hour, zaman.tm_min, zaman.tm_sec);

                fclose(file);
                char copy[50];
                sprintf(copy, "copy %d\\* %d > nul", shomarande, shomarande + 1);
                system(copy);

                exist_neogitDIR();

                sprintf(copy, "move .neogit\\stage\\* .neogit\\commit\\%d > nul", shomarande + 1);
                system(copy);

                FILE *file2;
                file2 = fopen(".neogit\\commit\\curcommit.txt", "w");
                fprintf(file2, "100");
                fclose(file2);

                fprintf(stdout, "Commit id:     %s\n", commited);
                fprintf(stdout, "Time:          %d-%02d-%02d %02d:%02d:%02d\n", zaman.tm_year + 1900, zaman.tm_mon + 1, zaman.tm_mday, zaman.tm_hour, zaman.tm_min, zaman.tm_sec);

                fprintf(stdout, "commit messag: %s\n", argv[3]);
            }
        }
    }
    else
    {
        fprintf(stdout, "wrong command !");
    }
}

int run_branch(int argc, char *const argv[])
{
    if (argc > 3)
    {
        fprintf(stdout, "wrong command !");
    }
    else
    {
        if (argc == 3)
        {
            bool exist = word_check_in_file(".neogit\\commit\\branches.txt", argv[2]);

            if (exist)
                printf("This branch already exists\n");

            else
            {
                FILE *file;
                file = fopen(".neogit\\commit\\branches.txt", "a");
                fprintf(file, "\n");
                fprintf(file, argv[2]);
                fclose(file);
                fprintf(stdout, "%s Branch created successfully\n", argv[2]);
            }
        }
        else if (argc == 2)
        {
            FILE *file;
            file = fopen(".neogit\\commit\\branches.txt", "r");
            while (file != NULL)
            {
                fprintf(stdout , "%s" , file);
            }
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *const argv[])
{
    if (argc < 2)
    {
        fprintf(stdout, "please enter what you need me to do !\n");
        return 1;
    }

    if (strcmp(argv[1], "init") == 0)
    {
        run_init(argc, argv);
    }

    else if (strcmp(argv[1], "add") == 0)
    {
        if (exist_neogitDIR())
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
        if (exist_neogitDIR())
        {
        }

        else
        {
            fprintf(stdout, "you dont have any neogit repertory !");
        }
    }

    else if (strcmp(argv[1], "log") == 0)
    {
        if (exist_neogitDIR())
        {
        }

        else
        {
            fprintf(stdout, "you dont have any neogit repertory !");
        }
    }

    else if (strcmp(argv[1], "branch") == 0)
        if (exist_neogitDIR())
        {
            run_branch(argc, argv);
        }
        else
        {
            fprintf(stdout, "you dont have any neogit repertory !");
        }

    ////   phase 2 ////
    else if (strcmp(argv[1], "revert") == 0)
    {
        if (exist_neogitDIR())
        {
        }

        else
        {
            fprintf(stdout, "you dont have any neogit repertory !");
        }
    }

    else if (strcmp(argv[1], "tag") == 0)
    {
        if (exist_neogitDIR())
        {
        }

        else
        {
            fprintf(stdout, "you dont have any neogit repertory !");
        }
    }

    else if (strcmp(argv[1], "tree") == 0)
    {
        if (exist_neogitDIR())
        {
        }

        else
        {
            fprintf(stdout, "you dont have any neogit repertory !");
        }
    }

    else if (strcmp(argv[1], "stash") == 0)
    {
        if (exist_neogitDIR())
        {
        }

        else
        {
            fprintf(stdout, "you dont have any neogit repertory !");
        }
    }

    else if (strcmp(argv[1], "pre-commit") == 0)
    {
        if (exist_neogitDIR())
        {
        }

        else
        {
            fprintf(stdout, "you dont have any neogit repertory !");
        }
    }

    else if (strcmp(argv[1], "grep") == 0)
    {
        if (exist_neogitDIR())
        {
        }

        else
        {
            fprintf(stdout, "you dont have any neogit repertory !");
        }
    }

    else if (strcmp(argv[1], "diff") == 0)
    {
        if (exist_neogitDIR())
        {
        }

        else
        {
            fprintf(stdout, "you dont have any neogit repertory !");
        }
    }

    else if (strcmp(argv[1], "merge") == 0)
    {
        if (exist_neogitDIR())
        {
        }

        else
        {
            fprintf(stdout, "you dont have any neogit repertory !");
        }
    }
    else
    {
        fprintf(stdout, "command does not exist !");
    }
    return 0;
}