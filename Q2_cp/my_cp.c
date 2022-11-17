#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>

////若需要程序打印调试日志，打开为'1'
#if 1
#define debug_print(format, ...) printf("\033[0;31m[%s:%d]%s\033[m\n", __FUNCTION__, __LINE__, format)
#else
#define debug_print(format, ...) 
#endif

#define MAX_PATHNAME 512

int r_flag = 0;      // -r, "show recursive" needed
int i_flag = 0;      // -i, "inqury" needed
int r_special_flag = 0;
char r_special_destbuf[MAX_PATHNAME];	//为-r存在的特殊情况时所用

void do_copy(char* sourcepath, char* destpath);
void file2file(char* source, char* dest);
void dir2dir(char* source, char* dest);

int main(int argc, char* argv[])
{
    int i = 0, j = 0;			////用于循环
	int num_opt = 0;			////带'-'选项的个数
	char option[32] = {0,};		////提取带'-'的选项
	int num_except_opt = 0;		////除带'-'的选项外的参数个数
    char id[MAX_PATHNAME] = {0,};		////源路径	
    char od[MAX_PATHNAME] = {0,};		////目标路径

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
		else if (0 == strlen(id)) //id为空时才赋值，即将第一个出现的非'-'字符串当作id
		{
			strncpy(id, argv[i], MAX_PATHNAME - 1);      // source path
			++num_except_opt;
		}
		else if (num_except_opt < 2)
		{
			strncpy(od, argv[i], MAX_PATHNAME - 1);      //dest path
			++num_except_opt;
		}
    }

	////判断需要的选项
    for (i = 0; i < num_opt; i++)
    {
        switch (option[i])
        {
            case 'r':
                r_flag = 1;
                break;
            case 'i':
                i_flag = 1;
                break;
            default:
                printf("cp: invalid option -- \'%c\'\n", option[i]);
                exit(-1);
        }
    }

	////参数错误
	if (0 == num_except_opt)
	{
		printf("cp: missing file operand\n");
		exit(1);
	}
	else if (1 == num_except_opt)
	{
		printf("cp: missing destination file operand after \'%s\'\n", argv[num_except_opt]);
		exit(1);
	}

	////将od字符串可能存在的最后一个'/'去掉，防止后续拼接后缀时，出现两个'/'连一起的路径名
	char* pOd = NULL;
	pOd = od + strlen(od) - 1;
	if ('/' == (*pOd))
	{
		*pOd = '\0';		
	}
	pOd = NULL;

	//开始复制
    do_copy(id, od);
}

static void joint_suffix(char* id, char* od, char* destpath)
{
    ////拼接后缀
    char* pSrc = NULL;
    pSrc = id + strlen(id) - 1;
    while ((pSrc >= id) && '/' != (*pSrc))
    {
        pSrc--;
    }
    pSrc++;
    snprintf(destpath, MAX_PATHNAME, "%s/%s", od, id);
}

static int check_od_validity(char* id, char* od)
{
	char* pDest = NULL;
    char temp_destpath[MAX_PATHNAME] ={0,};
	struct stat id_stat;
	memset(&id_stat, 0, sizeof(struct stat));
	lstat(id, &id_stat);
    strncpy(temp_destpath, od, MAX_PATHNAME - 1);
    pDest = temp_destpath + strlen(temp_destpath) - 1;
    while ((pDest > temp_destpath) && '/' != (*pDest))
    {
        *pDest = '\0';
        pDest--;
    }
    *pDest = '\0';

	////排除长度为0的情况，类似cp 123 bb/123，而bb不存在的情况
    if (0 != strlen(temp_destpath) && 0 != access(temp_destpath, F_OK)) 
    {
        if (1 == S_ISDIR(id_stat.st_mode)) //若源路径是目录
        {
            if (1 != r_flag)
            {
                printf("cp: -r not specified; omitting directory \'%s\'\n", id);
                exit(1);
            }
            printf("cp: cannot create directory '%s': No such file or directory\n", od);
        }
        else //若源路径是文件 
        {
            printf("cp: cannot create regular file '%s': No such file or directory\n", od);
        }
        exit(1);
    }
	return 0;
}

