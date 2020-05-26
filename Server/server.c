#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define MAX 1024 
#define PORT 8080
#define SA struct sockaddr

void addExpenses(int sockfd, char *file);
void getTotalExpenses(int sockfd, char *file);
void getBiggestExpense(int sockfd, char *file);

double calcExpensesForMonth(char *content, char *month, char *filtered);
double calcBiggestExpenseForMonth(char *content, char *month, char *type, char *filtered);

void addExpenses(int sockfd, char *file)
{
    char message[MAX] = {0};
    int n;

	int fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0777); //Creates file, opens it for write/append
	if (fd < 0)
	{
		printf("Unable to open file %s\n", file);
		exit(-1);
	}
	
	if (fd >= 0)
	{
	    for(;;)
        {
            bzero(message, MAX); 

	        // Read the message from client and copy it in buffer 
	        read(sockfd, message, sizeof(message));

            printf("Client message: %s", message);

            // If client message is "exit" then server stops receiving data for this mode
            if (strncmp("exit", message, 4) == 0)
            {
                write(sockfd, message, sizeof(message));
                break;
            }
                
            write(fd, message, strlen(message));
	        bzero(message, MAX);
			    
            n = 0;

            char response[MAX] = "Data was successfully saved!";
	        write(sockfd, response, sizeof(response));
            bzero(response, MAX);
        }

        (void) close(fd);
    }
}

void getTotalExpenses(int sockfd, char *file)
{
    char message[MAX] = {0};

	int fd = open(file, O_RDONLY | O_CREAT); //Creates file if not found, opens it for read
	if (fd < 0)
	{
		printf("Unable to open file %s\n", file);
		exit(-1);
	}
	
	if (fd >= 0)
	{
        char fileContent[MAX] = {0};
        read(fd, fileContent, sizeof(fileContent));
	    for(;;)
        {
	        // Read the message from client and copy it in buffer 
	        read(sockfd, message, sizeof(message));

            printf("Client message: %s", message);

            // If client message is "exit" then server stops receiving data for this mode
            if (strncmp("exit", message, 4) == 0)
            {
                write(sockfd, message, sizeof(message));
                break;
            }
            
            // Logic
            char temp[MAX] = {0};
            strcpy(temp, fileContent); // Copying the file contents to temp string because strtok modifies the original string
            char filteredData[MAX] = {0};            
            double totalExpenses = calcExpensesForMonth(temp, message, filteredData);
            bzero(temp, MAX);

            char response[MAX] = {0};
            sprintf(response, "%s\nTotal expenses: %.2f", filteredData, totalExpenses);
            bzero(message, MAX);
            bzero(filteredData, MAX);
            // Response to client
	        write(sockfd, response, sizeof(response));
            bzero(response, MAX);
        }

        (void) close(fd);
    }
}

double calcExpensesForMonth(char *content, char *month, char *filtered)
{
    double totalExpenses = 0;
    char *line;
    char *token;
    char buf[256];
    for (line = strtok(content, "\n"); line != NULL;
        line = strtok(line + strlen(line) + 1, "\n"))
    {
        strncpy(buf, line, sizeof(buf));
        //printf("Line: %s\n", buf);

        // Gets expense value
        int i = 0;
        char *arr[3];                    
        for (token = strtok(buf, ","); token != NULL;
            token = strtok(token + strlen(token) + 1, ","))
        {
            arr[i] = token;
            //printf("\tToken: %s\n", arr[i]);
            i++;
        }

        //printf("Value: %s\n", arr[1]);
        
        // Gets month number
        char *a[3];
        int j = 0;
        a[j] = strtok(arr[2], ".");
        while (a[j] != NULL)
        {
            a[++j] = strtok(NULL, ".");
        }
        //printf("Month: %s\n", a[1]);

        if (strncmp(a[1], month, 2) == 0)
        {
            totalExpenses += strtod(arr[1], NULL);
            char rLine[MAX] = {0};
            sprintf(rLine, "%s\n", line);
            filtered = strcat(filtered, rLine);
        }
    }
    if (totalExpenses == 0)
    {
        filtered = strcat(filtered, "No expenses for this month");
    }
    return totalExpenses;
}

void getBiggestExpense(int sockfd, char *file)
{
    char message[MAX] = {0};

	int fd = open(file, O_RDONLY | O_CREAT); //Creates file if not found, opens it for read
	if (fd < 0)
	{
		printf("Unable to open file %s\n", file);
		exit(-1);
	}
	
	if (fd >= 0)
	{
        char fileContent[MAX] = {0};
        read(fd, fileContent, sizeof(fileContent));
	    for(;;)
        {
	        // Read the message from client and copy it in buffer 
	        read(sockfd, message, sizeof(message));

            printf("Client message: %s", message);

            // If client message is "exit" then server stops receiving data for this mode
            if (strncmp("exit", message, 4) == 0)
            {
                write(sockfd, message, sizeof(message));
                break;
            }
            
            // Logic
            char temp[MAX] = {0};
            strcpy(temp, fileContent); // Copying the file contents to temp string because strtok modifies the original string
            char filteredData[MAX] = {0};
            char expenseType[MAX] = {0};        
            double biggestExpense = calcBiggestExpenseForMonth(temp, message, expenseType, filteredData);
            bzero(temp, MAX);

            char response[MAX] = {0};
            sprintf(response, "%s\nBiggest expense: %s Value: %.2f", filteredData, expenseType, biggestExpense);
            bzero(message, MAX);
            bzero(filteredData, MAX);
            bzero(expenseType, MAX);
            // Response to client
	        write(sockfd, response, sizeof(response));
            bzero(response, MAX);
        }

        (void) close(fd);
    }
}

