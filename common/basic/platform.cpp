
#include "platform.h"
#include "stringFunctions.h"
#ifdef WIN32
#include <windows.h>
#else//WIN32
#include <sys/types.h>	//系统类型定义
#include <errno.h>		//包含此文件就会设置errno变量
#include <netdb.h>
#include <sys/stat.h>
#include <unistd.h>
#endif//WIN32


long get_error(){
	long _error = 0;
#ifdef WIN32
	_error = ::GetLastError();
#else//WIN32
	_error = errno;
#endif//WIN32
	return _error;
}

const char*	get_error_str(long _error){
	const char* _error_str = nullptr;
#ifdef WIN32
	static char szMessage[512] = {0};
	_error_str = szMessage;
	long _size = ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
										nullptr,
										_error,
										MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),/*默认语言*/ 
										szMessage,
										sizeof(szMessage),
										nullptr );

	szMessage[512 - 1] = 0;
	for (long i = 0;i < _size;i++){
		if (szMessage[i] < 0 || szMessage[i] > 31) continue;
		szMessage[i] = 0;
		break;
	}
#else//WIN32
	_error_str = strerror(_error);
#endif//WIN32
	return _error_str;
}

const char*	get_herror_str(){
	const char* _error_str = nullptr;
#ifdef WIN32
	_error_str = get_error_str(get_error());
#else//WIN32
	char* _str = nullptr;
	herror(_str);
	_error_str = _str;
#endif//WIN32
	return _error_str;
}

const char*	get_herror_str(long _error){
	const char* _error_str = nullptr;
#ifdef WIN32
	_error_str = get_error_str(_error);
#else//WIN32
	_error_str = hstrerror(_error);
#endif//WIN32
	return _error_str;
}

bool findDirectory(pc_str _dir){
	if (!_dir || !*_dir)return false;
#ifdef WIN32
	WIN32_FIND_DATAA FileData;
	HANDLE hSearch;
	hSearch = ::FindFirstFile(_dir, &FileData);
	if (hSearch == INVALID_HANDLE_VALUE){
		::FindClose(hSearch);
		return false;
	}

	if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
		::FindClose(hSearch);
		return true;
	}

	while (::FindNextFile(hSearch, &FileData)){
		if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			::FindClose(hSearch);
			return true;
		}
	}
	::FindClose(hSearch);
#else // WIN32
	struct stat _stat;
	int _res = stat(_dir, &_stat);
	if (_res == 0 && S_ISDIR(_stat.st_mode))
		return true;
#endif // WIN32
	return false;
}

bool createDirectory(pc_str _dir, char _break){
	if (!_dir || !*_dir) return false;

#ifdef WIN32
	std::string strDir(_dir);
	std::string strMkDir;
	while (!strDir.empty()){
		std::string strTemp;
		getSubString(strDir, strTemp, _break);
		if (strTemp.empty()) continue;
		if (!strMkDir.empty()) strMkDir += _break;
		strMkDir += strTemp;
		if (!findDirectory(strMkDir.c_str())){
			if (::CreateDirectory(strMkDir.c_str(), nullptr) != TRUE)
				return false;
		}
	}

#else // WIN32
	std::string strDir(_dir);
	std::string strMkDir;
	while (!strDir.empty()){
		std::string strTemp;
		getSubString(strDir, strTemp, _break);
		if (strTemp.empty()) continue;
		if (!strMkDir.empty()) strMkDir += _break;
		strMkDir += strTemp;
		if (access(strMkDir.c_str(), 0) != 0){
			if (mkdir(strMkDir.c_str(), 0755) == -1) return false;
		}
	}
#endif // WIN32
	return true;
}


