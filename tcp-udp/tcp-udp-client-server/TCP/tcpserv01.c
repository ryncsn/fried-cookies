#include <netinet/in.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "common_struct.h"
#include <unistd.h>
#include <sys/unistd.h>

#define MAXLINE                  4096
#define LISTENQ                  1024
#define SERV_PORT                9877
#define SA      struct sockaddr

void str_echo(int);
ssize_t readBlock(int, void *, size_t);
static ssize_t my_read(int, char *);

int main(int argc, char **argv) {
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr,servaddr;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket error.\n");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	if (bind(listenfd, (SA *)&servaddr, sizeof(servaddr)) < 0) {
		printf("bind error.\n");
		exit(1);
	}

	if (listen(listenfd,LISTENQ) < 0) {
		printf("listen error.\n");
		exit(1);
	}

	FILE *trfile;
	int pipefd[2];
	int n;
	socklen_t len;
	basic_block *txbuffer = malloc(sizeof_basic_block(MAXLINE+1));
	basic_block *rxbuffer = malloc(sizeof_basic_block(MAXLINE+1));
	for(;;) {
		printf("Listen Begin\n");
		clilen = sizeof(cliaddr);
		if ( (connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0) {
			printf("accept error.\n");
			exit(1);
		}
		printf("TCP NEW Head Received");
		if((childpid = fork()) == 0) {
			close(listenfd);
			while(!0){
				n = readBlock(connfd, rxbuffer, block_head);
				if(n==0){
					return 0;
				}
				n = readBlock(connfd, rxbuffer->content, rxbuffer->size);
				rxbuffer->content[n]=0;
				if(n<0){
					return -1;
				}
				printf("New Block: size:%d\n",n);
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
							printf("CEND\n");
							if(tmp!=0||(tmp=read(pipefd[0],txbuffer->content,sizeof(char)*MAXLINE+1))==0){
								strcpy(txbuffer->content,"Empty output or something is wrong\n");
								txbuffer->size=35;
								tmp=35;
							}else{
								txbuffer->content[tmp]=0;
							}
							close(pipefd[0]);
							close(pipefd[1]);
							txbuffer->size=strlen(txbuffer->content);
							printf("Writing\n");
							write(connfd, txbuffer, sizeof_basic_block(tmp));
							printf("Writing Finish\n");
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
							n = readBlock(connfd, rxbuffer, block_head);
							n = readBlock(connfd, rxbuffer->content, rxbuffer->size);
							if(rxbuffer->type==CMD_FINISH){
								fwrite(rxbuffer->content,sizeof(char),n,trfile);
								break;
							}
							fwrite(rxbuffer->content,sizeof(char),n,trfile);
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
							txbuffer->size=readsize;
							write(connfd, txbuffer, sizeof_basic_block(readsize));
						}
						txbuffer->type=CMD_FINISH;
						txbuffer->size=0;
						write(connfd, txbuffer, sizeof_basic_block(0));
						break;
					case MSG_BLOCK:
						printf("Type:MSG\n");
						printf("%s\n",rxbuffer->content);
						break;
					case 0:
						break;
					default:
						printf("Type:Unknown\n");
						break;
				}
			}
			exit(0);
		}
		close(connfd);
	}
	free(rxbuffer);
	free(txbuffer);

}

ssize_t readBlock(int fd, void *vptr, size_t maxlen) {
	int n, rc;
	char c, *ptr;

	ptr = vptr;
	for (n = 0; n < maxlen; n++) {
		if ( (rc = my_read(fd, &c)) == 1) {
			*ptr++ = c;
		} else if (rc == 0) {
			if (n == 0)
				return(-1); /* EOF, no data read */
			else
				break; /* EOF, some data was read */
		} else
			return(-2); /* error, errno set by read() */
	}
	return(n);
}
/* end readline */

static ssize_t my_read(int fd, char *ptr) {
	static int      read_cnt = 0;
	static char     *read_ptr;
	static char     read_buf[MAXLINE];

	if (read_cnt <= 0) {
again:
		if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
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
