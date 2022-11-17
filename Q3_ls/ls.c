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

#define MAX_FILENAME 512
#define MAX_FILENUM  200

////全局标记符
int flag_a = 0;    //-a, "show all"
int flag_h = 0;    //-h, "human readable"
int flag_l = 0;    //-l, "long list format"
int flag_R = 0;    //-R, "recursive"
int flag_R_first = 0; //-R有效时记录第一次递归

enum COLOR
{
    COLOR_BLUE = 34,
    COLOR_GREEN = 32,
    COLOR_WHITE = 37,
};

void display(char* path);
int color(char* path);
void sort(char **a, int num);
void ShowDir(char *dirname);
void ShowADir(char *dirname);
void ShowFile(char *filename, char* dpname);


int main(int argc, char* argv[])
{
    int i = 0, j = 0;   //用于循环
    int num_opt = 0;            ////带'-'选项的个数
    char option[32] = {0,};    //提取带'-'的选项
    int num_except_opt = 0;     ////除带'-'的选项外的参数个数
    char path[MAX_FILENAME] = {0,}; //ls的路径

    ////提取带'-的选项参数，无论位置和个数
    for (i = 1; i < argc; i++)
    {
        if ('-' == argv[i][0])
        {
            for (j = 1; j < strlen(argv[i]); j++)   // Attention: fun_strlen() counts including the '\0'
            {
                option[num_opt] = argv[i][j];
                num_opt++;
            }
        }
        else if (0 == strlen(path)) //id为空时才赋值，即将第一个出现的非'-'字符串当作id
        {
            strncpy(path, argv[i], MAX_FILENAME - 1);      // source path
            ++num_except_opt;
        }
    }

    ////判断需要的选项
    for (i = 0; i < num_opt; i++)
    {
        switch (option[i])
        {
            case 'a':
                flag_a = 1;
                break;
            case 'l':
                flag_l = 1;
                break;
            case 'h':
                flag_h = 1;
                break;
            case 'R':
                flag_R = 1;
                break;
            default:
                printf("ls: invalid option -- %c\n", option[i]);
                return -1;
        }
    }

    if (0 == num_except_opt)
    {
        strncpy(path, ".", sizeof(path));
        path[1] = '\0';
    }

    display(path);
    putchar('\n');
    return 0;
}

void display(char* path)
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
 
    if (1 == flag_l)
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
    char temp[MAX_FILENAME + 1] = {0,};
    for (i = 0; i < num; i++)
    {
        for (j = i + 1; j < num; j++)
        {
            if (strcmp(a[i], a[j]) > 0)
            {
                // strcpy(temp, a[i]);
                // strcpy(a[i], a[j]);
                // strcpy(a[j], temp);
                strncpy(temp, a[i], MAX_FILENAME);
                strncpy(a[i], a[j], MAX_FILENAME);
                strncpy(a[j], temp, MAX_FILENAME);
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
    while (*x >= MAX_FILENAME)
    {
        *x = (long)(((float)(*x) / MAX_FILENAME) + 0.5);
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

    char pathname[MAX_FILENAME] = {0,};
    DIR* dir = NULL;
    struct dirent* dp = NULL;
    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    char* name[MAX_FILENUM] = {0,};       // 200 row, support most 200 files
    int NumOfDirectory = 0;        // num of dir under current directory

    char** sort_name = (char**)malloc(sizeof(char*) * MAX_FILENUM);     // 200 row, support most 200 files
    memset(sort_name, 0, sizeof(char*) * MAX_FILENUM);

    int n = 0;      // num of files, including the dir
    int i = 0;

    if ((dir = opendir(dirname)) == NULL)
    {
        perror("opendir:");
        return;
    }

    if (flag_R)
    {
        if(flag_R_first)
        {
            printf("\n\n");
            printf("\033[37m%s:\n", dirname);
        }
        else
        {
            printf("\033[37m%s:\n", dirname);
        }
    }
    
    if (flag_a)
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
        if ((flag_a == 0) && (dp->d_name[0] == '.'))
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
            name[NumOfDirectory] = (char*)malloc(sizeof(char) * MAX_FILENAME);
            memset(name[NumOfDirectory], 0, sizeof(char) * MAX_FILENAME);
            strncpy(name[NumOfDirectory], pathname, MAX_FILENAME);
            NumOfDirectory++;
            if (NumOfDirectory >= MAX_FILENUM)
            {
                perror("too much file in this path");
                return;
            }
        }

        sort_name[n] = (char*)malloc(sizeof(char) * MAX_FILENAME);       // 512 array
        memset(sort_name[n], 0, sizeof(char) * MAX_FILENAME);
        snprintf(sort_name[n], MAX_FILENAME, "%s", dp->d_name);

        n++;
        if (n >= MAX_FILENUM)
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

    for (i = 0; i < MAX_FILENUM; i ++)
    {
        free(sort_name[i]);
        sort_name[i] = NULL;
    }
    free(sort_name);

    if(flag_R)
    {
        flag_R_first = 1;
        for(i = 0; i < NumOfDirectory; i++)
        {
            ShowDir(name[i]);
        }
    }

    for (i = 0; i < MAX_FILENUM; i ++)
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

    char pathname[MAX_FILENAME] = {0,};
    DIR* dir = NULL;
    struct dirent* dp = NULL;
    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    char* name[MAX_FILENUM] = {0,};
    int NumOfDirectory = 0;

    char** sort_name = (char**)malloc(sizeof(char*) * MAX_FILENUM);
    memset(sort_name, 0, sizeof(char*) * MAX_FILENUM);
    int n = 0;      // index of sort_name

    int i = 0;

    if ((dir = opendir(dirname)) == NULL)
    {
        perror("opendir:");
        return;
    }

    if (flag_R)
    {
        if(flag_R_first)
        {
            printf("\n");
            printf("%s:\n", dirname);
        }
        else
        {
            printf("%s:\n", dirname);
        }
    }

    if (flag_a)
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
        if ((flag_a == 0) && (dp->d_name[0] == '.'))
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
            name[NumOfDirectory] = (char*)malloc(sizeof(char) * MAX_FILENAME);
            memset(name[NumOfDirectory], 0, sizeof(char) * MAX_FILENAME);
            strncpy(name[NumOfDirectory], pathname, MAX_FILENAME);
            NumOfDirectory++;
            if (NumOfDirectory >= MAX_FILENUM)
            {
                perror("too much file in this path");
                return;
            }
        }

        sort_name[n] = (char*)malloc(sizeof(char) * MAX_FILENAME);
        memset(sort_name[n], 0, sizeof(char) * MAX_FILENAME);
        snprintf(sort_name[n], MAX_FILENAME, "%s", dp->d_name);
        
        n++;
        if (n >= MAX_FILENUM)
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

    for (i = 0; i < MAX_FILENUM; i ++)
    {
        free(sort_name[i]);
        sort_name[i] = NULL;
    }
    free(sort_name);

    if(flag_R)
    {
        flag_R_first = 1;
        for(i = 0; i < NumOfDirectory; i++)
        {
            ShowADir(name[i]);
        }
    }

    for (i = 0; i < MAX_FILENUM; i ++)
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

    if (flag_h)
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

