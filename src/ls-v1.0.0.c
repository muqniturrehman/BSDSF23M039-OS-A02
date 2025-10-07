/*
* Programming Assignment 02: lsv1.2.0
* Features:
*   - Default: Adaptive column display
*   - -l flag: Long listing format
*   - Dynamically reads all filenames
*   - Calculates terminal width
*   - Down-then-across column printing
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
// ======== COLOR CODES (ANSI ESCAPE SEQUENCES) ========
#define COLOR_RESET   "\033[0m"
#define COLOR_BLUE    "\033[0;34m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_PINK    "\033[0;35m"
#define COLOR_REVERSE "\033[7m"


extern int errno;

/* Function declarations */
void do_ls(const char *dir);
void do_ls_long(const char *dir);
void do_ls_horizontal(const char *dir);
void print_colored(const char *name, const char *dir, mode_t mode);   // ✅ add this line


int compare_filenames(const void *a, const void *b)
{
    const char *fa = *(const char **)a;
    const char *fb = *(const char **)b;
    return strcasecmp(fa, fb);   
}


int main(int argc, char *argv[])
{
    int opt;
    int display_mode = 0; // 0 = default, 1 = long (-l), 2 = horizontal (-x)
    opterr = 0;           // disable getopt default error messages

    // Parse options
    while ((opt = getopt(argc, argv, "lx")) != -1)
    {
        switch (opt)
        {
            case 'l': display_mode = 1; break;
            case 'x': display_mode = 2; break;
            case '?':
            default:
                fprintf(stderr, "Usage: %s [-l | -x] [directory...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // If no directories provided → default to current directory
    if (optind == argc)
    {
        if (display_mode == 1)
            do_ls_long(".");
        else if (display_mode == 2)
            do_ls_horizontal(".");
        else
            do_ls(".");
        return 0;
    }

    // Process multiple directories
    for (int i = optind; i < argc; i++)
    {
        printf("Directory listing of %s:\n", argv[i]);

        if (display_mode == 1)
            do_ls_long(argv[i]);
        else if (display_mode == 2)
            do_ls_horizontal(argv[i]);
        else
            do_ls(argv[i]);

        puts("");
    }

    return 0;
}

/* ---------------------------------------------------------------
 * do_ls(): Adaptive Column Display (Default mode)
 * --------------------------------------------------------------- */
void do_ls(const char *dir)
{
    struct dirent *entry;
    DIR *dp = opendir(dir);
    if (!dp)
    {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        return;
    }

    char **filenames = NULL;
    int count = 0, max_len = 0;

    // Step 1: Read all filenames and find the longest name
    while ((entry = readdir(dp)) != NULL)
    {
        if (entry->d_name[0] == '.')
            continue;

        filenames = realloc(filenames, (count + 1) * sizeof(char *));
        filenames[count] = strdup(entry->d_name);

        int len = strlen(entry->d_name);
        if (len > max_len)
            max_len = len;

        count++;
    }
    closedir(dp);
	if (count > 1)
    qsort(filenames, count, sizeof(char *), compare_filenames);

    if (count == 0)
        return;

    // Step 2: Determine terminal width
    struct winsize w;
    int term_width = 80; // fallback
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0)
        term_width = w.ws_col;

    // Step 3: Calculate layout
    int spacing = 2;
    int col_width = max_len + spacing;
    int num_cols = term_width / col_width;
    if (num_cols < 1) num_cols = 1;

    int num_rows = (count + num_cols - 1) / num_cols; // ceiling division

    // Step 4: Print down-then-across
    for (int row = 0; row < num_rows; row++)
    {
        for (int col = 0; col < num_cols; col++)
        {
            int index = col * num_rows + row;
            if (index < count)
			{
				struct stat info;
				char path[1024];
				snprintf(path, sizeof(path), "%s/%s", dir, filenames[index]);

				if (stat(path, &info) == 0)
				{
					print_colored(filenames[index], dir, info.st_mode);
					int pad = col_width - (int)strlen(filenames[index]);
					for (int p = 0; p < pad; p++) printf(" ");
				}
				else
				{
				printf("%-*s", col_width, filenames[index]);
			}
		}


        }
        printf("\n");
    }

    // Step 5: Cleanup
    for (int i = 0; i < count; i++)
        free(filenames[i]);
    free(filenames);
}

/* ---------------------------------------------------------------
 * do_ls_long(): Long Listing Format (for -l)
 * --------------------------------------------------------------- */
void do_ls_long(const char *dir)
{
    struct dirent *entry;
    DIR *dp = opendir(dir);
    if (!dp)
    {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
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

        struct passwd *pw = getpwuid(info.st_uid);
        struct group  *gr = getgrgid(info.st_gid);
        const char *user = pw ? pw->pw_name : "unknown";
        const char *group = gr ? gr->gr_name : "unknown";

        char time_str[20];
        struct tm *tm = localtime(&info.st_mtime);
        strftime(time_str, sizeof(time_str), "%b %d %H:%M", tm);

        printf("%s %2lu %-8s %-8s %8ld %s ", perms, info.st_nlink, user, group, info.st_size, time_str);
		print_colored(entry->d_name, dir, info.st_mode);
		printf("\n");

    }

    if (errno != 0)
        perror("readdir failed");

    closedir(dp);
}
void do_ls_horizontal(const char *dir)
{
    struct dirent *entry;
    DIR *dp = opendir(dir);

    if (!dp)
    {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        return;
    }

    char **filenames = NULL;
    int count = 0, max_len = 0;

    // Step 1: Read all filenames and find the longest name
    while ((entry = readdir(dp)) != NULL)
    {
        if (entry->d_name[0] == '.')
            continue;

        filenames = realloc(filenames, (count + 1) * sizeof(char *));
        filenames[count] = strdup(entry->d_name);

        int len = strlen(entry->d_name);
        if (len > max_len)
            max_len = len;

        count++;
    }
    closedir(dp);
	if (count > 1)
    qsort(filenames, count, sizeof(char *), compare_filenames);

    if (count == 0)
        return;

    // Step 2: Determine terminal width
    struct winsize w;
    int term_width = 80; // default fallback
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0)
        term_width = w.ws_col;

    int spacing = 2;
    int col_width = max_len + spacing;
    int curr_width = 0;

    // Step 3: Print horizontally (left-to-right)
    for (int i = 0; i < count; i++)
    {
        int len = strlen(filenames[i]);

        // Wrap to next line if exceeding terminal width
        if (curr_width + col_width > term_width)
        {
            printf("\n");
            curr_width = 0;
        }

        struct stat info;
		char path[1024];
		snprintf(path, sizeof(path), "%s/%s", dir, filenames[i]);
	
		if (stat(path, &info) == 0)
		{
		print_colored(filenames[i], dir, info.st_mode);
		int pad = col_width - (int)strlen(filenames[i]);
		for (int p = 0; p < pad; p++) printf(" ");
		}
		else
	{
		printf("%-*s", col_width, filenames[i]);
	}

        curr_width += col_width;
    }
    printf("\n");

    // Step 4: Cleanup
    for (int i = 0; i < count; i++)
        free(filenames[i]);
    free(filenames);
}
void print_colored(const char *name, const char *dir, mode_t mode)
{
    char path[1024];
    snprintf(path, sizeof(path), "%s/%s", dir, name);

    // Determine color based on file type
    const char *color = COLOR_RESET;

    if (S_ISDIR(mode))
        color = COLOR_BLUE;   // Directory → Blue
    else if (S_ISLNK(mode))
        color = COLOR_PINK;   // Symbolic Link → Pink
    else if (S_ISCHR(mode) || S_ISBLK(mode) || S_ISSOCK(mode))
        color = COLOR_REVERSE; // Special Files → Reverse video
    else if (mode & S_IXUSR)
        color = COLOR_GREEN;  // Executable → Green
    else if (strstr(name, ".tar") || strstr(name, ".gz") || strstr(name, ".zip"))
        color = COLOR_RED;    // Archives → Red

    printf("%s%s%s", color, name, COLOR_RESET);
}