/*linux
//------------------------------ 
stat（取得文件状态）  
相关函数  fstat，lstat，chmod，chown，readlink，utime
表头文件  #include<sys/stat.h>
#include<unistd.h>
定义函数  int stat(const char * file_name,struct stat *buf);
函数说明  stat()用来将参数file_name所指的文件状态，复制到参数buf所指的结构中。
下面是struct stat内各参数的说明
//------------------------------ 

//------------------------------ 
*检查是否可以读/写某一已存在的文件
表头文件  #include<unistd.h>
定义函数  int access(const char * pathname,int mode);
@mode:R_OK【读】W_OK【写】X_OK【执行】F_OK【存在】
返回：若所有欲查核的权限都通过了检查则返回0值，表示成功，只要有一权限被禁止则返回-1
错误代码:
EACCESS 参数pathname 所指定的文件不符合所要求测试的权限。
EROFS 欲测试写入权限的文件存在于只读文件系统内。
EFAULT 参数pathname指针超出可存取内存空间。
EINVAL 参数mode 不正确。
ENAMETOOLONG 参数pathname太长。
ENOTDIR 参数pathname为一目录。
ENOMEM 核心内存不足
ELOOP 参数pathname有过多符号连接问题。
EIO I/O 存取错误。
//------------------------------ 
*依字母顺序排序目录结构
表头文件  #include<dirent.h>
定义函数  int alphasort(const struct dirent **a,const struct dirent **b);
函数说明  alphasort()为scandir()最后调用qsort()函数时传给qsort()作为判断的函数，详细说明请参考scandir()及qsort()。
返回值  参考qsort()。
范例:
//* 读取/目录下所有的目录结构，并依字母顺序排列
main()
{
	struct dirent **namelist;
	int i,total;
	total = scandir(“/”,&namelist ,0,alphasort);
	if(total <0)
		perror(“scandir”);
	else
	{
		for(i=0;i<total;i++)
			printf(“%s\n”,namelist[i]->d_name);
		printf(“total = %d\n”,total);
	}
}
//------------------------------ 

//------------------------------ 
*chdir（改变当前的工作（目录）  
相关函数  getcwd，chroot
表头文件  #include<unistd.h>
定义函数  int chdir(const char * path);
函数说明  chdir()用来将当前的工作目录改变成以参数path所指的目录。
返回值  执行成功则返回0，失败返回-1，errno为错误代码。
//------------------------------ 
*fchdir（改变当前的工作目录）  
相关函数  getcwd，chroot
表头文件  #include<unistd.h>
定义函数  int fchdir(int fd);
函数说明  fchdir()用来将当前的工作目录改变成以参数fd 所指的文件描述词。
返回值执  行成功则返回0，失败返回-1，errno为错误代码。
//------------------------------ 
*getcwd（取得当前的工作目录）  
相关函数  get_current_dir_name，getwd，chdir
表头文件  #include<unistd.h>
定义函数  char * getcwd(char * buf,size_t size);
函数说明  getcwd()会将当前的工作目录绝对路径复制到参数buf所指的内存空间，参数size为buf的空间大小。
在调用此函数时，buf所指的内存空间要足够大，若工作目录绝对路径的字符串长度超过参数size大小，则回值NULL，
errno的值则为ERANGE。倘若参数buf为NULL，getcwd()会依参数size的大小自动配置内存(使用malloc())，
如果参数size也为0，则getcwd()会依工作目录绝对路径的字符串程度来决定所配置的内存大小，进程可以在使用完此字符串后利用free()来释放此空间。
返回值  执行成功则将结果复制到参数buf所指的内存空间，或是返回自动配置的字符串指针。失败返回NULL，错误代码存于errno。
//------------------------------ 

//------------------------------ 
*chmod（改变文件的权限）  
相关函数  fchmod，stat，open，chown
表头文件  #include<sys/types.h>
#include<sys/stat.h>
定义函数  int chmod(const char * path,mode_t mode);
函数说明  chmod()会依参数mode 权限来更改参数path 指定文件的权限。

//------------------------------ 
closedir（关闭目录）  
相关函数  opendir
表头文件  #include<sys/types.h>
#include<dirent.h>
定义函数  int closedir(DIR *dir);
//------------------------------ 
opendir（打开目录）  
相关函数  open，readdir，closedir，rewinddir，seekdir，telldir，scandir
表头文件  #include<sys/types.h>
#include<dirent.h>
定义函数  DIR * opendir(const char * name);
函数说明  opendir()用来打开参数name指定的目录，并返回DIR*形态的目录流，和open()类似，接下来对目录的读取和搜索都要使用此返回值。
返回值  成功则返回DIR* 型态的目录流，打开失败则返回NULL。
//------------------------------ 
readdir（读取目录）  
相关函数  open，opendir，closedir，rewinddir，seekdir，telldir，scandir
表头文件  #include<sys/types.h>
#include<dirent.h>
定义函数  struct dirent * readdir(DIR * dir);
函数说明  readdir()返回参数dir目录流的下个目录进入点。
结构dirent定义如下
struct dirent
{
	ino_t d_ino;
	ff_t d_off;
	signed short int d_reclen;
	unsigned char d_type;
	har d_name[256;
	};
d_ino 此目录进入点的inode
d_off 目录文件开头至此目录进入点的位移
d_reclen _name的长度，不包含NULL字符
d_type d_name 所指的文件类型
d_name 文件名
返回值  成功则返回下个目录进入点。有错误发生或读取到目录文件尾则返回NULL。
//------------------------------ 
rewinddir（重设读取目录的位置为开头位置）  
相关函数  open，opendir，closedir，telldir，seekdir，readdir，scandir
表头文件  #include<sys/types.h>
#include<dirent.h>
定义函数  void rewinddir(DIR *dir);
函数说明  rewinddir()用来设置参数dir 目录流目前的读取位置为原来开头的读取位置。
//------------------------------ 
seekdir（设置下回读取目录的位置）  
相关函数  open，opendir，closedir，rewinddir，telldir，readdir，scandir
表头文件  #include<dirent.h>
定义函数  void seekdir(DIR * dir,off_t offset);
函数说明  seekdir()用来设置参数dir目录流目前的读取位置，在调用readdir()时便从此新位置开始读取。参数offset 代表距离目录文件开头的偏移量。
//------------------------------ 
telldir（取得目录流的读取位置）  
相关函数  open，opendir，closedir，rewinddir，seekdir，readdir，scandir
表头文件  #include<dirent.h>
定义函数  off_t telldir(DIR *dir);
函数说明  telldir()返回参数dir目录流目前的读取位置。此返回值代表距离目录文件开头的偏移量返回值返回下个读取位置，有错误发生时返回-1。
错误代码  EBADF参数dir为无效的目录流。
//------------------------------ 

//------------------------------ 
utime（修改文件的存取时间和更改时间）  
相关函数  utimes，stat
表头文件  #include<sys/types.h>
#include<utime.h>
定义函数  int utime(const char * filename,struct utimbuf * buf);
函数说明  utime()用来修改参数filename文件所属的inode存取时间。
结构utimbuf定义如下
struct utimbuf
{
	time_t actime;
	time_t modtime;
};
//------------------------------ 

//------------------------------ 
remove（删除文件）  
相关函数  link，rename，unlink
表头文件  #include<stdio.h>
定义函数  int remove(const char * pathname);

//------------------------------ 
rename（更改文件名称或位置）  
相关函数  link，unlink，symlink
表头文件  #include<stdio.h>
定义函数  int rename(const char * oldpath,const char * newpath);
函数说明  rename()会将参数oldpath 所指定的文件名称改为参数newpath所指的文件名称。若newpath所指定的文件已存在，则会被删除。
返回值  执行成功则返回0，失败返回-1，错误原因存于errno
*/
//-------------------------------------------------------------