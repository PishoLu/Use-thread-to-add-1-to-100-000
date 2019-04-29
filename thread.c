#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <pthread.h>

struct arguement
{
	int op;
	int ed;
};

struct shm_mem
{
	int lock;
	long and[4];
};

long accumulation(struct arguement* oped) {
	long count = 0;
	//printf("oped->op=%d\noped->ed=%d\n", oped->op, oped->ed);
	int times = oped->op;
	while (times <= oped->ed)
	{
		count += times;
		times++;
	}
	//printf("%ld\n", count);
	return count;
}

int main()
{
	int shm_id; //共享内存标识符
	struct shm_mem* shared;
	void* shm = NULL;
	shm_id = shmget(IPC_PRIVATE, 4 * sizeof(int), 0666);//创建共享内存
	if (shm_id < 0) {
		perror("shmget");
		exit(1);
	}

	printf("successfully created segment : %d \n", shm_id);
	system("ipcs -m"); /*调用ipcs命令查看IPC*/
	shm = shmat(shm_id, (void*)0, 0);
	if (shm == (void*)-1)
	{
		fprintf(stderr, "shmat fail\n");
	}
	printf("Memory attached at %x\n", (int)shm);
	shared = (struct shm_mem*)shm;
	shared->lock = 1;

	pid_t pid = fork();
	if (pid < 0) {
		printf("vfork error");
	}
	else if (pid == 0) {
		printf("this is child\n");


		int status = 0;
		long reason1, reason2, reason3, reason4;
		int temp1, temp2, temp3, temp4;

		struct arguement* arguement1;
		struct arguement* arguement2;
		struct arguement* arguement3;
		struct arguement* arguement4;

		pthread_t thread1, thread2, thread3, thread4;

		arguement1 = (struct arguement*)malloc(sizeof(struct arguement));
		arguement1->op = 1;
		arguement1->ed = 25000;
		//printf("%d,%d\n", arguement1->op, arguement1->ed);
		status = pthread_create(&thread1, NULL, (void*)& accumulation, (void*)arguement1);
		if (status != 0) {
			printf("thread1 fails\n");
		}
		else {
			printf("thread1 successful\n");
			pthread_join(thread1, &reason1);
			printf("thread1 return value is %ld\n", reason1);
			while (1)
			{
				if (shared->lock == 1) {
					shared->lock = 0;
					printf("thread1 is writing\n");
					shared->and [0] = reason1;
					printf("thread1 writing over\n");
					shared->lock = 1;
					break;
				}
				else {
					sleep(1);
				}
			}
		}
		arguement2 = (struct arguement*)malloc(sizeof(struct arguement));
		arguement2->op = 25001;
		arguement2->ed = 50000;
		//printf("%d,%d\n", arguement2->op, arguement2->ed);
		status = pthread_create(&thread2, NULL, (void*)& accumulation, (void*)arguement2);
		if (status != 0) {
			printf("thread2 fails\n");
		}
		else {
			printf("thread2 successful\n");
			pthread_join(thread2, &reason2);
			printf("thread2 return value is %ld\n", reason2);
			while (1)
			{
				if (shared->lock == 1) {
					shared->lock = 0;
					printf("thread2 is writing\n");
					shared->and [1] = reason2;
					printf("thread2 writing over\n");
					shared->lock = 1;
					break;
				}
				else {
					sleep(1);
				}
			}
		}
		arguement3 = (struct arguement*)malloc(sizeof(struct arguement));
		arguement3->op = 50001;
		arguement3->ed = 75000;
		//printf("%d,%d\n", arguement3->op, arguement3->ed);
		status = pthread_create(&thread3, NULL, (void*)& accumulation, (void*)arguement3);
		if (status != 0) {
			printf("thread3 falis\n");
		}
		else {
			printf("thread3 successful\n");
			pthread_join(thread3, &reason3);
			printf("thread3 return value is %ld\n", reason3);
			while (1)
			{
				if (shared->lock == 1) {
					shared->lock = 0;
					printf("thread3 is writing\n");
					shared->and [2] = reason3;
					printf("thread3 writing over\n");
					shared->lock = 1;
					break;
				}
				else {
					sleep(1);
				}
			}
		}
		arguement4 = (struct arguement*)malloc(sizeof(struct arguement));
		arguement4->op = 75001;
		arguement4->ed = 100000;
		//printf("%d,%d\n", arguement4->op, arguement4->ed);
		status = pthread_create(&thread4, NULL, (void*)& accumulation, (void*)arguement4);
		if (status != 0) {
			printf("线程4创建失败\n");
		}
		else {
			printf("thread4 successful\n");
			pthread_join(thread4, &reason4);
			printf("thread4 return value is %ld\n", reason4);
			while (1)
			{
				if (shared->lock == 1) {
					shared->lock = 0;
					printf("thread4 is writing\n");
					shared->and [3] = reason4;
					printf("thread4 writing over\n");
					shared->lock = 1;
					break;
				}
				else {
					sleep(1);
				}
			}
		}
		shmdt(&shared);
	}
	else {
		printf("this is father\n");
		sleep(5);
		unsigned long fin_reason = 0;
		int i = 0;
		for (; i < 4; i++) {
			printf("and[%d]=%ld\n", i, shared-> and [i]);
			fin_reason += shared-> and [i];
		}
		printf("1-100,000 add to %ld\n", fin_reason);
		shmdt(&shared);
		FILE* fp = NULL;
		char name[20]; 
		sprintf(name, "%ld", fin_reason);
		printf("%s\n", name);
		fp = fopen(name, "w+");
		fclose(fp);
	}
	shmctl(shm_id, IPC_RMID, NULL);
	return 0;
}