static int check_od_special(char* id, char* od, char* destpath)
{
	int ret = 0;
	int if_special = 0;	//标志是否属于特殊情况
	struct stat id_stat;
	memset(&id_stat, 0, sizeof(struct stat));
	lstat(id, &id_stat);
	char temp_idpath[MAX_PATHNAME] = {0,};
    char temp_currpath[MAX_PATHNAME] = {0,};
    char temp_odpath[MAX_PATHNAME] = {0,};

	////目标路径不存在
	if (0 != access(od, F_OK))
	{
		////先创建新文件夹
		ret = mkdir(od, id_stat.st_mode);
		if (-1 == ret)
		{
			debug_print("[line:%d] mkdir failed \n", __LINE__);
			exit(1);
		}
		////检测od是否为id的一个子路径
		getcwd(temp_currpath, MAX_PATHNAME);
		chdir(id);
		getcwd(temp_idpath, MAX_PATHNAME);
		chdir(temp_currpath);
		chdir(od);
		getcwd(temp_odpath, MAX_PATHNAME);
		chdir(temp_currpath);
		if (0 == strncmp(temp_idpath, temp_odpath, strlen(temp_idpath)))
		{
			if_special = 1;
			r_special_flag = 1;
			strncpy(r_special_destbuf, od, MAX_PATHNAME);
		}
	}
	else //目标路径存在
	{
		////先拼接后缀
		joint_suffix(id, od, destpath); 
		////检测od是否为id的一个子路径
		getcwd(temp_currpath, MAX_PATHNAME);
		chdir(id);
		getcwd(temp_idpath, MAX_PATHNAME);
		chdir(temp_currpath);
		chdir(od);
		getcwd(temp_odpath, MAX_PATHNAME);
		chdir(temp_currpath);
		if (0 == strncmp(temp_idpath, temp_odpath, strlen(temp_idpath)))
		{
			if_special = 1;
			r_special_flag = 1;
			if (0 == access(destpath, F_OK))
			{
				r_special_flag = 0;
			}
			else
			{
				ret = mkdir(destpath, id_stat.st_mode);
				if (-1 == ret)
				{
					debug_print("[line:%d] mkdir failed \n", __LINE__);
					exit(1);
				}
			}
			strncpy(r_special_destbuf, destpath, MAX_PATHNAME);
		}
	}
#if 0 
	printf("%d\n", r_special_flag);
	printf("%s\n", temp_currpath);
	printf("%s\n", temp_idpath);
	printf("%s\n", temp_odpath);
	printf("%s\n", id);
	printf("%s\n", destpath);
#endif
	////特殊情况，调用dir2dir
	if (1 == if_special)
	{
		dir2dir(id, destpath);
	}
	return if_special;
}

