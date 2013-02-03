/*
 * NetFrag - Fragmenting streams into UDP payloads
 *
 * Author: Gabriel Gonzalez
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <getopt.h>

int
init_tcp_input(char *ip, int port)
{
	struct sockaddr_in	sin, tmp;
	socklen_t		tmplen = 0;
	int			srv = 0, ret = 0;

	if ((srv = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("init_tcp_input:socket()");
		return -1;
	}
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(ip);
	sin.sin_port = htons(port);

	if (bind(srv, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		perror("init_tcp_input:bind()");
	}
	listen(srv, 1);

	tmplen = sizeof(tmp);
	if ((ret = accept(srv, (struct sockaddr *)&tmp, &tmplen)) < 0) {
		perror("init_tcp_input:accept()");
		return -1;
	}

	return ret;
}

int
init_udp_input(char *ip, int port)
{
	struct sockaddr_in	sin, tmp;
	socklen_t		tmplen = 0;
	int			ret = 0;

	if ((ret = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("init_udp_input:socket()");
		return -1;
	}
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(ip);
	sin.sin_port = htons(port);

	if (bind(ret, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		perror("init_udp_input:bind()");
	}

	return ret;
}

int
init_tcp_output(char *ip, int port)
{
	struct sockaddr_in	sout;
	int			ret = 0;
	
	if ((ret = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("init_tcp_output:socket()");
		return -1;
	}
	memset(&sout, 0, sizeof(sout));
	sout.sin_family = AF_INET;
	sout.sin_addr.s_addr = inet_addr(ip);
	sout.sin_port = htons(port);

	if (connect(ret, (struct sockaddr *)&sout, sizeof(sout)) < 0) {
		perror("init_tcp_output:socket()");
		return -1;
	}

	return ret;
}

int
init_udp_output(char *ip, int port)
{
	struct sockaddr_in	sout;
	int			ret = 0;
	
	if ((ret = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("init_udp_output:socket()");
		return -1;
	}
	memset(&sout, 0, sizeof(sout));
	sout.sin_family = AF_INET;
	sout.sin_addr.s_addr = inet_addr(ip);
	sout.sin_port = htons(port);

	if (connect(ret, (struct sockaddr *)&sout, sizeof(sout)) < 0) {
		perror("init_udp_output:socket()");
		return -1;
	}

	return ret;
}

void
usage(char *name)
{
	fprintf(stderr, "%s [ --src-tcp | --src-udp ] <port> [ --to-udp <destip> <destport> <maxsize>| --to-tcp <destip> <destport> ]\n", name);
}

int
main(int argc, char *argv[])
{
	int	(*init_src)(char *, int);
	int	(*init_dst)(char *, int);
	char	*buffer = NULL, *srcip = NULL, *dstip = NULL;
	int	fdin = 0, fdout = 0, bufflen = 0;
	int	srcport = 0, dstport = 0, maxsize = 0;

	if (argc < 6) {
		usage(argv[0]);
		return -1;
	}

	if (!strcmp(argv[1], "--src-tcp")) {
		init_src = init_tcp_input;
		srcip = "0.0.0.0";
		srcport = atoi(argv[2]);
		fprintf(stderr, "DEBUG: src tcp port: %d\n", srcport);
	} else if (!strcmp(argv[1], "--src-udp")) {
		init_src = init_tcp_input;
		srcip = "0.0.0.0";
		srcport = atoi(argv[2]);
		fprintf(stderr, "DEBUG: src udp port: %d\n", srcport);
	} else {
		usage(argv[0]);
		return -1;
	}

	if (!strcmp(argv[3], "--to-tcp")) {
		init_dst = init_tcp_output;
		dstip = argv[4];
		dstport = atoi(argv[5]);
		fprintf(stderr, "DEBUG: src tcp %s:%d\n", dstip, dstport);
	} else if (!strcmp(argv[3], "--to-udp") && (argc == 7)) {
		init_dst = init_udp_output;
		dstip = argv[4];
		dstport = atoi(argv[5]);
		maxsize = atoi(argv[6]);
		fprintf(stderr, "DEBUG: src udp %s:%d\n", dstip, dstport);
	} else {
		usage(argv[0]);
		return -1;
	}

	if ((buffer = (char *) calloc(maxsize, sizeof(char))) == NULL) {
		perror("calloc()");
		return -1;
	}

	if ((fdin = init_src(srcip, srcport)) < 0) 
		return -1;

	if ((fdout = init_dst(dstip, dstport)) < 0)
		return -1;

	while ((bufflen = read(fdin, buffer, maxsize)) > 0) {
		printf("Read %d Bytes, sending via UDP\n", bufflen);
		if (write(fdout, buffer, bufflen) != bufflen) {
			perror("sendto()");
			return -1;
		}		
	}

	free(buffer);
	close(fdin);
	close(fdout);

	return 0;
}
