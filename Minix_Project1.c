#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <malloc.h>

#define TRUE 1
#define READ_BUF_ALLOC 1024
#define TOKEN_SIZE 128
#define TOK_OPEN_PAREN "("
#define TOKEN_COMMA ","
#define TOK_ESCCODE_SPACE " \t\r\n\a"

char* check_alias(char *);
char* alias_exe;
void alias_add(char *);
char *fetch_line(void);
char **split_on_comma(char *line);
char **split_on_paranthese(char *line);
char **split_on_escapeCode(char *line);
int set_flag =0;













struct node {
	char* cmd;
	struct node *next;
};

void allocMem(char **ptr, int size) {

	*ptr = (char*)malloc(size);
	if (ptr == NULL) {
		printf("failed to allocate memory\n");
	}
}

void freeNode(struct node *ptr) {
	struct node * next = NULL;
	if (ptr != NULL) {
		next = ptr->next;
		free(ptr->cmd);
		free(ptr);
		ptr = next;
	}
}


void pareval(char *arg)
{
	struct node* arr[30];
	char **args;
	char* str;
	
	allocMem(&str, strlen(arg));
	strcpy(str, arg);
//	char str[80] = "(ps(ls,ps)dir)";
	char *strDup;
	//char str[80] = "";
	const char s[5] = "()";
	char *token;
	int j = 0;
	for (j = 0; j < 30; j++) {
		arr[j] = NULL;
	}

	allocMem(&strDup, strlen(str));
	strcpy(strDup, str);
	printf("original %s   dup %s\n", str, strDup);

	int i = 0;
	int tokenLength = 0;
	int max = 0;

	/* get the first token */
	token = strtok(str, s);

	/* walk through other tokens */
	while (token != NULL)
	{
		tokenLength = strlen(token);
		struct node* node_to_add = (struct node*)malloc(sizeof(struct  node));
		node_to_add->next = NULL;

		allocMem(&node_to_add->cmd, tokenLength);

		//node_to_add->cmd = (char*)(malloc(sizeof(tokenLength)));
		strcpy(node_to_add->cmd, token);

		if (arr[i] == NULL) {
			arr[i] = node_to_add;

		}
		else {
			struct node* head = arr[i];
			while (head->next != NULL) {
				head = head->next;
			}
			head->next = node_to_add;
		}


		if (strDup[tokenLength] == '(') {
			i++;
		}
		else {
			i--;

		}

		if (max < i) {
			max = i;
		}


		printf(" %s  ", token);

		token = strtok(NULL, s);
	}

	for (i = max; i >= 0; i--) {
		printf("\n%d  cmd ->", i);
		if (arr[i] == NULL) {
			printf("no command in root");
		}
		else {
			struct node* head = arr[i];
			while (head != NULL) {
				printf(" %s ", head->cmd);
				
				args = split_on_comma(head->cmd);
				status = exec_command(args);
				
				
				
				
				head = head->next;
			}
			freeNode(head);
			arr[i] = NULL;
		}
	}
	
	free(str);

}











char* get_profile()
{


	FILE *fd;
	char line[100];
	static char var[100], dir[100];
	struct stat stat_var;
	static char check[100] = "/home/profile_custom";

	if (stat(check, &stat_var) == -1)
	{
		printf("Profile file could not be opened. Creating a new profile file called profile_custom in /home\n");
		fd = fopen("/home/profile_custom", "wb+");
		if (fd == NULL) {
			printf("failed to open file /home/profile_custom\n");
			//fclose(fd);
			return NULL;
		}
		else {
			fprintf(fd, "$HOME = /bin/");
			fclose(fd);

		}
	}
	printf("Profile file profile_custom loaded from /home\n");
	fd = fopen("/home/profile_custom", "r");
	if (fd == NULL) {
		printf("failed to open file /home/profile_custom\n");
		//fclose(fd);
		return NULL;
	}
	else {

		while (1)
		{
			if (fgets(line, 150, fd) == NULL) break;
			sscanf(line, "$%s = %s", var, dir);

		}
		fclose(fd);
		return dir;

	}
}

void ctrlC_Handler() {
	char choice;
	printf("\nDo you want to exit? Enter y/n \n");
	scanf("%c", &choice);
	{
		if (choice == 'Y' || choice == 'y') {
			exit(1);
		}
	}
}


