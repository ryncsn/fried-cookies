#include<netinet/in.h>
#include<sys/wait.h>
#include<string.h>
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/unistd.h>
#include<unistd.h>
#include<sys/socket.h>
#include"common_struct.h"
#define MAXLINE                  4096
#define LISTENQ                  1024    /* 2nd argument to listen() */
#define SERV_PORT                9877
#define SA      struct sockaddr

static int sockfd;
void dg_echo(int, SA *, socklen_t);

	int 
main(int argc, char ** argv)
{
	struct sockaddr_in servaddr, cliaddr;

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0) {
		printf("socket error.\n");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	bind(sockfd, (SA *)&servaddr, sizeof(servaddr));

	dg_echo(sockfd, (SA *)&cliaddr, sizeof(cliaddr));
}


	void 
dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
	FILE *trfile;
	int pipefd[2];
	int n;
	socklen_t len;
	basic_block *txbuffer=malloc(sizeof_basic_block(MAXLINE+1));
	basic_block *rxbuffer=malloc(sizeof_basic_block(MAXLINE+1));
	for (;;) {
		len = clilen;
		n = recvfrom(sockfd, rxbuffer, sizeof_basic_block(MAXLINE), 0, pcliaddr, &len);
		printf("UDP Package Received(size:%d)\n",n);
		rxbuffer->content[basic_block_data_length(n)]=0;
		switch(rxbuffer->type){
			case CMD_LS:
				printf("Type:Listing\n");
				pipe(pipefd);
				int cpid;
				if((cpid=fork())==0){
					//Child
					close(pipefd[0]);
					dup2(pipefd[1],1);
					dup2(pipefd[1],2);
					close(pipefd[1]);
					execl("/bin/ls","ls",NULL);
					exit(0);//Chile exit
				}
				else{
					int tmp;
					waitpid(cpid,&tmp,0);
					if(tmp!=0||(tmp=read(pipefd[0],txbuffer->content,sizeof(char)*MAXLINE+1))==0){
						strcpy(txbuffer->content,"Empty output or something is wrong\n");
						tmp=35;
					}else{
						txbuffer->content[tmp]=0;
					}
					close(pipefd[0]);
					close(pipefd[1]);
					sendto(sockfd, txbuffer, sizeof_basic_block(tmp), 0, pcliaddr, len);
				}
				break;
			case CMD_UP:
				printf("Type:Upload\n");
				trfile=fopen(rxbuffer->content,"w");
				if(trfile==NULL){
					printf("Failed to create file");
					continue;
				}
				while(!0){
					n = recvfrom(sockfd, rxbuffer, sizeof_basic_block(MAXLINE), 0, pcliaddr, &len);
					rxbuffer->content[basic_block_data_length(n)]=0;
					if(rxbuffer->type==CMD_FINISH){
						fwrite(rxbuffer->content,sizeof(char),basic_block_data_length(n),trfile);
						break;
					}
					fwrite(rxbuffer->content,sizeof(char),basic_block_data_length(n),trfile);
				}
				fclose(trfile);
				break;
			case CMD_DOWN:
				printf("Type:Upload\n");
				trfile=fopen(rxbuffer->content,"r");
				if(trfile==NULL){
					printf("Failed to open file");
					continue;
				}
				txbuffer->type=DATA_BLOCK;
				while(!0){
					int readsize;
					if(feof(trfile))
						break;
					readsize=fread(txbuffer->content,sizeof(char),DEFLINE,trfile);
					sendto(sockfd, txbuffer, sizeof_basic_block(readsize), 0, pcliaddr, len);
				}
				txbuffer->type=CMD_FINISH;
				sendto(sockfd, txbuffer, sizeof_basic_block(0), 0, pcliaddr, len);
				break;
			case MSG_BLOCK:
				printf("Type:MSG\n");
				printf("%s\n",rxbuffer->content);
				break;
			case CMD_CHDIR:
				printf("Type:CD\n");
				chdir(rxbuffer->content);
				break;
			default:
				printf("Type:Unknown\n");
				break;
		}
	}
	free(rxbuffer);
	free(txbuffer);
}