void do_copy(char* id, char* od)
{
	int ret = 0;
	char destpath[MAX_PATHNAME] = {0};	//cp的目的路径
	struct stat id_stat;
    struct stat od_stat;
    memset(&id_stat, 0, sizeof(struct stat));
    memset(&od_stat, 0, sizeof(struct stat));
	if (NULL == id || NULL == od)
	{
		debug_print("path error");
		exit(1);
	}

	////获取id相关信息
	ret = lstat(id, &id_stat);
	if (0 != ret)
	{
		printf("cp: cannot stat \'%s\': No such file or directory\n", id);
		exit(1);
	}

    ////判断除掉目标路径的最后一部分后，是否还是合法路径，如cp 123 bb/123，而bb不存在的情况
	ret = check_od_validity(id, od);
	////路径合法，先赋od给destpath
	if (0 == ret)
	{
		strncpy(destpath, od, MAX_PATHNAME);
	}

	////若源路径是目录
	if (1 == S_ISDIR(id_stat.st_mode))
	{
		////需加选项'-r'
		if (1 != r_flag)
		{
			printf("cp: -r not specified; omitting directory \'%s\'\n", id);
			exit(1);
		}

        ////检测od是否为特殊情况，即od是否为id的一个子路径
        ret = check_od_special(id, od, destpath);
		if (1 == ret)
		{
			printf("cp: cannot copy a directory, \'%s\', into itself, \'%s\'\n", id, destpath);
			exit(1);
		}
        
		////od不是特殊情况
		////od不存在
		if (0 != access(od, F_OK))
		{	
			////创建新文件夹，进行递归复制
			ret = mkdir(od, id_stat.st_mode);
			if (-1 == ret)
			{
				debug_print("[line:%d] mkdir failed \n", __LINE__);
				exit(1);
			}	
			////dir->dir
			dir2dir(id, destpath);
		}
		else	//目标路径存在
		{
			////获取od相关信息
			ret = lstat(od, &od_stat);
			if (0 != ret)
			{
				debug_print("get stat error");
			}

			////目标路径是文件，报错
			if (0 == S_ISDIR(od_stat.st_mode))
			{
				printf("cp: cannot overwrite non-directory \'%s\' with directory \'%s\'\n", od, id);
				exit(1);
			}
			else	//目标路径是目录
				////若拼接后的路径已存在
				if (0 ==access(destpath, F_OK))
				{
					////但判断拼接后的路径与id指向同一inode，则报错
					struct stat temp_dest_stat;
					memset(&temp_dest_stat, 0, sizeof(struct stat));
					ret = lstat(destpath, &temp_dest_stat);
					if (0 != ret)
					{
					////但判断拼接后的路径与id指向同一inode，则报错
					struct stat temp_dest_stat;
					memset(&temp_dest_stat, 0, sizeof(struct stat));
					ret = lstat(destpath, &temp_dest_stat);
					if (0 != ret)
					{
						debug_print("get stat error");
					}
					if (id_stat.st_ino == temp_dest_stat.st_ino)
					{
							
						exit(1);
					}

					////若拼接后是已存在的文件，也报错
					if (0 == S_ISDIR(temp_dest_stat.st_mode))
					{
						printf("cp: cannot overwrite non-directory \'%s\' with directory \'%s\'\n", destpath, id);
						exit(1);
					}	
				}
				else	////拼接后的路径不存在，先创建新文件夹
				{
					ret = mkdir(destpath, id_stat.st_mode);
					if (0 != ret)
					{
						debug_print("[line:%d] mkdir failed \n", __LINE__);
						exit(1);
					}
				}

				////dir->dir
				dir2dir(id, destpath);
			}
		}
	}
	else	////若源路径是文件
	{
		////目标路径不存在
		if (0 != access(od, F_OK))
		{
			////路径合法，直接file->file
			file2file(id, destpath);
		}
		else	//目标路径存在
		{
			ret = lstat(od, &od_stat);
			if (0 != ret)
			{
				debug_print("get stat error");
			}

			////目标路径是文件
			if (0 == S_ISDIR(od_stat.st_mode))
			{
				////先检测源路径和目标路径是否指向同一文件inode，若是则报错
				if (od_stat.st_ino == id_stat.st_ino)
				{
					printf("cp: \'%s\' and \'%s\' are the same file\n", id, od);
					exit(1);
				}
				
				////再判断'-i'覆盖标志
				if (i_flag)
                {
                    printf("cp: overwrite \'%s\'? ", od);
                    int temp = getchar();
                    if ('y' != temp)
                    {
						while ('\n' != getchar());
						exit(1);
                    }
					while ('\n' != getchar());
                }

				file2file(id, destpath);
			}
			else	////目标路径是目录
			{
				////拼接最终目标路径的路径名
				joint_suffix(id, od, destpath);

				////若拼接后的路径已存在，先判断是否指向同一inode，再判断'-i'覆盖标志；若无，进行复制
				if (0 == access(destpath, F_OK))
				{
					////判断拼接后的路径是否与源路径指向同一inode，如cp 123 ../cp2/ -- 假设当前目录在cp2文件夹下
					struct stat temp_dest_stat;
					memset(&temp_dest_stat, 0, sizeof(struct stat));
					ret = lstat(destpath, &temp_dest_stat);
					if (id_stat.st_ino == temp_dest_stat.st_ino)
					{
						printf("cp: \'%s\' and \'%s\' are the same file\n", id, destpath);
						exit(1);
					}

					if (i_flag)
					{
						printf("cp: overwrite \'%s\'? ", destpath);
						int temp = getchar();
						if ('y' != temp)
						{
							while ('\n' != getchar());
							exit(1);
						}
						while ('\n' != getchar());
					}
				}

				////file->file
				file2file(id, destpath);
			}
		}
	}
}

