#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

void usage(const char* proc)
{
	printf("usage : %s [filename]\n", proc);
}

int main(int argc, char const *argv[])
{
	DIR *dp;
	struct dirent *dirp;
	if (argc < 2){
		usage(argv[0]);
		return 1;
	}
	if ( (dp = opendir(argv[1])) == NULL){ /* 打开文件 */
		perror("opendir");
	}
	while ( (dirp = readdir(dp)) != NULL)
		printf("%s\n", dirp->d_name);

	closedir(dp);
	return 0;
}
