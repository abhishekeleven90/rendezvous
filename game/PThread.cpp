//============================================================================
// Name        : Hello.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <pthread.h>
#include <stdio.h>

/* this function is run by the second thread */
/*void *inc_x(void *x_void_ptr) {

	 increment x to 100
	int *x_ptr = (int *) x_void_ptr;
	while (++(*x_ptr) < 100)
		;

	printf("x increment finished\n");

	 the function must return something - NULL will do
	return NULL;

}*/

/* this function is run by the second thread */
void* myThread(void* x_void_ptr) {
	while (true) {
		printf("myThread\n");
	}
	return NULL;
}

int main() {

	/* this variable is our reference to the second thread */
	pthread_t my_thread_ref;

	/* create a second thread which executes inc_x(&x) */
	/*	if (pthread_create(&inc_x_thread, NULL, inc_x, &x)) {

	 fprintf(stderr, "Error creating thread\n");
	 return 1;

	 }*/

	if (pthread_create(&my_thread_ref, NULL, myThread, NULL)) {

		fprintf(stderr, "Error creating thread\n");
		return 1;

	}

	/* increment y to 100 in the first thread */
	while (true) {
		printf("first thread\n");
	}
	//join code, may not be required as of now, may be in client
	/* wait for the second thread to finish
	 if (pthread_join(inc_x_thread, NULL)) {

	 fprintf(stderr, "Error joining thread\n");
	 return 2;

	 }
	 */
	return 0;

}

