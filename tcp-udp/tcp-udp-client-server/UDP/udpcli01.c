#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "common_struct.h"
#define SA struct sockaddr

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
	else if(strcmp(src,"cd")==0){
		return CMD_CHDIR;
	}
	else if(strcmp(src,"")==0){
		return 0;
	}
	return -1;
}

void dg_cli(int sockfd, const SA *pservaddr, socklen_t servlen)
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
				sendto(sockfd, txbuffer, sizeof_basic_block(0), 0, pservaddr, servlen);
				n = recvfrom(sockfd, rxbuffer, sizeof_basic_block(MAXLINE), 0, NULL, NULL);
				rxbuffer->content[basic_block_data_length(n)]=0; /* null terminate */
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
				sendto(sockfd, txbuffer, sizeof_basic_block(strlen(txbuffer->content)), 0, pservaddr, servlen);
				txbuffer->type=DATA_BLOCK;
				while(!0){
					int readsize;
					if(feof(trfile))
						break;
					readsize=fread(txbuffer->content,sizeof(char),DEFLINE,trfile);
					sendto(sockfd, txbuffer, sizeof_basic_block(readsize), 0, pservaddr, servlen);
				}
				txbuffer->type=CMD_FINISH;
				sendto(sockfd, txbuffer, sizeof_basic_block(0), 0, pservaddr, servlen);
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
				sendto(sockfd, txbuffer, sizeof_basic_block(strlen(txbuffer->content)), 0, pservaddr, servlen);
				while(!0){
					n = recvfrom(sockfd, rxbuffer, sizeof_basic_block(MAXLINE), 0, NULL, NULL);
					rxbuffer->content[basic_block_data_length(n)]=0;
					if(rxbuffer->type==CMD_FINISH){
						fwrite(rxbuffer->content,sizeof(char),basic_block_data_length(n),trfile);
						break;
					}
					fwrite(rxbuffer->content,sizeof(char),basic_block_data_length(n),trfile);
				}
				fclose(trfile);
				break;
			case MSGING:
				printf("Enter Messege to send\n");
				//fgets(txbuffer->content, MAXLINE,stdin);
				scanf("%s",txbuffer->content);
				txbuffer->type=MSG_BLOCK;
				sendto(sockfd, txbuffer, sizeof_basic_block(strlen(txbuffer->content)), 0, pservaddr, servlen);
				//fputs(rxbuffer->content, stdout);
				break;
			case CMD_CHDIR:
				//fgets(txbuffer->content, MAXLINE,stdin);
				scanf("%s",txbuffer->content);
				txbuffer->type=CMD_CHDIR;
				sendto(sockfd, txbuffer, sizeof_basic_block(strlen(txbuffer->content)), 0, pservaddr, servlen);
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


int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;

	if (argc != 2) {
		printf("usage:%s <IPaddress>\n",argv[0]);
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);

	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0) {
		printf("socket error.\n");
		exit(1);
	}

	dg_cli(sockfd, (SA *)&servaddr, sizeof(servaddr));

	exit(0);
}


