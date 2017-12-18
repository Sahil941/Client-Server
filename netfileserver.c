/* ASST2
   Authors: Sahil Kumbhani, Sandeep Behera */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <errno.h>

struct fileInfo{
	char nameOfFile[1024];
	int writeFlag;
	int unrestrictedFlag;
	int exclusiveFlag;
	int transactionFlag;
	int fileOpen;
	int writeCounter;
	int unrestrictedCounter;
	int exclusiveCounter;
	int transactionCounter;
	int fileCounter;
}fileInfoArr[10];
int counter = 0;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void *socketHandler(void *inConnection){
	int sockfd = *((int *)inConnection);
	int clientfd;
	char *partOfMsg, posOfMsg;
	int fd;

	char buffer[256];
	char retBuffer[1024];
	bzero(buffer, 256);
	clientfd = read(sockfd, buffer, 255);
	partOfMsg = strtok_r(buffer, " ", &posOfMsg);

	if (strcmp(partOfMsg, "OPEN") == 0){
		char *mode = strtok_r(NULL, " ", &posOfMsg);
		char *fileName = strtok_r(NULL, " ", &posOfMsg);
		char *flag = strtok_r(NULL, " ", &posOfMsg);
		int index = 0;

		if (counter == 0){
			pthread_mutex_lock(&m);
			strcpy(fileInfoArr[counter].nameOfFile, fileName);
			pthread_mutex_unlock(&m);
			fileInfoArr[counter].writeFlag = 0;
			fileInfoArr[counter].unrestrictedFlag = 0;
			fileInfoArr[counter].exclusiveFlag = 0;
			fileInfoArr[counter].transactionFlag = 0;
			fileInfoArr[counter].fileOpen = 0;
			fileInfoArr[counter].writeCounter = 0;
			fileInfoArr[counter].unrestrictedCounter = 0;
			fileInfoArr[counter].exclusiveCounter = 0;
			fileInfoArr[counter].transactionCounter = 0;
			fileInfoArr[counter].fileCounter = 0;
			index = counter;
			counter = 1;
		}
		else if (counter > 0){
			int i = 0;
			for (i = 0; i < counter; i++){
				if (strcmp(fileName, fileInfoArr[i].nameOfFile) == 0){
					index = i;
					break;
				}
			}

			if (i == counter){
				pthread_mutex_lock(&m);
				strcpy(fileInfoArr[counter].nameOfFile, fileName);
				pthread_mutex_unlock(&m);
				fileInfoArr[counter].writeFlag = 0;
				fileInfoArr[counter].unrestrictedFlag = 0;
				fileInfoArr[counter].exclusiveFlag = 0;
				fileInfoArr[counter].transactionFlag = 0;
				fileInfoArr[counter].fileOpen = 0;
				fileInfoArr[counter].writeCounter = 0;
				fileInfoArr[counter].unrestrictedCounter = 0;
				fileInfoArr[counter].exclusiveCounter = 0;
				fileInfoArr[counter].transactionCounter = 0;
				fileInfoArr[counter].fileCounter = 0;
				index = counter;
				counter++;
			}
		}

		if (strcmp(mode, "unrestricted") == 0){
			if (strcmp(flag, "r") == 0){
				if (fileInfoArr[index].transactionFlag == 1){
					errno = EACCES;
					sprintf(retBuffer, "%s", strerror(errno));
					clientfd = write(sockfd, retBuffer, sizeof(retBuffer));
					close(sockfd);
				}
				else{
					fileInfoArr[index].unrestrictedFlag = 1;
					fileInfoArr[index].unrestrictedCounter++;
					FILE *fp = fopen(fileName, flag);
					if (fp != NULL){
						fileInfoArr[index].fileOpen = 1;
						fileInfoArr[index].fileCounter++;
						fd = fileno(fp);
					}

					sprintf(retBuffer, "%d", fd);
					clientfd = write(sockfd, retBuffer, 5);
					close(sockfd);
				}
			}
			else if ((strcmp(flag, "w") == 0) || (strcmp(flag, "a+") == 0)){
				if (fileInfoArr[index].transactionFlag == 1){
					errno = EACCES;
					sprintf(retBuffer, "%s", strerror(errno));
					clientfd = write(sockfd, retBuffer, sizeof(retBuffer));
					close(sockfd);
				}
				else if (fileInfoArr[index].exclusiveFlag == 1){
					if (fileInfoArr[index].writeFlag == 1){
						errno = EACCES;
						sprintf(retBuffer, "%s", strerror(errno));
						clientfd = write(sockfd, retBuffer, sizeof(retBuffer));
						close(sockfd);
					}
					else{
						fileInfoArr[index].unrestrictedFlag = 1;
						fileInfoArr[index].unrestrictedCounter++;
						fileInfoArr[index].writeFlag = 1;
						fileInfoArr[index].writeCounter++;
						FILE *fp = fopen(fileName, flag);
						if (fp != NULL){
							fileInfoArr[index].fileOpen = 1;
							fileInfoArr[index].fileCounter++;
							fd = fileno(fp);
						}

						sprintf(retBuffer, "%d", fd);
						clientfd = write(sockfd, retBuffer, 5);
						close(sockfd);
					}
				}
				else{
					fileInfoArr[index].unrestrictedFlag = 1;
					fileInfoArr[index].unrestrictedCounter++;
					fileInfoArr[index].writeFlag = 1;
					fileInfoArr[index].writeCounter++;
					FILE *fp = fopen(fileName, flag);
					if (fp != NULL){
						fileInfoArr[index].fileOpen = 1;
						fileInfoArr[index].fileCounter++;
						fd = fileno(fp);
					}

					sprintf(retBuffer, "%d", fd);
					clientfd = write(sockfd, retBuffer, 5);
					close(sockfd);
				}
			}
			else{
				sprintf(retBuffer, "Improper file flag sent.");
				clientfd = write(sockfd, retBuffer, sizeof(retBuffer));
				close(sockfd);
			}
		}
		else if (strcmp(mode, "exclusive") == 0){
			if (strcmp(flag, "r") == 0){
				if (fileInfoArr[index].transactionFlag == 1){
					errno = EACCES;
					sprintf(retBuffer, "%s", strerror(errno));
					clientfd = write(sockfd, retBuffer, sizeof(retBuffer));
					close(sockfd);
				}
				else{
					fileInfoArr[index].exclusiveFlag = 1;
					fileInfoArr[index].exclusiveCounter++;
					FILE *fp = fopen(fileName, flag);
					if (fp != NULL){
						fileInfoArr[index].fileOpen = 1;
						fileInfoArr[index].fileCounter++;
						fd = fileno(fp);
					}

					sprintf(retBuffer, "%d", fd);
					clientfd = write(sockfd, retBuffer, 5);
					close(sockfd);
				}
			}
			else if ((strcmp(flag, "w") == 0) || (strcmp(flag, "a+") == 0)){
				if (fileInfoArr[index].transactionFlag == 1){
					errno = EACCES;
					sprintf(retBuffer, "%s", strerror(errno));
					clientfd = write(sockfd, retBuffer, sizeof(retBuffer));
					close(sockfd);
				}
				else if (fileInfoArr[index].writeFlag == 1){
					errno = EACCES;
					sprintf(retBuffer, "%s", strerror(errno));
					clientfd = write(sockfd, retBuffer, sizeof(retBuffer));
					close(sockfd);
				}else{
					fileInfoArr[index].exclusiveFlag = 1;
					fileInfoArr[index].exclusiveCounter++;
					fileInfoArr[index].writeFlag = 1;
					fileInfoArr[index].writeCounter++;
					FILE *fp = fopen(fileName, flag);
					if (fp != NULL){
						fileInfoArr[index].fileOpen = 1;
						fileInfoArr[index].fileCounter++;
						fd = fileno(fp);
					}

					sprintf(retBuffer, "%d", fd);
					clientfd = write(sockfd, retBuffer, 5);
					close(sockfd);
				}
			}
			else{
				sprintf(retBuffer, "Improper file flag sent.");
				clientfd = write(sockfd, retBuffer, sizeof(retBuffer));
				close(sockfd);
			}
		}
		else if (strcmp(mode, "transaction") == 0){
			if (fileInfoArr[index].fileOpen == 1){
				errno = EACCES;
				sprintf(retBuffer, "%s", strerror(errno));
				clientfd = write(sockfd, retBuffer, sizeof(retBuffer));
				close(sockfd);
			}
			else{
				if ((strcmp(flag, "r") == 0) || (strcmp(flag, "w") == 0) || (strcmp(flag, "a+") == 0)){
					fileInfoArr[index].transactionFlag = 1;
					fileInfoArr[index].transactionCounter++;
					if (strcmp(flag, "r") != 0){
						fileInfoArr[index].writeFlag = 1;
						fileInfoArr[index].writeCounter++;
					}
					FILE *fp = fopen(fileName, flag);
					if (fp != NULL){
						fileInfoArr[index].fileOpen = 1;
						fileInfoArr[index].fileCounter++;
						fd = fileno(fp);
					}

					sprintf(retBuffer, "%d", fd);
					clientfd = write(sockfd, retBuffer, 5);
					close(sockfd);
				}
				else{
					sprintf(retBuffer, "Improper file flag sent.");
					clientfd = write(sockfd, retBuffer, sizeof(retBuffer));
					close(sockfd);
				}
			}
		}

		return NULL;
	}
	else if (strcmp(partOfMsg, "CLOSE") == 0){
		char *filedes = strtok_r(NULL, " ", &posOfMsg);
		fd = atoi(filedes);
		char *mode = strtok_r(NULL, " ", &posOfMsg);
		char *fileName = strtok_r(NULL, " ", &posOfMsg);
		char *flag = strtok_r(NULL, " ", &posOfMsg);

		int index = 0;
		int i = 0;
		for (i = 0; i < counter; i++){
			printf("%s\n", fileInfoArr[i].nameOfFile);
			if (strcmp(fileName, fileInfoArr[i].nameOfFile) == 0){
				index = i;
				break;
			}
		}

		if (i == counter){
			printf("This file has not been created/opened.\n");
		}
		else{
			counter--;
			fileInfoArr[index].fileCounter--;
			if (fileInfoArr[index].fileCounter == 0){
				fileInfoArr[index].fileOpen = 0;
			}
			bzero(fileInfoArr[index].nameOfFile, sizeof(fileInfoArr[index].nameOfFile));

			if (strcmp(mode, "unrestricted") == 0){
				fileInfoArr[index].unrestrictedCounter--;
				if (fileInfoArr[index].unrestrictedCounter == 0){
					fileInfoArr[index].unrestrictedFlag = 0;
				}

				if ((strcmp(flag, "w") == 0) || (strcmp(flag, "a+") == 0)){
					fileInfoArr[index].writeCounter--;
					if (fileInfoArr[index].writeCounter == 0){
						fileInfoArr[index].writeFlag = 0;
					}
				}
			}
			else if (strcmp(mode, "exclusive") == 0){
				fileInfoArr[index].exclusiveCounter--;
				if (fileInfoArr[index].exclusiveCounter == 0){
					fileInfoArr[index].exclusiveFlag = 0;
				}

				if ((strcmp(flag, "w") == 0) || (strcmp(flag, "a+") == 0)){
					fileInfoArr[index].writeCounter--;
					if (fileInfoArr[index].writeCounter == 0){
						fileInfoArr[index].writeFlag = 0;
					}
				}
			}
			else if (strcmp(mode, "transaction") == 0){
				fileInfoArr[index].transactionCounter--;
				if (fileInfoArr[index].transactionCounter == 0){
					fileInfoArr[index].transactionFlag = 0;
				}

				if ((strcmp(flag, "w") == 0) || (strcmp(flag, "a+") == 0)){
					fileInfoArr[index].writeCounter--;
					if (fileInfoArr[index].writeCounter == 0){
						fileInfoArr[index].writeFlag = 0;
					}
				}
			}
			else{
				printf("Improper file mode sent.\n");
			}
		}

		int err = close(fd);
		if (err == 0){
			sprintf(retBuffer, "%d", err);
			clientfd = write(sockfd, retBuffer, 5);
		}
		else{
			errno = EBADF;
			sprintf(retBuffer, "%d", err);
			clientfd = write(sockfd, retBuffer, 5);
			sprintf(retBuffer, "%s", strerror(errno));
			clientfd = write(sockfd, retBuffer, 30);
		}

		close(sockfd);
		return NULL;
	}
	else if (strcmp(partOfMsg, "READ") == 0){
		fd = atoi(strtok_r(NULL, " ", &posOfMsg));
		char *readBuffer = strtok_r(NULL, " ", &posOfMsg);
		char *nBytes = strtok_r(NULL, " ", &posOfMsg);
		int num = atoi(nBytes);
		
		/* FILE *fp = fdopen(fd, "a+");
		fseek(fp, SEEK_SET, 0);
		fread(retBuffer, (num + 1), 1, fp); */
		int numBytes = read(fd, retBuffer, num);
		
		/* sprintf(retBuffer, "%s", retBuffer);
		clientfd = write(sockfd, retBuffer, numbytes); */
		
		if (numBytes < 0){
			errno = EBADF;
			sprintf(retBuffer, "%d", numBytes);
			clientfd = write(sockfd, retBuffer, 5);
			sprintf(retBuffer, "%s", strerror(errno));
			clientfd = write(sockfd, retBuffer, 30);
		}
		else{
			sprintf(retBuffer, "%d", numBytes);
			clientfd = write(sockfd, retBuffer, 5);
		}
		return NULL;
	}
	else if (strcmp(partOfMsg, "WRITE") == 0){
		fd = atoi(strtok_r(NULL, " ", &posOfMsg));
		char *nBytes = strtok_r(NULL, " ", &posOfMsg);
		char *writeThis = strtok_r(NULL, " ", &posOfMsg);
		int num = atoi(nBytes);
		
		fd = write(fd, writeThis, num);
		if (fd < 0){
			errno = EBADF;
			sprintf(retBuffer, "%d", fd);
			clientfd = write(sockfd, retBuffer, 5);
			sprintf(retBuffer, "%s", strerror(errno));
			clientfd = write(sockfd, retBuffer, 30);
		}
		else{
			sprintf(retBuffer, "%d", fd);
			clientfd = write(sockfd, retBuffer, 10);
		}

		return NULL;
	}
	else{
		int fd = atoi(strtok_r(NULL, " ", &posOfMsg));
		printf("%d\n", fd);
		struct stat st;
		fstat(fd, &st);
		int size = (int)st.st_size;
		printf("%d\n", size);
		
		/* FILE *f = fdopen(fd, "r");
		fseek(f, 0L, SEEK_END);
		long int size = ftell(f);
		rewind(f); */
		char buff[200];
		sprintf(buff, "%d", size);
		clientfd = write(sockfd, buff, strlen(buff));
		close(fd);
		close(clientfd);
	}
}

/* int fileSize(int fd){
	struct stat st;
	fstat(fd, &st);
	int size = (int)st.st_size;
	//printf("%d\n", size);
	return size;
} */

int main(int argc, char** argv){
	int sockfd, inConnection, portNum;
	int clientLen;
	struct sockaddr_in servAddr, clientAddr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		perror("ERROR GETTING SOCKET");
	}

	memset((char*)&servAddr, 0, sizeof(servAddr));
	portNum = atoi(argv[1]);
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = INADDR_ANY;
	servAddr.sin_port = htons(portNum);
	int enable = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

	if (bind(sockfd, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0){
		perror("ERROR BINDING TO PORT");
	}
	listen(sockfd, 10);
	
	pthread_t tid;
	clientLen = sizeof(clientAddr);
	while (inConnection = accept(sockfd, ((struct sockaddr *) &clientAddr), &clientLen)){
		if (pthread_create(&tid, NULL, socketHandler, (void *) &inConnection) < 0){
			perror("ERROR CREATING THREAD");
			return -1;
		}
	}
	
	if (inConnection < 0){
		perror("ERROR ACCEPTING CONNECTION");
	}
	
	return 0;
}