#include<stdio.h>
#include<semaphore.h>
#include<pthread.h>
#include<stdlib.h>
void runner(void *dir);
int carNum=0;//total car number
int car=0;   //car in road
int WestC=0,EastC=0,NorthC=0,SouthC=0;//counter for each direction
//mutex for resources
pthread_mutex_t n_mutex,		//for car crossing
				s_mutex,
				w_mutex,
				e_mutex,
				cnter_mutex;//for counter

//semaphore for queue
sem_t   queueNorth,
		queueSouth,
		queueWest,
		queueEast;
//condition variables
pthread_cond_t  firstNorth,
				firstSouth,
				firstEast,
				firstWest;
//function for threads
void *runnerWest(void *carNum);
void *runnerEast(void *carNum);
void *runnerNorth(void *carNum);
void *runnerSouth(void *carNum);
void *detectDeadLock(void *argv);
int main(int argc, char* argv[])
{
	pthread_t tid[500];//thread for each car,assume 500 is the max number
	pthread_t manager;
	int carNo[500];
	if(argc!=2)
	{
		printf("parameter error!\n");
		exit(0);
	}
	int i=0;
	for(i=0;argv[1][i]!='\0';i++)
		car++;
	carNum=car;
	//initialize muteices
	pthread_mutex_init(&n_mutex,NULL);
	pthread_mutex_init(&s_mutex,NULL);
	pthread_mutex_init(&w_mutex,NULL);
	pthread_mutex_init(&e_mutex,NULL);
	pthread_mutex_init(&cnter_mutex,NULL);
	//initialize semaphores
	sem_init(&queueNorth,0,1);
	sem_init(&queueSouth,0,1);
	sem_init(&queueWest,0,1);
	sem_init(&queueEast,0,1);
	//initialize condition variables
	pthread_cond_init(&firstNorth,NULL);
	pthread_cond_init(&firstSouth,NULL);
	pthread_cond_init(&firstEast,NULL);
	//initialize deadlock detection thread
	pthread_create(&manager,NULL,detectDeadLock,NULL);

	for(i=0;argv[1][i]!='\0';i++){
		carNo[i]=i+1;
		//printf("a\n");
		if(argv[1][i]=='w')
			pthread_create(&tid[i],NULL,runnerWest,(void*)&carNo[i]);
		else if(argv[1][i]=='e')
			pthread_create(&tid[i],NULL,runnerEast,(void*)&carNo[i]);
		else if(argv[1][i]=='n')	
			pthread_create(&tid[i],NULL,runnerNorth,(void*)&carNo[i]);
		else if(argv[1][i]=='s')
			pthread_create(&tid[i],NULL,runnerSouth,(void*)&carNo[i]);
		else{
			printf("wrong parameter:%c\n",argv[1][i]);
			exit(0);
		}
	}
	for(i=0;i<carNum;i++)
		pthread_join(tid[i],NULL);
	car=0;
	pthread_join(manager,NULL);
	printf("end of the program\n");
	return 0;
}
void *runnerWest(void *carNum)
{
	//printf("parameter:\n");
	int no = *(int *)carNum;
	//printf("%d",no);
	sem_wait(&queueWest);
	pthread_mutex_lock(&cnter_mutex);
	WestC++;
	pthread_mutex_unlock(&cnter_mutex);
	printf("car %d from west arrives at crossing\n",no);
	sleep(1);
	pthread_mutex_lock(&s_mutex);
	pthread_mutex_lock(&cnter_mutex);
	if(SouthC>0)
	{
		pthread_mutex_unlock(&cnter_mutex);
		pthread_cond_wait(&firstWest,&s_mutex);
	}
	else
		pthread_mutex_unlock(&cnter_mutex);
	pthread_mutex_lock(&w_mutex);

	pthread_mutex_unlock(&s_mutex);
	pthread_mutex_unlock(&w_mutex);
	//sleep(1);
	printf("car %d from west leaving crossing\n",no);
	pthread_mutex_lock(&cnter_mutex);
	WestC--;
	pthread_mutex_unlock(&cnter_mutex);
	pthread_cond_signal(&firstNorth);
	sem_post(&queueWest);
}
void *runnerEast(void *carNum)
{
	int no= *(int *)carNum;
	sem_wait(&queueEast);
	pthread_mutex_lock(&cnter_mutex);
	EastC++;
	pthread_mutex_unlock(&cnter_mutex);
	printf("car %d from east arrives at crossing\n",no);
	sleep(1);
	pthread_mutex_lock(&n_mutex);
	pthread_mutex_lock(&cnter_mutex);
	if(NorthC>0)
	{
		pthread_mutex_unlock(&cnter_mutex);
		pthread_cond_wait(&firstNorth,&n_mutex);
	}
	else
		pthread_mutex_unlock(&cnter_mutex);
	pthread_mutex_lock(&e_mutex);

	pthread_mutex_unlock(&n_mutex);
	pthread_mutex_unlock(&e_mutex);
	//sleep(1);
	printf("car %d from east leaving crossing\n",no);
	pthread_mutex_lock(&cnter_mutex);
	EastC--;
	pthread_mutex_unlock(&cnter_mutex);
	pthread_cond_signal(&firstSouth);
	sem_post(&queueEast);
}
void *runnerNorth(void *carNum)
{
	int no= *(int *)carNum;
	sem_wait(&queueNorth);
	pthread_mutex_lock(&cnter_mutex);
	NorthC++;
	pthread_mutex_unlock(&cnter_mutex);
	printf("car %d from north arrives at crossing\n",no);
	sleep(1);
	pthread_mutex_lock(&w_mutex);
	pthread_mutex_lock(&cnter_mutex);
	if(WestC>0){
		pthread_mutex_unlock(&cnter_mutex);
		pthread_cond_wait(&firstNorth,&w_mutex);
	}
	else
		pthread_mutex_unlock(&cnter_mutex);
	pthread_mutex_lock(&n_mutex);

	pthread_mutex_unlock(&w_mutex);
	pthread_mutex_unlock(&n_mutex);
	//sleep(1);
	printf("car %d from north leaving crossing\n",no);
	pthread_mutex_lock(&cnter_mutex);
	NorthC--;
	pthread_mutex_unlock(&cnter_mutex);
	pthread_cond_signal(&firstEast);
	sem_post(&queueNorth);
}
void *runnerSouth(void *carNum)
{
	int no= *(int *)carNum;
	sem_wait(&queueSouth);
	pthread_mutex_lock(&cnter_mutex);
	SouthC++;
	//printf("car south:%d",SouthC);//test
	pthread_mutex_unlock(&cnter_mutex);
	printf("car %d from south arrives at crossing\n",no);
	sleep(1);
	pthread_mutex_lock(&e_mutex);
	pthread_mutex_lock(&cnter_mutex);
	if(EastC>0){
		pthread_mutex_unlock(&cnter_mutex);
		pthread_cond_wait(&firstSouth,&e_mutex);
	}
	else
		pthread_mutex_unlock(&cnter_mutex);
	pthread_mutex_lock(&s_mutex);

	pthread_mutex_unlock(&e_mutex);
	pthread_mutex_unlock(&s_mutex);
	//sleep(1);
	printf("car %d from south leaving crossing\n",no);
	pthread_mutex_lock(&cnter_mutex);
	SouthC--;
	pthread_mutex_unlock(&cnter_mutex);
	pthread_cond_signal(&firstWest);
	sem_post(&queueSouth);
}
void *detectDeadLock(void *argv)
{
	while(car){
		pthread_mutex_lock(&cnter_mutex);
		if(WestC!=0&&EastC!=0&&NorthC!=0&&SouthC!=0){
			printf("Deadlock Detected!Signalling north to go\n");
			pthread_cond_signal(&firstNorth);
		}
		pthread_mutex_unlock(&cnter_mutex);
		sleep(4);
	}
}

