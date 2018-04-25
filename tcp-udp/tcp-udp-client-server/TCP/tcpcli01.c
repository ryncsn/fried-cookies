#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "common_struct.h"
#define SA struct sockaddr

#define MAXLINE                  4096
#define LISTENQ                  1024
#define SERV_PORT                9877
#define SA      struct sockaddr

void str_cli(FILE *,int);
ssize_t readBlock(int, void *, size_t);
static ssize_t my_read(int, char *);

int command_trans(char* src){
	if(strcmp(src,"ls")==0){
		return LIST_FILE;
	}
	else if(strcmp(src,"upload")==0){
		return UPLOAD;
	}
	else if(strcmp(src,"download")==0){
		return DOWNLOAD;
	}
	else if(strcmp(src,"exit")==0){
		return EXITING;
	}
	else if(strcmp(src,"msg")==0){
		return MSGING;
	}
	else if(strcmp(src,"")==0){
		return 0;
	}
	return -1;
}


	int
main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in  servaddr;

	if (argc != 2) {
		printf("usage:tcpcli01 <IPaddress>");
		exit(1);
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket error.\n");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0) {
		printf("connect error.\n");
		exit(1);
	}

	str_cli(stdin, sockfd);
	exit(0);
}

void str_cli(FILE *fp,int sockfd)
{
	FILE *trfile;
	int  n;
	char readbuf[MAXLINE+1];
	basic_block *txbuffer=malloc(sizeof_basic_block(MAXLINE+1));
	basic_block *rxbuffer=malloc(sizeof_basic_block(MAXLINE+1));

	while (!0){
		char buffer[1024];
		scanf("%s",readbuf);
		//fgets(readbuf, MAXLINE,stdin);
		switch(command_trans(readbuf)){
			case LIST_FILE:
				txbuffer->type=CMD_LS;
				txbuffer->size=0;
				write(sockfd, txbuffer, sizeof_basic_block(0));
				printf("lsing sent\n");
				n = readBlock(sockfd, rxbuffer, block_head);
				printf("rcv head %d\n",n);
				n = readBlock(sockfd, rxbuffer->content, rxbuffer->size);
				printf("rcv content %d\n",n);
				rxbuffer->content[n]=0; /* null terminate */
				printf("%s\n",rxbuffer->content);
				//fputs(rxbuffer->content, stdout);
				break;
			case UPLOAD:
				txbuffer->type=CMD_UP;
				printf("Enter File to upload:\n");
				scanf("%s",buffer);
				trfile=fopen(buffer,"r");
				if(trfile==NULL){
					printf("Failed to open file");
					continue;
				}
				printf("Enter uploaded filename:\n");
				scanf("%s",txbuffer->content);
				txbuffer->size=strlen(txbuffer->content);
				write(sockfd, txbuffer, sizeof_basic_block(strlen(txbuffer->content)));
				txbuffer->type=DATA_BLOCK;
				while(!0){
					int readsize;
					if(feof(trfile))
						break;
					readsize=fread(txbuffer->content,sizeof(char),DEFLINE,trfile);
					txbuffer->size=readsize;
					write(sockfd, txbuffer, sizeof_basic_block(readsize));

				}
				txbuffer->type=CMD_FINISH;
				txbuffer->size=0;
				write(sockfd, txbuffer, sizeof_basic_block(0));
				break;
			case DOWNLOAD:
				txbuffer->type=CMD_DOWN;
				printf("Enter Filename to download:\n");
				scanf("%s",txbuffer->content);
				printf("Enter File to save:\n");
				scanf("%s",buffer);
				trfile=fopen(buffer,"w");
				if(trfile==NULL){
					printf("Failed to create file");
					continue;
				}
				txbuffer->size=strlen(txbuffer->content);
				write(sockfd, txbuffer, sizeof_basic_block(strlen(txbuffer->content)));
				while(!0){
					n = readBlock(sockfd, rxbuffer, block_head);
					n = readBlock(sockfd, rxbuffer->content, rxbuffer->size);
					rxbuffer->content[n]=0;
					if(rxbuffer->type==CMD_FINISH){
						fwrite(rxbuffer->content,sizeof(char),n,trfile);
						break;
					}
					fwrite(rxbuffer->content,sizeof(char),n,trfile);
				}
				fclose(trfile);
				break;
			case MSGING:
				printf("Enter Messege to send\n");
				//fgets(txbuffer->content, MAXLINE,stdin);
				scanf("%s",txbuffer->content);
				txbuffer->type=MSG_BLOCK;
				txbuffer->size=strlen(txbuffer->content);
				write(sockfd, txbuffer, sizeof_basic_block(strlen(txbuffer->content)));
				//fputs(rxbuffer->content, stdout);
				break;
			case EXITING:
				free(txbuffer);
				free(rxbuffer);
				exit(0);
				break;
		}
	}
	free(txbuffer);
	free(rxbuffer);
}
/*end str_cli */

ssize_t readBlock(int fd, void *vptr, size_t maxlen) {
	int n, rc;
	char c, *ptr;

	ptr = vptr;
	for (n = 0; n < maxlen; n++) {
		if ( (rc = my_read(fd, &c)) == 1) {
			*ptr++ = c;
		} else if (rc == 0) {
			if (n == 0)
				return(0); /* EOF, no data read */
			else
				break; /* EOF, some data was read */
		} else
			return(-1); /* error, errno set by read() */
	}
	return(n);
}
/* end readline */

static ssize_t my_read(int fd, char *ptr) {
	static int read_cnt = 0;
	static char *read_ptr;
	static char read_buf[MAXLINE];

	if (read_cnt <= 0) {
again:
		if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
			if (errno == EINTR)
				goto again;
			return(-1);
		} else if (read_cnt == 0)
			return(0);
		read_ptr = read_buf;
	}

	read_cnt--;
	*ptr = *read_ptr++;
	return(1);
}
/*end ssize_t*/
