#include <pthread.h>
#include <stdio.h>

/* this function is run by the second thread */
void* myThread(void* x_void_ptr) {
	while (true) {
		printf("myThread\n");
	}
	return NULL;
}

int main() {

	pthread_t my_thread_ref;

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