void alias_add(char *cmd)
{
	struct stat stat_var;
	char *key, *val, *put, *t;
	char *key_1, *val_1, *put_1, *t_1;
	char line[100] = "";
	char check[100] = "/bin/";
	key_1 = malloc(150);
	key = malloc(150);
	val_1 = malloc(150);
	val = malloc(150);
	FILE *fd;

	key = strtok(cmd, "=");
	if(!key){printf("!! failed to execute alias. alias should be of the format alias ln=\"listname\"");}
	val = strtok(NULL, "\"");
	
	strcat(check, key);
	if (stat(check, &stat_var) == 0)
	{
		printf("Cannot assign %s to %s. %s command exists already\n", val, key, key);
		exit(EXIT_FAILURE);
	}

	fd = fopen("alias.txt", "r");

	if (fd == NULL) {
		
	    printf("!! failed to open alias.txt. Creating new alias file\n");
		//fclose(fd);
		fd = fopen("alias.txt", "w+");
		if(fd == NULL)
		{
			printf("!! failed to create alias.txt file\n");
		}
		else{
			fclose(fd);
			fd = fopen("alias.txt", "r");
		}
	}
	else {

		while (1) {
			if (fgets(line, 150, fd) == NULL) break;
			sscanf(line, "%s %s", key_1, val_1);

			if (!(strcmp(key_1, key)))
			{
				printf("Alias already exists. Please try another name\n");
				exit(EXIT_FAILURE);
			}
		}

		fclose(fd);
	}



	if (val)
	{
		fd = fopen("alias.txt", "a");
		if (fd == NULL) {

			//fclose(fd);
			printf(" !! failed to open file alias.txt\n");

		}
		else {

			fprintf(fd, "%s %s", key, val);
			fprintf(fd, "\r\n");
			printf("Alias added!\n");
			fclose(fd);

		}
	}

}

char* check_alias(char *cmd)
{
	char *key, *val;
	char line[100] = "";
	key = malloc(150);
	val = malloc(150);
	FILE *fd;
	fd = fopen("alias.txt", "r");


	if (fd == NULL) {

		printf("!! failed to open alias.txt \n");
		//fclose(fd);


	}
	else {

		while (1) {
			if (fgets(line, 150, fd) == NULL) break;
			sscanf(line, "%s %s", key, val);

			if (!(strcmp(cmd, key)))
			{

				return val;

			}

		}
		fclose(fd);
	}

	return NULL;
}


int cd_execute(char **args) {

	if (strcmp(args[0], "cd") == 0) {
		if (args[1] == NULL) {

			fprintf(stderr, " !! No arg to \"cd\"\n");
		}
		else {

			if (chdir(args[1]) != 0) {

				perror(" !! bad command");
			}
			else return 1;
		}

	}
	return 0;


}

