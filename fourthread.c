

#define _GNU_SOURCE
//#include<sched.h>
#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include <unistd.h>

#define SET 100000

typedef struct thread_data
{
	int *array;
	int low;
	int high;
}thread_data_t;

void swap (int a[], int left, int right)
{
	int temp;
	temp=a[left];
	a[left]=a[right];
	a[right]=temp; 
}

void quicksortl( int a[], int low, int high )
{
	int pivot;
	thread_data_t front;
	// Termination condition! 
	if ( high > low )
	{
		pivot = partitionl( a, low, high );
		quicksortl( a, low, pivot-1 );
		quicksortl( a, pivot+1, high );
	}
} //end quicksort

void quicksortr( int a[], int low, int high )
{
	int pivot;
	// Termination condition! 
	if ( high > low )
	{
		pivot = partitionr( a, low, high );
		quicksortr( a, low, pivot-1 );
		quicksortr( a, pivot+1, high );
	}
}

int partitionl( int a[], int low, int high )
{
	int left, right;
	int pivot_item;
	int pivot = left = low; 
	pivot_item = a[low]; 
	right = high;
	while ( left < right ) 
	{
		// Move left while item < pivot 
		while( a[left] <= pivot_item ) 
			left++;
		// Move right while item > pivot 
		while( a[right] > pivot_item ) 
			right--;
		if ( left < right ) 
			swap(a,left,right);
	}
	// right is final position for the pivot 
	a[low] = a[right];
	a[right] = pivot_item;
	return right;
}//end partition

int partitionr( int a[], int low, int high )
{
	int left, right;
	int pivot_item;
	int pivot = left = low; 
	pivot_item = a[low]; 
	right = high;
	while ( left < right ) 
	{
		// Move left while item < pivot 
		while( a[left] <= pivot_item ) 
			left++;
		// Move right while item > pivot 
		while( a[right] > pivot_item ) 
			right--;
		if ( left < right ) 
			swap(a,left,right);
	}
	// right is final position for the pivot 
	a[low] = a[right];
	a[right] = pivot_item;
	return right;
}


void *sortfuncl(void *data)
{
	thread_data_t *thread_data_new = (thread_data_t *)data;
	printf("ID: %lu, CPU: %d\n", pthread_self(), sched_getcpu());

	quicksortl(thread_data_new->array,thread_data_new->low,thread_data_new->high);


}

void *sortfuncr(void *data)
{

	thread_data_t *thread_data_new = (thread_data_t *)data;
	printf("ID: %lu, CPU: %d\n", pthread_self(), sched_getcpu());
	quicksortr(thread_data_new->array,thread_data_new->low,thread_data_new->high);

} 

void printarray(int a[], int);

int main()
{

	int a[SET], i, n = SET,pivot,pivotl,pivotr,r;
	pthread_t threadll,threadlr,threadrl,threadrr;
	thread_data_t threadll_data,threadlr_data;
	thread_data_t threadrl_data,threadrr_data;

	pthread_attr_t attr;
	cpu_set_t cpus;

	pthread_attr_init(&attr);

	int numberOfProcessors = sysconf(_SC_NPROCESSORS_ONLN);

	printf("Number of processors:%d\r\n",numberOfProcessors);

	for( i =0 ; i < SET; i++)
	{
		r= rand() % SET;
		a[i] = r ;

	}

	printf("\nUnsorted elements: \n");
	//printarray(a,n);
	pivot = partitionl(a,0,n-1);

	pivotl = partitionl(a,0,pivot -1 );

	threadll_data.array = a;
	threadll_data.low = 0;
	threadll_data.high = pivotl - 1;

	CPU_ZERO(&cpus);
	CPU_SET(0,&cpus);
	pthread_attr_setaffinity_np(&attr,sizeof(cpu_set_t),&cpus);
	pthread_create(&threadll,&attr,sortfuncl,&threadll_data);


	threadlr_data.array = a;
	threadlr_data.low = pivotl +  1;
	threadlr_data.high = pivot - 1;

	CPU_ZERO(&cpus);
	CPU_SET(1,&cpus);
	pthread_attr_setaffinity_np(&attr,sizeof(cpu_set_t),&cpus);
	pthread_create(&threadlr,&attr,sortfuncl,&threadlr_data);




	pivotr = partitionr(a,pivot + 1, n-1);


	threadrl_data.array = a;
	threadrl_data.low = pivot+1;
	threadrl_data.high = pivotr - 1;

	CPU_ZERO(&cpus);
	CPU_SET(2,&cpus);
	pthread_attr_setaffinity_np(&attr,sizeof(cpu_set_t),&cpus);
	pthread_create(&threadrl,&attr,sortfuncr,&threadrl_data);


	threadrr_data.array = a;
	threadrr_data.low = pivotr + 1;
	threadrr_data.high = n - 1;

	CPU_ZERO(&cpus);
	CPU_SET(3,&cpus);
	pthread_attr_setaffinity_np(&attr,sizeof(cpu_set_t),&cpus);
	pthread_create(&threadrr,&attr,sortfuncr,&threadrr_data);


	pthread_join(threadll,NULL);
	pthread_join(threadlr,NULL);
	pthread_join(threadrl,NULL);
	pthread_join(threadrr,NULL);

	printf("\nSorted elements: \n");
	// printarray(a,n);

}


void printarray(int a[], int n)
{
	int i;
	for (i=0; i<n; i++)
		printf(" %d ", a[i]);
	printf("\n");
}