double calcBiggestExpenseForMonth(char *content, char *month, char *type, char *filtered)
{
    double biggestExpense = 0;
    char *line;
    char *token;
    char buf[256];
    for (line = strtok(content, "\n"); line != NULL;
        line = strtok(line + strlen(line) + 1, "\n"))
    {
        strncpy(buf, line, sizeof(buf));
        //printf("Line: %s\n", buf);

        // Gets expense value
        int i = 0;
        char *arr[3];                    
        for (token = strtok(buf, ","); token != NULL;
            token = strtok(token + strlen(token) + 1, ","))
        {
            arr[i] = token;
            //printf("\tToken: %s\n", arr[i]);
            i++;
        }

        //printf("Value: %s\n", arr[1]);
        
        // Gets month number
        char *a[3];
        int j = 0;
        a[j] = strtok(arr[2], ".");
        while (a[j] != NULL)
        {
            a[++j] = strtok(NULL, ".");
        }
        //printf("Month: %s\n", a[1]);

        if (strncmp(a[1], month, 2) == 0)
        {
            double current = strtod(arr[1], NULL);
            if (current > biggestExpense)
            {
                biggestExpense = current;
                sprintf(type, "%s", arr[0]);
            }
    
            char rLine[MAX] = {0};
            sprintf(rLine, "%s\n", line);
            filtered = strcat(filtered, rLine);
        }
    }
    if (biggestExpense == 0)
    {
        filtered = strcat(filtered, "No expenses for this month");
    }
    return biggestExpense;
}

// Function designed for chat between client and server. 
void process(int sockfd, char *ip) 
{
    // Create unique user file if it doesn't exist
    char *file = strcat(ip, ".txt");
    printf("User file: %s\n", file);
    
    for (;;)
    {
        char choice[2];
        char response[MAX];
        // Get client choice
        read(sockfd, choice, sizeof(choice));
        if (strncmp("1", choice, 1) == 0)
        {
            strcpy(response, "Enter 'exit' to stop adding expenses");
            write(sockfd, response, sizeof(response));
            bzero(response, MAX);

            addExpenses(sockfd, file);
        }
        else if (strncmp("2", choice, 1) == 0)
        {
            strcpy(response, "Enter 'exit' to stop searching total expenses by month");
            write(sockfd, response, sizeof(response));
            bzero(response, MAX);

            getTotalExpenses(sockfd, file);
        }
        else if (strncmp("3", choice, 1) == 0)
        {
            strcpy(response, "Enter 'exit' to stop searching biggest expense by month");
            write(sockfd, response, sizeof(response));
            bzero(response, MAX);

            getBiggestExpense(sockfd, file);
        }
        else
        {
            strcpy(response, "Mode not implemented!");
            write(sockfd, response, sizeof(response));
            bzero(response, MAX);
        }   
    }
} 

// Driver function 
int main(int argc, char **argv) 
{
	int sockfd, connfd, len; 
	struct sockaddr_in servaddr, cli; 

	// Socket creation and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1)
    { 
		printf("Socket creation failed\n"); 
		exit(0); 
	} 

	printf("Socket successfully created\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// Assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr("192.168.0.102"); // Bridged 192.168.0.102
	servaddr.sin_port = htons(PORT);
	
	int on = 1;
	if(setsockopt(sockfd, SOL_IP, IP_TRANSPARENT, &on, sizeof on) == -1)
	{
		printf("setsockopt failed %s\n", strerror(errno));
		exit(-1); 
	}

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0)
    { 
		printf("Socket bind failed\n"); 
		exit(-1); 
	} 
	else
		printf("Socket successfully binded\n"); 

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0)
    { 
		printf("Listen failed\n"); 
		exit(0); 
	} 
	else
		printf("Server listening...\n"); 
	len = sizeof(cli); 

    int pid;
    while (1)
    {
	    // Accept the data packet from client and verification 
	    connfd = accept(sockfd, (SA*)&cli, &len); 
	    if (connfd < 0)
        { 
		    printf("Server accept failed...\n"); 
		    exit(0); 
	    }
        // Using fork to handle multiple clients
        pid = fork();
        if (pid < 0)
        {
            printf("Error creating process\n");
        }

        if (pid == 0)
        {
            // Child process
            close(sockfd);

	        printf("Server accept client\n");
            struct in_addr ipAddress = ((struct sockaddr_in*)&cli)->sin_addr;
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &ipAddress, ip, INET_ADDRSTRLEN);
            printf("Client IP is: %s\n", ip);

	        // Process messages between client and server 
	        process(connfd, ip);

            close(connfd);
        }
        else
        {
            //parent process
            close(connfd);
        }
    }
	// Close the socket
	//close(sockfd);
}
