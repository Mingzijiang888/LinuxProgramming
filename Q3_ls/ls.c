#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/sysmacros.h>
#include <grp.h>
#include <pwd.h>

#define LONGEST_FILENAME 512
#define LONGEST_FILENUM  200

#define PERMISSION_NONE 0
#define PERMISSION_a 1
#define PERMISSION_l 2
#define PERMISSION_h 4
#define PERMISSION_R 8

void display(int flag, char* path);
int color(char* path);
void sort(char **a, int num);
void ShowDir(char *dirname);
void ShowADir(char *dirname);
void ShowFile(char *filename, char* dpname);

enum COLOR
{
    COLOR_BLUE = 34, COLOR_GREEN = 32, COLOR_WHITE = 37
};

// Global Flag
int per_a = 0;      // -a, "show all" needed
int per_h = 0;      // -h, "show human size" needed
int per_r = 0;      // -r, "show recursive" needed
// judge if the first recursion
int flag_R = 0;

int main(int argc, char* argv[])
{
    char path[LONGEST_FILENAME] = {0,};
    char argument[32] = {0,};
    int flag = PERMISSION_NONE;     // decide the mode according to the argument
    int k = 0;      // index of argument
    int num = 0;    // index of "option"

    int i = 0, j = 0;      // used for "for" cycle

    // figure out the num of '-'
    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            for (j = 1; j < strlen(argv[i]); j++)   // Attention: fun_strlen() counts including the '\0'
            {
                argument[k] = argv[i][j];
                k++;
            }
            num++;
        }
    }

    for (i = 0; i < k; i++)
    {
        switch (argument[i])
        {
            case 'a':
                flag |= PERMISSION_a;
                per_a = 1;
                break;
            case 'l':
                flag |= PERMISSION_l;
                break;
            case 'h':
                flag |= PERMISSION_h;
                per_h = 1;
                break;
            case 'R':
                flag |= PERMISSION_R;
                per_r = 1;
                break;
            default:
                printf("error: invalid option -%c\n", argument[i]);
                return -1;
        }
    }

    if (argc == num + 1)
    {
        strncpy(path, ".", sizeof(path));
        path[1] = '\0';
        display(flag, path);
    }
    else
    {
        for (i = 1; i < argc; i++)
        {
            if (argv[i][0] == '-')
            {
                continue;
            }
            else
            {
                strncpy(path, argv[i], sizeof(path));
                path[strlen(argv[i])] = '\0';
                display(flag, path);
            }
        }
    }

    putchar('\n');
    return 0;
}

void display(int flag, char* path)
{
    if (path == NULL)
    {
        printf("[line:%d] error path \n", __LINE__);
        return;
    }

    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    if (stat(path, &st) < 0)
    {
        perror("stat");
        return;
    }
 
    if (flag & PERMISSION_l)
    {
        if (S_ISDIR(st.st_mode))    // charge if it is a directory
        {
            ShowADir(path);
        }
        else
        {
            ShowFile(path, path);
        }
    }
    else
    {
        ShowDir(path);
    }
}


// fail: -1;  
int color(char* path)
{
    if (path == NULL)
    {
        printf("[line:%d] error path \n", __LINE__);
        return -1;
    }

    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    if (stat(path, &st) < 0)
    {
        perror("stat");
        return -1;
    }
    
    if (S_ISDIR(st.st_mode))
    {
        return COLOR_BLUE;      //blue
    }
    else if(S_ISREG(st.st_mode) && (st.st_mode & S_IXUSR))  /* Execute by owner.  */
    {
        return COLOR_GREEN;      //green
    }
    else
    {
        return COLOR_WHITE;      //white
    }
}

void sort(char **a, int num)
{
    if (a == NULL)
    {
        printf("[line:%d] Error\n", __LINE__);
        return;
    }

    int i = 0, j = 0;
    char temp[LONGEST_FILENAME + 1] = {0,};
    for (i = 0; i < num; i++)
    {
        for (j = i + 1; j < num; j++)
        {
            if (strcmp(a[i], a[j]) > 0)
            {
                // strcpy(temp, a[i]);
                // strcpy(a[i], a[j]);
                // strcpy(a[j], temp);
                strncpy(temp, a[i], LONGEST_FILENAME);
                strncpy(a[i], a[j], LONGEST_FILENAME);
                strncpy(a[j], temp, LONGEST_FILENAME);
            }
        }
    }
}

char bytes2human(long* x)
{
    if (x == NULL)
    {
        printf("[line:%d] error value \n", __LINE__);
        return '\0';
    }

    char symbols[] = {'\0','K', 'M', 'G', 'T'};
    int i = 0;
    while (*x >= LONGEST_FILENAME)
    {
        *x = (long)(((float)(*x) / LONGEST_FILENAME) + 0.5);
        i++;
    }
    return  symbols[i];
}

