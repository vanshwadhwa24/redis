#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

		

void *handle_client(void *arg){
	int client_fd = *(int *)arg;
	free(arg);
	const char *response="";
	char buffer[1024];
	while(1){
		ssize_t bytes_read = read(client_fd,buffer,sizeof(buffer));
		if(bytes_read==0){
			break;
		}else if(bytes_read==-1){
			fprintf(stderr,"error reading from client");
			break;
		}
			

		 
		 char *response_array[20];
int idx = 0;
int i = 0;

while (buffer[i] != '\0') {

    
    if (buffer[i] == '*') {

        i++;    

       
        int num_of_args = 0;
        while (buffer[i] != '\r') {
            num_of_args = num_of_args * 10 + (buffer[i] - '0');
            i++;
        }

        i += 2;      

        while (num_of_args--) {

          
            if (buffer[i] != '$')
                break;

            i++;

            int len = 0;
            while (buffer[i] != '\r') {
                len = len * 10 + (buffer[i] - '0');
                i++;
            }

            i += 2;     

            char *s = malloc(len + 1);

            for (int j = 0; j < len; j++)
                s[j] = buffer[i++];

            s[len] = '\0';

            response_array[idx++] = s;

            i += 2;    
        }

        break;
    }

   
    else if (buffer[i] == '$') {

        i++;

        int len = 0;
        while (buffer[i] != '\r') {
            len = len * 10 + (buffer[i] - '0');
            i++;
        }

        i += 2;

        char *s = malloc(len + 1);

        for (int j = 0; j < len; j++)
            s[j] = buffer[i++];

        s[len] = '\0';

        response_array[idx++] = s;

        i += 2;
    }

    else {
        break;
    }
}

if (strcasecmp(response_array[0], "PING") == 0) {

    strcpy(response, "+PONG\r\n");

}
else if (strcasecmp(response_array[0], "ECHO") == 0) {

    sprintf(response,
            "$%zu\r\n%s\r\n",
            strlen(response_array[1]),
            response_array[1]);
}



if (send(client_fd, response, strlen(response), 0) == -1) {
           fprintf(stderr, "Error sending response\n");
            break;
	}
for (int j = 0; j < idx; j++)
    free(response_array[j]);
	}
	close(client_fd);
return NULL;
	}

int main() {
	// Disable output buffering
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	
	// You can use print statements as follows for debugging, they'll be visible when running tests.
	printf("Logs from your program will appear here!\n");

	// Uncomment the code below to pass the first stage
	
	int server_fd, client_addr_len;	
	struct sockaddr_in client_addr;
	
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		printf("Socket creation failed: %s...\n", strerror(errno));
		return 1;
	}
	
	// Since the tester restarts your program quite often, setting SO_REUSEADDR
	// ensures that we don't run into 'Address already in use' errors
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		printf("SO_REUSEADDR failed: %s \n", strerror(errno));
		return 1;
	}
	
	struct sockaddr_in serv_addr = { .sin_family = AF_INET ,
									 .sin_port = htons(6379),
									 .sin_addr = { htonl(INADDR_ANY) },
									};
	
	if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
		printf("Bind failed: %s \n", strerror(errno));
		return 1;
	}
	
	int connection_backlog = 5;
	if (listen(server_fd, connection_backlog) != 0) {
		printf("Listen failed: %s \n", strerror(errno));
		return 1;
	}
	
	printf("Waiting for a client to connect...\n");
	client_addr_len = sizeof(client_addr);
	pthread_t tid;
	while(true){

		int client_fd=	accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
	int *fd = malloc(sizeof(int)) ;
	*fd=client_fd;

		pthread_create(&tid, NULL, handle_client,fd );
// 		close(client_fd);
// free(fd);
		printf("Client connected\n");
	
}
	
	close(server_fd);

	return 0;
}