void file2file(char* source, char* dest)
{
	if (NULL == source || NULL == dest)
    {
		debug_print("path error");
        exit(1);
    }

	int ret = 0;
    int source_fd = 0;
    int dest_fd = 0;
    char buf[1024] = {0,};
	struct stat source_stat;
    memset(&source_stat, 0, sizeof(struct stat));
	stat(source, &source_stat);
	int datacount = (int)(source_stat.st_size);
    int read_count = 1024;
    int write_count = 0;

	source_fd = open(source, O_RDONLY);
	if (-1 == source_fd)
	{
		debug_print("open file error");
		exit(1);
	}

	dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, source_stat.st_mode);
	if (-1 == dest_fd)
	{
		debug_print("open file error");
		exit(1);
	}

	while(read_count > 0)
	{
		read_count = (read_count > datacount) ? datacount : read_count;
		if(read_count == (write_count = read(source_fd, buf, read_count)))
		{
			ret = write(dest_fd, buf, write_count);
			if (-1 == ret)
			{
				debug_print("write error");
				exit(1);
			}
		}
		else
		{
			debug_print("read error");
			exit(1);
		}
		
		datacount = datacount - write_count;
	}
	
	ret = close(source_fd);
    if (-1 == ret)
    {
        debug_print("close failed");
        exit(1);
    }
    ret = close(dest_fd);
    if (-1 == ret)
    {
        debug_print("close failed");
        exit(1);
    }
}

void dir2dir(char* source, char* dest)
{
    if (NULL == source || NULL == dest)
    {
        exit(1);
    }

    int ret = 0;
    DIR* currentdir = NULL;
    struct dirent* currentdp = NULL;
    struct stat currentstat;
    memset(&currentstat, 0, sizeof(struct stat));
    char sourcepath[512] = {0,}; 
    char destpath[512] = {0,};
    
    currentdir = opendir(source);
    if (NULL == currentdir)
    {
        debug_print("open dir error");
    }

	////逐一读取文件夹下的项目
    while (NULL != (currentdp = readdir(currentdir)))
    {
		////忽略掉readdir中，名称为"."和".."的目录
		if (0 != strcmp(currentdp->d_name, ".") && 0 != strcmp(currentdp->d_name, ".."))
        {
			////拼接后缀
            snprintf(sourcepath, MAX_PATHNAME, "%s/%s", source, currentdp->d_name);
            ret = lstat(sourcepath, &currentstat);
			if (-1 == ret)
            {
                debug_print("get stat error");
                exit(1);
            }

			////因为特殊情况的存在，被迫加的这一段
			if (1 == r_special_flag)
			{
				struct stat temp_deststat;
				memset(&temp_deststat, 0, sizeof(struct stat));
				lstat(r_special_destbuf, &temp_deststat);
				//printf("%d, %s\n", __LINE__, sourcepath);
				//printf("%d, %d\n", (int)temp_deststat.st_ino, (int)currentstat.st_ino);
				if (temp_deststat.st_ino == currentstat.st_ino)
				{
					continue;
				}
			}
			
            snprintf(destpath, MAX_PATHNAME, "%s/%s", dest, currentdp->d_name);

			////若当前路径是目录，则递归（深度优先）
            if (S_ISDIR(currentstat.st_mode))   
            {
                if (-1 == access(destpath, F_OK))	
                {
                    ret = mkdir(destpath, currentstat.st_mode);
                    if (-1 == ret)
                    {
						debug_print("mkdir error");
                        exit(1);
                    }
					if (0 == r_special_flag)
					{
						r_special_flag = 1;
						strncpy(r_special_destbuf, destpath, MAX_PATHNAME);
					}
                }
                dir2dir(sourcepath, destpath);
            }
            else	////是文件，file->file
            {
				////若文件已存在，-i询问
                if (0 == access(destpath, F_OK))
                {
					if (i_flag)
					{
						printf("cp: overwrite \'%s\'? ", destpath);
						int temp = getchar();
						if ('y' != temp)
						{
							while ('\n' != getchar()); //除掉用户输入后的多余字符(包括换行符)
							continue;
						}
						while ('\n' != getchar());
					}
				}
				file2file(sourcepath, destpath);
            }
        }
    }

    ret = closedir(currentdir);
    if (-1 == ret)
    {
		debug_print("closedir error");
        exit(1);
    }
}