// Show the file/director with '-l' (Recursive)
void ShowDir(char *dirname)
{
    int ret = 0;
    if (dirname == NULL)
    {
        printf("[line:%d] error dirname \n", __LINE__);
        return;
    }

    char pathname[LONGEST_FILENAME] = {0,};
    DIR* dir = NULL;
    struct dirent* dp = NULL;
    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    char* name[LONGEST_FILENUM] = {0,};       // 200 row, support most 200 files
    int NumOfDirectory = 0;        // num of dir under current directory

    char** sort_name = (char**)malloc(sizeof(char*) * LONGEST_FILENUM);     // 200 row, support most 200 files
    memset(sort_name, 0, sizeof(char*) * LONGEST_FILENUM);

    int n = 0;      // num of files, including the dir
    int i = 0;

    if ((dir = opendir(dirname)) == NULL)
    {
        perror("opendir:");
        return;
    }

    if (per_r)
    {
        if(flag_R)
        {
            printf("\n\n");
            printf("\033[37m%s:\n", dirname);
        }
        else
        {
            printf("\033[37m%s:\n", dirname);
        }
    }
    
    if (per_a)
    {
        printf("\033[%dm.    ", color("."));
        printf("\033[%dm..    ", color(".."));
    }

    while ((dp = readdir(dir)) != NULL)   // the pointer will move on after once execute ? yes
    {
        if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
        {
            continue;
        }
        // override the hidden file when without -a
        if ((per_a == 0) && (dp->d_name[0] == '.'))
        {
            continue;
        }

        snprintf(pathname, sizeof(pathname), "%s/%s", dirname, dp->d_name);

        if (stat(pathname, &st) < 0)
        {
            perror("stat");
            return;
        }
        if (S_ISDIR(st.st_mode))        // records the num of dir under current path
        {
            name[NumOfDirectory] = (char*)malloc(sizeof(char) * LONGEST_FILENAME);
            memset(name[NumOfDirectory], 0, sizeof(char) * LONGEST_FILENAME);
            strncpy(name[NumOfDirectory], pathname, LONGEST_FILENAME);
            NumOfDirectory++;
            if (NumOfDirectory >= LONGEST_FILENUM)
            {
                perror("too much file in this path");
                return;
            }
        }

        sort_name[n] = (char*)malloc(sizeof(char) * LONGEST_FILENAME);       // 512 array
        memset(sort_name[n], 0, sizeof(char) * LONGEST_FILENAME);
        snprintf(sort_name[n], LONGEST_FILENAME, "%s", dp->d_name);

        n++;
        if (n >= LONGEST_FILENUM)
        {
            perror("too much file in this path");
            return;
        }
    }

    sort(sort_name, n);
    for (i = 0; i < n; i++)
    {
        snprintf(pathname, sizeof(pathname), "%s/%s", dirname, sort_name[i]);
        printf("\033[%dm%s   ", color(pathname), sort_name[i]);
    }

    closedir(dir);
    if (ret != 0)
    {
        printf("[Line:%d] cannot close the dir", __LINE__);
        return;
    }

    for (i = 0; i < LONGEST_FILENUM; i ++)
    {
        free(sort_name[i]);
        sort_name[i] = NULL;
    }
    free(sort_name);

    if(per_r)
    {
        flag_R = 1;
        for(i = 0; i < NumOfDirectory; i++)
        {
            ShowDir(name[i]);
        }
    }

    for (i = 0; i < LONGEST_FILENUM; i ++)
    {
        free(name[i]);
        name[i] = NULL;
    }
}

