#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define BUFSIZE 5

bool	selecting(int fd){
	fd_set	read_fd;

	FD_SET(fd, &read_fd);
	if (select(1, &read_fd, NULL, NULL, NULL) == -1)
		return (false);
	return (true);
}

int	main()
{
	int	fd = 0;
	int	ret;
	char	buf[BUFSIZE];
	memset(buf, 0, BUFSIZE);
	fcntl(fd, F_SETFL, O_NONBLOCK);

	while (1){
		ret = read(fd, buf, BUFSIZE - 1);
		if (ret == -1 && errno == EAGAIN)
		{
			printf("ENGAIN [%s]\n", buf);
			if (selecting(fd) == false)
			{
				perror("( ; _ ; )/~~~");
				return (1);
			}
		}
		else
		{
			printf("FINISH! [%s]\n", buf);
			memset(buf, 0, BUFSIZE);
		}
	}
}