int trigger_command(char **args)
{
	pid_t pid;
	int status;
	char *val;


	if (!strcasecmp(args[0], "exit"))
	{
		return 0;
	}


	pid = fork();
	if (pid == 0) {

		if (!(strcmp(args[0], "alias")))
		{
			alias_add(args[1]);
			return 1;
		}

		val = malloc(strlen(args[0]));
		stpcpy(val, args[0]);



		//status = cd_execute(args);
		if (status == 1) { return 1; }

		if (!(strcmp(args[0], "exit"))) {
			exit(0);
		}

		if (execvp(args[0], args) == -1)
		{
			args[0] = check_alias(args[0]);
			if (args[0])
			{
				alias_exe = malloc(150);
				strcpy(alias_exe, args[0]);
				return 5;
			}
			perror(val);
		}
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
	{
		perror(" !! fork failed ");
	}
	else
	{
		do {
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}


int exec_command(char **args)
{

	if (args[0] == NULL) {
		return 1;
	}


	int status = cd_execute(args);
	if (status == 1) return 1;

	return trigger_command(args);
}

void execute_pgm(void)
{
	char *line, **args, **combined, **paran;
	char cwd[1024];
	int status;
	int i = 0;
	int j = 0;

	do {

		i = 0;
		j = 0;

		if (getcwd(cwd, sizeof(cwd)) != NULL)
			printf("%s$", cwd);
		else
			printf("$");

		line = fetch_line();
		if(set_flag == 1)
		{
	pareval(line);
		}
		else
			
			{	
		paran = split_on_paranthese(line);
		if (paran[0] != NULL)
		{


			while (paran[j] != NULL)
			{
				i = 0;
				combined = split_on_comma(paran[j]);

				if (combined[0] != NULL)
				{

					while (combined[i] != NULL)
					{

						args = split_on_escapeCode(combined[i++]);
						status = exec_command(args);
					}
				}

				else {
					args = split_on_escapeCode(paran[j]);
					status = exec_command(args);
				}
				j++;
			}
		}

		else
		{

			combined = split_on_comma(line);

			if (combined[0] != NULL)
			{
				while (combined[i] != NULL)
				{
					args = split_on_escapeCode(combined[i++]);
					status = exec_command(args);
				}
			}
			else {
				args = split_on_escapeCode(line);
				status = exec_command(args);
			}
		}

		if (status == 5)
		{
			//printf("Executing again!!!!");

			args = split_on_escapeCode(alias_exe);
			status = exec_command(args);
		}

		free(line);
		free(args);
	}	
	} while (status);
	
}

char *fetch_line(void)
{
	int bufferLength = READ_BUF_ALLOC, index = 0;
	int char_in_int;
	char *buffer = malloc(sizeof(char) * bufferLength);


	if (!buffer)
	{
		fprintf(stderr, "Memory allocation failed!");
		exit(EXIT_FAILURE);
	}

	while (TRUE)
	{
		char_in_int = getchar();
		switch (char_in_int)
		{
		case EOF:
		case '\n': {

			buffer[index] = '\0';
			return buffer;
		}

		default: {
			if(char_in_int == "("){set_flag =1;}
			buffer[index++] = char_in_int;
			if (index >= bufferLength) {
				bufferLength += READ_BUF_ALLOC;
				buffer = realloc(buffer, bufferLength);
				if (!buffer) {
					fprintf(stderr, "Memory Allocation failed!\n");
					exit(EXIT_FAILURE);
				}
			}
		}
		}// END OF SWITCH
	}
}

char **split_on_paranthese(char *line)
{

	int stringArrayLength = TOKEN_SIZE, position = 0;
	char **stringArray = malloc(stringArrayLength * sizeof(char*));
	char *token, **stringArrayDup;

	if (!stringArray) {
		fprintf(stderr, " !! malloc failed(split_on_paranthese)\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, TOK_OPEN_PAREN);
	// if(token == NULL)
	// {
	// printf("\n%s\n",token);
	// token = strtok(NULL,TOK_OPEN_PAREN);
	//printf("\n%s\n",token);
	// }
	while (token != NULL) {

		stringArray[position + 1] = token;
		//printf("\n%s\n",stringArray[position+1]);
		token = strtok(NULL, ")");
		stringArray[position] = token;

		position = position + 2;
		token = strtok(NULL, ")");
		if (token != NULL) { stringArray[position++] = token; }

		if (position >= stringArrayLength) {
			stringArrayLength += TOKEN_SIZE;
			stringArrayDup = stringArray;
			stringArray = realloc(stringArray, stringArrayLength * sizeof(char*));
			if (!stringArray) {
				free(stringArrayDup);
				fprintf(stderr, " !! realloc failed(split_on_paranthese)\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, TOK_OPEN_PAREN);
	}
	stringArray[position] = NULL;
	return stringArray;
}


char **split_on_comma(char *line)
{

	int stringArrayLength = TOKEN_SIZE, position = 0;
	char **stringArray = malloc(stringArrayLength * sizeof(char*));
	char *token, **stringArrayDup;

	if (!stringArray) {
		fprintf(stderr, " !! malloc failed(split_on_comma)\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, TOKEN_COMMA);
	while (token != NULL) {
		stringArray[position] = token;

		position++;

		if (position >= stringArrayLength) {
			stringArrayLength += TOKEN_SIZE;
			stringArrayDup = stringArray;
			stringArray = realloc(stringArray, stringArrayLength * sizeof(char*));
			if (!stringArray) {
				free(stringArrayDup);
				fprintf(stderr, " !! realloc failed(split_on_comma)\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, TOKEN_COMMA);
	}
	stringArray[position] = NULL;

	return stringArray;
}


char **split_on_escapeCode(char *line)
{
	int stringArrayLength = TOKEN_SIZE, position = 0;
	char **stringArray = malloc(stringArrayLength * sizeof(char*));
	char *token, **stringArrayDup;

	if (!stringArray) {
		fprintf(stderr, " !! malloc failed(split_on_escapeCode)\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, TOK_ESCCODE_SPACE);
	while (token != NULL) {
		stringArray[position] = token;
		position++;

		if (position >= stringArrayLength) {
			stringArrayLength += TOKEN_SIZE;
			stringArrayDup = stringArray;
			stringArray = realloc(stringArray, stringArrayLength * sizeof(char*));
			if (!stringArray) {
				free(stringArrayDup);
				fprintf(stderr, " !! realloc failed(split_on_escapeCode) \n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, TOK_ESCCODE_SPACE);
	}
	stringArray[position] = NULL;
	return stringArray;
}


int main(int argc, char **argv)
{
	

	signal(SIGINT, ctrlC_Handler);


	char *directory;
	directory = get_profile();

	if (directory == NULL) {
		perror(" !! no directory error");

	}
	else if (chdir(directory) != 0)
	{
		perror(" !! no directory error");
	}
	FILE *fd;
	fd = fopen("alias.txt", "a+");
	if(fd == NULL)
	{
		printf("!! failed to create alias.txt file");
	}
	else
	{
		fclose(fd);
	}
	
	execute_pgm();


	return EXIT_SUCCESS;
}