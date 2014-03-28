int open_port(uint16_t port) {
  int connfd, sockfd, y[1]={1};
  struct sockaddr_in addr = {.sin_family=AF_INET,.sin_port=htons(port),.sin_addr={.s_addr=htonl(INADDR_ANY)}};
  if( ( sockfd = socket(PF_INET, SOCK_STREAM, 0)                         ) < 0)  perror(  "socket"  );
  if( (      setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, y, sizeof(int))) < 0)  perror("setsockopt");
  if( (            bind(sockfd, (struct sockaddr*)&addr, sizeof(addr))   ) < 0)  perror(   "bind"   );
  if( (          listen(sockfd, 1)                                       ) < 0)  perror(  "listen"  );
  if( ( connfd = accept(sockfd, NULL, 0)                                 ) < 0)  perror(  "accept"  );
  return connfd;
}
