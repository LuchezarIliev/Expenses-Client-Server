#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <errno.h>

#include <winsock2.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define MAX 1024
#define PORT 8080
#define SA struct sockaddr

enum Expenses
{
    Kola = 1, Tok = 2, Voda = 3, Parno = 4, Telefon = 5, Kredit = 6, Zastrahovka = 7
};

void printExpenseTypes()
{
    printf("\nChoose expense type or enter custom one:\n");
	printf("\t1-Kola\n");
	printf("\t2-Tok\n");
	printf("\t3-Voda\n");
	printf("\t4-Parno\n");
	printf("\t5-Telefon\n");
	printf("\t6-Kredit\n");
	printf("\t7-Zastrahovka\n");
}

void getDataFromClient(char *buff)
{
    int n = 0;

    char expense[MAX] = {0};
    printf("\tType: ");
    while((expense[n++] = getchar()) != '\n');
    if (strncmp("exit", expense, 4) == 0)
    {
        strcpy(buff, expense);
        return;
    }
    expense[n-1] = '\0';
    int i;
    if(sscanf(expense, "%d", &i) == 1)
    {
        switch(i)
        {
            case Kola: strcpy(expense, "Kola"); break;
            case Tok: strcpy(expense, "Tok"); break;
            case Voda: strcpy(expense, "Voda"); break;
            case Parno: strcpy(expense, "Parno"); break;
            case Telefon: strcpy(expense, "Telefon"); break;
            case Kredit: strcpy(expense, "Kredit"); break;
            case Zastrahovka: strcpy(expense, "Zastrahovka"); break;
        }
    }
    n = 0;

    char value[MAX] = {0};
    printf("\tValue: ");
    while((value[n++] = getchar()) != '\n');
    value[n-1] = '\0';
    n = 0;

    char date[MAX] = {0};
    printf("\tDate (DD.MM.YYYY or type 'now'):");
    while((date[n++] = getchar()) != '\n');
    date[n-1] = '\0';
    if (strncmp("now", date, 3) == 0)
    {
        memset(date, 0, sizeof(date));
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        sprintf(date, "%02d.%02d.%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    }

    sprintf(buff, "%s,%s,%s\n", expense, value, date);
}

void addExpenses(int sockfd)
{
    char buff[MAX] = { 0 };
    int n; // Bytes received per read
    for(;;)
    {
        memset(buff, 0, sizeof(buff));
        n = 0;
        printf("\nNew Expense: \n");

        getDataFromClient(buff);

        send(sockfd, buff, sizeof(buff), 0);
        memset(buff, 0, sizeof(buff));

        n = recv(sockfd, buff, sizeof(buff), 0);
        printf("Server: %s\n", buff);

        if ((strncmp(buff, "exit", 4)) == 0)
        {
            printf("Client Exit...\n");
            break;
        }
    }
}

void getExpensesByMonth(int sockfd)
{
    char buff[MAX] = { 0 };
    int n; // Bytes received per read
    for(;;)
    {
        memset(buff, 0, sizeof(buff));
        n = 0;
        printf("\nEnter month (format MM): ");

        while((buff[n++] = getchar()) != '\n');

        send(sockfd, buff, sizeof(buff), 0);
        memset(buff, 0, sizeof(buff));

        n = recv(sockfd, buff, sizeof(buff), 0);
        printf("Server: \n%s", buff);

        if ((strncmp(buff, "exit", 4)) == 0)
        {
            printf("Client Exit...\n");
            break;
        }
    }
}

void process(int sockfd)
{
    char serverResponse[MAX] = {0};
    int choice, k;
    do
    {
		printf("\t\t\tMenu:\n\n");
		printf("\t|1| Enter data for new expenses\n");
		printf("\t|2| Get total expenses for a month\n");
		printf("\t|3| Get biggest expense\n");
		printf("\t|4| Exit\n\n");
		printf("Choose [1-4]: ");
		do
		{
			k = scanf("%d", &choice);
			while (getchar() != '\n');
			if (choice < 1 || choice > 4)
				printf("\tError! Try again:\n");
		} while (k != 1);
        system("CLS");
        char strChoice[2] = {0};
		switch (choice)
		{
            case 1:
                printf("|1| Enter data for new expenses\n");

                sprintf(strChoice, "%d", choice);
                send(sockfd, strChoice, sizeof(strChoice), 0);
                memset(strChoice, 0, sizeof(strChoice));
                recv(sockfd, serverResponse, sizeof(serverResponse), 0);
                printf("%s", serverResponse);
                memset(serverResponse, 0, sizeof(serverResponse));

                printExpenseTypes();

                addExpenses(sockfd);
                break;
            case 2:
                printf("|2| Get total expenses for a month\n");

                sprintf(strChoice, "%d", choice);
                send(sockfd, strChoice, sizeof(strChoice), 0);
                memset(strChoice, 0, sizeof(strChoice));
                recv(sockfd, serverResponse, sizeof(serverResponse), 0);
                printf("%s", serverResponse);
                memset(serverResponse, 0, sizeof(serverResponse));

                getExpensesByMonth(sockfd);
                break;
            case 3:
                printf("|3| Get biggest expense\n");

                sprintf(strChoice, "%d", choice);
                send(sockfd, strChoice, sizeof(strChoice), 0);
                memset(strChoice, 0, sizeof(strChoice));
                recv(sockfd, serverResponse, sizeof(serverResponse), 0);
                printf("%s", serverResponse);
                memset(serverResponse, 0, sizeof(serverResponse));

                getExpensesByMonth(sockfd);
                break;
		}
	} while (choice != 4);
}

int main()
{
    WSADATA wsa;
	SOCKET s;

	printf("Initializing Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code: %d",WSAGetLastError());
		return 1;
	}

	printf("Initialized\nCreating socket...");

	if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket: %d" , WSAGetLastError());
	}

	printf("Socket created\nConnecting...");

    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr("192.168.0.102"); // public IP: 87.116.97.43 | private IP: 192.168.0.102
    server.sin_family = AF_INET;
	server.sin_port = htons(PORT);

    //Connect to remote server
	if (connect(s , (struct sockaddr *)&server , sizeof(server)) ==  SOCKET_ERROR)
	{
		printf("Connection timeout. Error number: %d\n", WSAGetLastError());
		return 1;
	}
    else
    {
        printf("Connection successful\n");
        process(s);
    }
    closesocket(s);
    WSACleanup();
    return 0;
}
