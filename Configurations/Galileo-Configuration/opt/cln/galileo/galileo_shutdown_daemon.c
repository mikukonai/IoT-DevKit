#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/select.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>

#define PATH_LEN 200
#define GPIO_VALUE_PATH "/sys/class/gpio/gpio%d/value"

fd_set fs_except_set;

int gpio_fd;

int main(int argc, char * argv[])
{
	char gpio_path[PATH_LEN];
	char command[PATH_LEN];
	int  gpio_num = 53;
	int arg;

	while ((arg = getopt(argc, argv, "i:c:")) != -1) {
		switch(arg) {
		case 'i':
			gpio_num = atoi(optarg);
			printf("监视引脚编号：%d\n", gpio_num);
			break;
		case 'c':
			strcpy(command, optarg);
			printf("回调命令：%s\n", command);
			break;
		default:
			break;
		}
	}

	sprintf(gpio_path,"echo in > /sys/class/gpio/gpio%d/direction ", gpio_num);
	system(gpio_path);

	//sprintf(gpio_path,"echo both > /sys/class/gpio/gpio%d/edge ", gpio_num);
	//system(gpio_path);


	// 设定路径中的GPIO号
	sprintf(gpio_path, GPIO_VALUE_PATH, gpio_num);

	// 打开GPIO文件
	gpio_fd = open(gpio_path, O_RDWR);
	printf("GPIO%d: %s\n", gpio_fd, gpio_path);
	if(gpio_fd < 0) {
		//gpio_fd = open(gpio_path, O_RDWR);
		fprintf(stderr, "Failed to open: %s. Try again.\n", gpio_path);
	}

	int gpio_value = 0;
	lseek(gpio_fd, 0, SEEK_SET);
	read(gpio_fd, &gpio_value, 1);
	int gpio_initial_value = gpio_value;
	printf("Gpio init val: %c\n", gpio_initial_value);

	int counter = 0;
	int ret = 0;

	while(1) {
		FD_ZERO(&fs_except_set);
		FD_SET(gpio_fd, &fs_except_set);
		ret = select(gpio_fd + 1, 0, 0, &fs_except_set, 0);

		switch(ret){
		case -1:
			fprintf(stderr, "critical fault during select errno=%d", errno);
			break;
		case 0:
			/* timeout */
			fprintf(stderr,"select timeout\n");
			break;
		default:
			if(FD_ISSET(gpio_fd, &fs_except_set)){
				lseek(gpio_fd, 0, SEEK_SET);
				read(gpio_fd, &gpio_value, 1);
				if(gpio_value == '0') {
					counter = 20;
					while(counter--){
						system("echo 1 > /sys/class/gpio/gpio3/value");
						usleep(30000);
						system("echo 0 > /sys/class/gpio/gpio3/value");
						usleep(30000);
					};
					printf("Execute command: %s\n", command);
					system(command);
				}
				else if(gpio_value == '1') {
					counter = 0;
				}
			}
		}
	}
}
