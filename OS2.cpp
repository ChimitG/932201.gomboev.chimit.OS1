#include <vector>
#include <signal.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

volatile sig_atomic_t wasSigHup = 0;
void sigHupHandler(int r)
{
wasSigHup = 1;
}

int main() {
struct sigaction sa;
sigaction(SIGHUP, NULL, &sa);
sa.sa_handler = sigHupHandler;
sa.sa_flags |= SA_RESTART;
sigaction(SIGHUP, &sa, NULL);

sigset_t blockedMask, origMask;
sigemptyset(&blockedMask);
sigaddset(&blockedMask, SIGHUP);
sigprocmask(SIG_BLOCK, &blockedMask, &origMask);

int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

struct sockaddr_in serverAddr;
serverAddr.sin_family = AF_INET;
serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
serverAddr.sin_port = htons(1234);

if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
printf("Ошибка\n");
exit(EXIT_FAILURE);
}

if (listen(serverSocket, SOMAXCONN) < 0) {
printf("Ошибка\n");
exit(EXIT_FAILURE);
}

printf("сервер запущен\n");

std::vector<int> clients;
int maxFd = serverSocket;
fd_set fds;

while (true)
{
FD_ZERO(&fds);
FD_SET(serverSocket, &fds);
maxFd = serverSocket;

for (int clientSocket : clients) {
FD_SET(clientSocket, &fds);
if (clientSocket > maxFd) maxFd = clientSocket;
}

if (pselect(maxFd + 1, &fds, NULL, NULL, NULL, &origMask) == -1)
{
if (errno == EINTR)
{
printf("some actions on receiving the signal\n");
if (wasSigHup) {
wasSigHup = 0;
printf("some actions on the descriptor activity\n");
}
continue;
}
else
{
break;
}
}
{
printf("Внутренняя работа\n");
}
}
close(serverSocket);
}
