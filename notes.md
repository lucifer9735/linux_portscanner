# Notes

## header files

- ### #include<sys/socket.h>

  ​

- ### #include<sys/select.h>

- ### #include<sys/types.h>

## functions

#### fd_set

#### struct timeval

#### FD_ZERO()

#### int sockfd = socket(AF_INET, SOCK_STREAM, 0);

#### flag = fcntl(sockfd, F_GETFL, 0);

#### fcntl(sockfd, F_SETFL, flag | O_NONBLOCK);

#### struct sockaddr_in address;

#### bzero(&address, sizeof(address));

#### address.sin_family = AF_INET;

#### inet_pton(AF_INET, ip, &address.sin_addr);

#### address.sin_port = htons(port);

#### connect(sockfd, (struct sockaddr *)&address, sizeof(address))

#### errno != EINPROGRESS

#### FD_SET(sockfd, &rset);

#### int rst=select(sockfd+1, &rset, &wset, NULL, &tval);

#### close(sockfd);

#### FD_ISSET(sockfd, &rset)

#### getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len)