void ShowADir(char *dirname)
{
    int ret = 0;
    if (dirname == NULL)
    {
        printf("[line:%d] error dirname \n", __LINE__);
        return;
    }

    char pathname[LONGEST_FILENAME] = {0,};
    DIR* dir = NULL;
    struct dirent* dp = NULL;
    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    char* name[LONGEST_FILENUM] = {0,};
    int NumOfDirectory = 0;

    char** sort_name = (char**)malloc(sizeof(char*) * LONGEST_FILENUM);
    memset(sort_name, 0, sizeof(char*) * LONGEST_FILENUM);
    int n = 0;      // index of sort_name

    int i = 0;

    if ((dir = opendir(dirname)) == NULL)
    {
        perror("opendir:");
        return;
    }

    if (per_r)
    {
        if(flag_R)
        {
            printf("\n");
            printf("%s:\n", dirname);
        }
        else
        {
            printf("%s:\n", dirname);
        }
    }

    if (per_a)
    {
        ShowFile(".", ".");
        ShowFile("..", "..");
    }
        
    while ((dp = readdir(dir)) != NULL)   // the pointer will move on after once execute ? yes
    {
        if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
        {
            continue;
        }
        if ((per_a == 0) && (dp->d_name[0] == '.'))
        {
            continue;
        }
        snprintf(pathname, sizeof(pathname), "%s/%s", dirname, dp->d_name);

        if (stat(pathname, &st) < 0)
        {
            perror("stat");
            return;
        }
        if (S_ISDIR(st.st_mode))
        {
            name[NumOfDirectory] = (char*)malloc(sizeof(char) * LONGEST_FILENAME);
            memset(name[NumOfDirectory], 0, sizeof(char) * LONGEST_FILENAME);
            strncpy(name[NumOfDirectory], pathname, LONGEST_FILENAME);
            NumOfDirectory++;
            if (NumOfDirectory >= LONGEST_FILENUM)
            {
                perror("too much file in this path");
                return;
            }
        }

        sort_name[n] = (char*)malloc(sizeof(char) * LONGEST_FILENAME);
        memset(sort_name[n], 0, sizeof(char) * LONGEST_FILENAME);
        snprintf(sort_name[n], LONGEST_FILENAME, "%s", dp->d_name);
        
        n++;
        if (n >= LONGEST_FILENUM)
        {
            perror("too much file in this path");
            return;
        }
    }

    sort(sort_name, n);
    for (i = 0; i < n; i++)
    {
        snprintf(pathname, sizeof(pathname), "%s/%s", dirname, sort_name[i]);
        ShowFile(pathname, sort_name[i]);
    }

    closedir(dir);
    if (ret != 0)
    {
        printf("[Line:%d] cannot close the dir", __LINE__);
        return;
    }

    for (i = 0; i < LONGEST_FILENUM; i ++)
    {
        free(sort_name[i]);
        sort_name[i] = NULL;
    }
    free(sort_name);

    if(per_r)
    {
        flag_R = 1;
        for(i = 0; i < NumOfDirectory; i++)
        {
            ShowADir(name[i]);
        }
    }

    for (i = 0; i < LONGEST_FILENUM; i ++)
    {
        free(name[i]);
        name[i] = NULL;
    }
}

void ShowFile(char *filename, char* dpname)
{
    if (filename == NULL || dpname == NULL)
    {
        printf("[line:%d] error filename \n", __LINE__);
        return;
    }

    // default: white
    printf("\033[37m");

    struct stat st;
    struct passwd psd1;
    struct group grp1;
    memset(&st, 0, sizeof(struct stat));
    memset(&psd1, 0, sizeof(struct passwd));
    memset(&grp1, 0, sizeof(struct group));

    struct passwd* psd = &psd1;
    struct group* grp = &grp1;
    char buffer_time[100] = {0,};
    char buffer1_time[100] = {0,};
    int i = 0;

    if(stat(filename, &st) < 0)
    {
        perror("stat");
        return;
    }

    switch (st.st_mode & S_IFMT)    /* These bits determine file type. */
    {
    case S_IFBLK: 
        printf("b");
        break;
    case S_IFCHR: 
        printf("c");
        break;
    case S_IFDIR: 
        printf("d");
        break;
    case S_IFIFO: 
        printf("f");
        break;
    case S_IFLNK: 
        printf("l");
        break;
    case S_IFREG: 
        printf("-");
        break;
    case S_IFSOCK: 
        printf("s");
        break;
    default:
        printf(" ");
        break;
    }

    // for(int i = 0; i < 9; i++)       // bit calculate
    // {
    //     printf("%c", st.st_mode & (0400 >> i) ? arr[i % 3] : '-');
    // }
    putchar((st.st_mode & S_IRUSR) ? 'r' : '-');
    putchar((st.st_mode & S_IWUSR) ? 'w' : '-');
    putchar((st.st_mode & S_IXUSR) ? 'x' : '-');
    putchar((st.st_mode & S_IRGRP) ? 'r' : '-');
    putchar((st.st_mode & S_IWGRP) ? 'w' : '-');
    putchar((st.st_mode & S_IXGRP) ? 'x' : '-');
    putchar((st.st_mode & S_IROTH) ? 'r' : '-');
    putchar((st.st_mode & S_IWOTH) ? 'w' : '-');
    putchar((st.st_mode & S_IXOTH) ? 'x' : '-');
    
    psd = getpwuid(st.st_uid);
    grp = getgrgid(st.st_gid);
    printf(" %ld ", (long)st.st_nlink);
    printf("%-8s ", psd->pw_name);
    printf("%-8s ", grp->gr_name);

    if (per_h)
    {
        long temp = (long)st.st_size;
        char ch = bytes2human(&temp);
        printf("%ld%c\t", temp, ch);
    }
    else
    {
        printf("%ld\t", (long)st.st_size);        
    }

    strncpy(buffer_time, ctime(&st.st_mtime), sizeof(buffer_time) - 1);
    buffer_time[strlen(buffer_time) - 1] = '\0';    // delete the '\n'
    for (i = 4; i < 16; i++)
    {
        buffer1_time[i - 4] = buffer_time[i];
    }
    buffer1_time[12] = '\0';
    printf(" %s ", buffer1_time);

    printf("\033[%dm%s\n", color(filename), dpname);
}

