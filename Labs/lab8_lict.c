#include <stdio.h>
#include <dirent.h>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "not enough arguments\n");
		return 1;
	}

	DIR* dir = opendir(argv[1]);

	if (dir == NULL){
		fprintf(stderr, "Problem with opendir and %s as directory name\n", argv[1]);
		return 1;
	}

	struct dirent* entry = readdir(dir);

	while (entry != NULL){
		fprintf(stderr, "File Inode: %lu Filename: %s File type: %d\n", entry->d_ino, entry->d_name, entry->d_type);

		switch (entry->d_type){
			case DT_DIR:
				printf("%s directory\n", entry->d_name);
				break;

			case DT_LNK:
				printf("%s symlink\n", entry->d_name);
				break;

			case DT_REG:
				printf("%s regular\n", entry->d_name);
				break;

			default:
				printf("%s other\n", entry->d_name);
		}


		entry = readdir(dir);

	}

	closedir(dir);


	return 0;
}
