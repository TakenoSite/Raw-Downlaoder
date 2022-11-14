#include<stdio.h>
#include<string.h>    
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<netdb.h>

#define DOWNLOAD_BUFSIZE 1024   
#define HEADER_BUFSIZE 1024
#define IPV4_BUFSIZE 32
#define URI_BUFSIZE 64
#define PATH_BUFSIZE 64

int socket_d;
char server_reply[DOWNLOAD_BUFSIZE];
FILE *file;
struct sockaddr_in server;
int count = 0;

int byte_count = 0;
char HeaderBuf[HEADER_BUFSIZE];
int Content_len;
char *sHeader;

char *URL_BUF;
char *path[PATH_BUFSIZE];
char request_path[URI_BUFSIZE];
char *request_header;
char ipv4[IPV4_BUFSIZE];


static int getHost(char *url, in_port_t port){
	struct hostent *hostent;
	
	if((hostent = gethostbyname(url)) == NULL){
		sprintf(ipv4, "%s", "255.255.255.255");
		return 1;
	};
	
	sprintf(ipv4, "%u.%u.%u.%u", 
		   (unsigned char)hostent->h_addr_list[0][0],
		   (unsigned char)hostent->h_addr_list[0][1],
		   (unsigned char)hostent->h_addr_list[0][2],
		   (unsigned char)hostent->h_addr_list[0][3]);
	
	return 0;
}

static void HeaderSplit(char* src){
	
	sHeader = (char*)malloc(strlen(src)*sizeof(src)+3);
	sprintf(sHeader,"%s: ",src);
	char *ptr = strtok(HeaderBuf, "\r\n");
	puts(ptr);

	while(ptr != NULL){
		ptr = strtok(NULL ,"\r\n");
		if(ptr != NULL && strstr(ptr, sHeader) != NULL){
			sscanf(strtok(ptr,sHeader), "%d", &Content_len);
		}
	}
	
	free(sHeader);
}

static int UrlSplit(int port){
	
	char *url_ptr = strtok(URL_BUF, "/");
	int url_count = 0;
	char *edit;
	
	if(strcmp(url_ptr, "http:") == 0 || strcmp(url_ptr, "https:") == 0){
		puts("no http and https please ");
		return 1;
	}

	getHost(url_ptr, port);

	while(url_ptr != NULL){
		url_ptr = strtok(NULL, "/");
		
		if(url_ptr != NULL){
			path[url_count] = url_ptr;	
			url_count ++;
		}
	}
	
	for(int i=0; i<10; i++){ // patch dept
		if(path[i] !=NULL){
			edit = (char*)malloc(strlen(path[i])*sizeof(path[i]));
			sprintf(edit, "/%s", path[i]);
			strcat(request_path, edit);
			free(edit);
		}else{
			break;
		}
	}
	
	request_header = (char*)malloc(strlen(request_path)*sizeof(request_path));
	sprintf(request_header,"GET %s HTTP/1.1 \r\n\r\n", request_path); // HttpHeader
	bzero(request_path, sizeof(request_path));
	puts(request_header);
	
	return 0;
}

int downloader(char *url,int port, char* filename)
{
	char *Header_to_Data;
	long int diff = 0;

	URL_BUF = (char*)malloc(strlen(url)*sizeof(url));
	strcpy(URL_BUF, url);
	if(UrlSplit(port)){
		return 1;
	};
	free(URL_BUF);
	
    socket_d = socket(AF_INET , SOCK_STREAM , 0);
    server.sin_addr.s_addr = inet_addr(ipv4);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if(connect(socket_d , (struct sockaddr *)&server , sizeof(server)) < 0){
	    	return 1;
		puts("can not conection");
	}	

    if(send(socket_d , request_header , strlen(request_header) , 0) < 0){
	    	return 1;
		puts("send error");
	};
    
	
    file = fopen(filename, "ab");
	
	while(1){
		int responsed = recv(socket_d,server_reply,sizeof server_reply,0);	
		if(count < 2){
			if(sizeof(HeaderBuf) > strlen(server_reply)){
				strcpy(HeaderBuf, server_reply);	
			}else{
				puts("header buffor over");
				return 2;
			}
			
			HeaderSplit("Content-Length");  
			Header_to_Data = strstr(server_reply, "\r\n\r\n");
			diff =&Header_to_Data[4] - server_reply;
			if(Header_to_Data != NULL){
				byte_count += responsed - diff; 
				fwrite(&Header_to_Data[4], responsed - diff, 1, file);
			}
		}

		if(count > 1){
			byte_count += responsed;
			fwrite(server_reply,responsed,1,file);
		}
		
		if(responsed < 1){
			if(Content_len == byte_count){
				printf("contetn_len : %d\nwriteByte : %d\nCompleted\n",\
						Content_len, byte_count);
				
				free(request_header);
				close(socket_d);
				fclose(file);
				
				count      = 0;
				byte_count = 0;

				return 1;
		
			}else{
				printf("content_len : %d\nwriteByte : %d\nlossByte : %d\nRestart .. \n",\
						Content_len,byte_count, Content_len - byte_count);	
				
				free(request_header);
				close(socket_d);
				fclose(file);

				count      = 0;
				byte_count = 0;
				
				return 0;	
			}
		};
		count ++;
	}
}


int Download(char* url, int port, char* filename){

	int BadCount = 0;
	int downloader_code;
	while(1){
		downloader_code = downloader(url, port, filename);
		if(downloader_code == 1){
			return 0;
			break;
		}else if(downloader_code == 2){
			return 2;
		}
		
		if(BadCount > 10){
			return 1;
			puts("Dis Conection");
		}
		BadCount ++;
	}

	return 0;
}


