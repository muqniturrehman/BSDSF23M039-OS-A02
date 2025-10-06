/*
* Programming Assignment 02: lsv1.0.0
* This is the source file of version 1.0.0
* Read the write-up of the assignment to add the features to this base version
* Usage:
*       $ lsv1.0.0 
*       % lsv1.0.0  /home
*       $ lsv1.0.0  /home/kali/   /etc/
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <getopt.h>
#include <pwd.h>     
#include <grp.h>     
#include <time.h>

extern int errno;

void do_ls(const char *dir);
void do_ls_long(const char *dir);   


int main(int argc, char *argv[])
{
    int opt;                 // holds option character
    int long_format = 0;     // flag variable for -l
    opterr = 0;              // disable automatic getopt() errors

    // If no arguments → list current directory
    if (argc == 1)
    {
        do_ls(".");
        return 0;
    }

    // Parse options using getopt()
    while ((opt = getopt(argc, argv, "l")) != -1)
    {
        switch (opt)
        {
            case 'l':
                long_format = 1; // user typed -l
                break;

            case '?':   // invalid or undeclared option
            default:
                fprintf(stderr, "Error: Invalid option '-%c'\n", optopt);
                fprintf(stderr, "Usage: %s [-l] [directory...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // If no directories specified after options
    if (optind == argc)
    {
        if (long_format)
            do_ls_long(".");
        else
            do_ls(".");
    }
    else
    {
        // Loop through each directory given
        for (int i = optind; i < argc; i++)
        {
            printf("Directory listing of %s:\n", argv[i]);

            if (long_format)
                do_ls_long(argv[i]);
            else
                do_ls(argv[i]);

            puts("");
        }
    }

    return 0;
}


void do_ls(const char *dir)
{
    struct dirent *entry;
    DIR *dp = opendir(dir);

    if (dp == NULL)
    {
        fprintf(stderr, "Cannot open directory : %s\n", dir);
        return;
    }

    errno = 0;

    while ((entry = readdir(dp)) != NULL)
    {
        if (entry->d_name[0] == '.')
            continue;
        printf("%s\n", entry->d_name);
    }

    if (errno != 0)
    {
        perror("readdir failed");
    }

    closedir(dp);
}

void do_ls_long(const char *dir)
{
    struct dirent *entry;
    DIR *dp = opendir(dir);

    if (dp == NULL)
    {
        fprintf(stderr, "Cannot open directory : %s\n", dir);
        return;
    }

    errno = 0;

    while ((entry = readdir(dp)) != NULL)
    {
        if (entry->d_name[0] == '.')
            continue;

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);

        struct stat info;
        if (stat(path, &info) == -1)
        {
            perror("stat");
            continue;
        }

        /* -------- File type and permissions -------- */
        char perms[11];
        perms[0] = S_ISDIR(info.st_mode) ? 'd' :
                   S_ISLNK(info.st_mode) ? 'l' :
                   S_ISCHR(info.st_mode) ? 'c' :
                   S_ISBLK(info.st_mode) ? 'b' :
                   S_ISFIFO(info.st_mode) ? 'p' :
                   S_ISSOCK(info.st_mode) ? 's' : '-';

        perms[1] = (info.st_mode & S_IRUSR) ? 'r' : '-';
        perms[2] = (info.st_mode & S_IWUSR) ? 'w' : '-';
        perms[3] = (info.st_mode & S_IXUSR) ? 'x' : '-';
        perms[4] = (info.st_mode & S_IRGRP) ? 'r' : '-';
        perms[5] = (info.st_mode & S_IWGRP) ? 'w' : '-';
        perms[6] = (info.st_mode & S_IXGRP) ? 'x' : '-';
        perms[7] = (info.st_mode & S_IROTH) ? 'r' : '-';
        perms[8] = (info.st_mode & S_IWOTH) ? 'w' : '-';
        perms[9] = (info.st_mode & S_IXOTH) ? 'x' : '-';
        perms[10] = '\0';

        /* -------- Owner, group, and modification time -------- */
        struct passwd *pw = getpwuid(info.st_uid);
        struct group  *gr = getgrgid(info.st_gid);
        const char *user = pw ? pw->pw_name : "unknown";
        const char *group = gr ? gr->gr_name : "unknown";

        char time_str[20];
        struct tm *tm = localtime(&info.st_mtime);
        strftime(time_str, sizeof(time_str), "%b %d %H:%M", tm);

        /* -------- Print formatted output -------- */
        printf("%s %2lu %-8s %-8s %8ld %s %s\n",
               perms,
               info.st_nlink,
               user,
               group,
               info.st_size,
               time_str,
               entry->d_name);
    }

    if (errno != 0)
        perror("readdir failed");

    closedir(dp);
}