/*Group 5: Veronika Alex, Elizabeth Mlynarski*/

/* 
This code primarily comes from
http://www.prasannatech.net/2008/07/socket-programming-tutorial.html
and
http://www.binarii.com/files/papers/c_sockets.txt
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <curses.h>
#include <fcntl.h>
#include <sys/select.h>

// #include "parseCSV.h"
#include "modify_operations.h"
#include "html.h"
#include "dataframe.h"


typedef struct pthread_t_data {
	int* thr_fd;
	data_frame* thr_df;
	int tid;
} thr_data;
// thr_data* thr_data_ary = NULL;




int start_server(int PORT_NUMBER, data_frame* df);
void* user_input_thread(void* v);
void* process_one_request(void* v_thr_data);
char* sort_request(int column, int direction, char* html_str, data_frame* df);
char* filter_request(float threshold, int comparison, char* html_str, data_frame* df);
char* calculate_request(int calc, char* html_str, data_frame* df);


/*
 * Global variables for threading
 */
int flag_quit = 0;
int flag_thr_count = 0;


pthread_mutex_t input_mutex, thr_count;
// pthread_cond_t  input_cond = PTHREAD_COND_INITIALIZER;

pthread_t input_thr;


int kbhit() {
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds); 
    return select(1, &fds, NULL, NULL, &tv);
}


void* user_input_thread(void* v) {
	int user_quit = 0;

	sleep(10);

	while(user_quit == 0){
		if (!kbhit()) {
			// printf("no input available\n");
			sleep(4);
		}
		else {
			char ch = getchar();
			printf("input ch = %c\n", ch);
			if (ch == 'q') {
            	
            	user_quit = 1;
            	pthread_mutex_lock(&input_mutex);
            	flag_quit = 1;
            	pthread_mutex_unlock(&input_mutex);

            	printf("QUIT!!\n");
            	printf("exiting input thread\n");
            	pthread_exit(NULL);
    			return NULL;
            }
			else continue;
		}

    }
    printf("exiting input thread\n");
    pthread_exit(NULL);
    return NULL;
}



int start_server(int PORT_NUMBER, data_frame* df) {

	// structs to represent the server and client
	struct sockaddr_in server_addr,client_addr;		
	int sock; // socket descriptor


	// 1. socket: creates a socket descriptor that you later use to make other system calls
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}
	int temp;
	if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&temp,sizeof(int)) == -1) {
		perror("Setsockopt");
		exit(1);
	}

	// configure the server
	server_addr.sin_port = htons(PORT_NUMBER); // specify port number
	server_addr.sin_family = AF_INET;				 
	server_addr.sin_addr.s_addr = INADDR_ANY; 
	bzero(&(server_addr.sin_zero),8); 
	
	// 2. bind: use the socket and associate it with the port number
	if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
		perror("Unable to bind");
		exit(1);
	}

	// 3. listen: indicates that we want to listen to the port to which we bound; second arg is number of allowed connections
	if (listen(sock, 1) == -1) {
		perror("Listen");
		exit(1);
	}
					
	// once you get here, the server is set up and about to start listening
	printf("\nServer configured to listen on port %d\n", PORT_NUMBER);
	fflush(stdout);
	
	while(1) {
		// sleep(1);
		pthread_mutex_lock(&input_mutex);
        int check = flag_quit;
        pthread_mutex_unlock(&input_mutex);
		
		if (check == 1) {
			printf("\nWebserver terminated by the user\n");
			break;
		}

		// 4. accept: wait here until we get a connection on that port
		int sin_size = sizeof(struct sockaddr_in);

		int fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);
		if (fd != -1) {
			printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

			/*
			 * copy the fd & data_frame struct for this thread 
			 * --> save in this thread's thread_data struct 
			 */
		
			thr_data* data_t = NULL;

			data_t = malloc(sizeof(thr_data));
			if (!data_t) {
				printf("malloc for course number failed\n");
				return -1 ;
			}
			/* copy the file descriptor */
			data_t->thr_fd = NULL;
			data_t->thr_fd = malloc(sizeof(int));
			memcpy(data_t->thr_fd, &fd, sizeof(int));

			/* copy the data_frame */
			data_t->thr_df = NULL;
			data_t->thr_df = copy_dataframe(df, data_t->thr_df);

			pthread_t thr;

			if (pthread_create(&thr, NULL, process_one_request, data_t) != 0) {
    			perror("pthread_create");
    			exit(1);
  			}

  			pthread_mutex_lock(&thr_count);
  			flag_thr_count++;
  			pthread_mutex_unlock(&thr_count);
  			sleep(1);


		}

	}

	/* wait here until all processing threads have finished */
	while(flag_thr_count > 0) {
		sleep(1);
	}

	// 8. close: close the socket
	close(sock);
	printf("Server shutting down\n");
	return 0;
} 




int main(int argc, char *argv[]) {

	char* file_name;
	data_frame* df = NULL;

	// check the number of arguments
	if (argc != 3) {
			printf("\nUsage: %s [port_number] [csv file name]\n", argv[0]);
			exit(-1);
	}

	int port_number = atoi(argv[1]);
	if (port_number <= 1024) {
		printf("\nPlease specify a port number greater than 1024\n");
		exit(-1);
	}

	/* FILE: open -> read -> convert to data_frame struct */
	file_name = argv[2];
	df = parse_data_file(df, file_name);
	

	pthread_mutex_init(&input_mutex, NULL);
	if (pthread_create(&input_thr, NULL, &user_input_thread, NULL) != 0) {
		perror("pthread_create");
		exit(1);
	}
	sleep(1);


	start_server(port_number, df);

	//////////////// DO SOMETHING ELSE HERE??? ///////

	free_data_frame_struct(df); 
	free(df);
	return 0;
}






//////////////////////////////////////////////////////////////////////////////////
//////// http request processing functions ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


/*
 * Parses http request --> calls appropriate fxn to execute the request
 */
void* process_one_request(void* v_thr_data) {
	char request[4096];
	char *reply = NULL;
	char* test_get = NULL;

	thr_data* this_thr = (thr_data*) v_thr_data;
	int* fd = this_thr->thr_fd;
	data_frame* df = this_thr->thr_df;

	// recv: read incoming message (request) into buffer
	int bytes_received = recv(*fd,request,1024,0);
	// null-terminate the string
	request[bytes_received] = '\0';
	// print it to standard out
	printf("\n\nThis is the incoming request:\n%s\n", request);

	/* 1. determine if this is a GET request or not: */
	test_get = strstr(request, "GET");
	if (test_get) {
		printf("GET request\n");		
		reply = generate_html_str(reply, df);
		send(*fd, reply, strlen(reply), 0);
		close(*fd);
		printf("Server closed connection\n");
	}

	/* 2. this is NOT a GET request --> parse user input from POST request: */
	else {
		int action = 0;
		int col_index = 9;
		int calc_num = 9;
		int sort_direct_num = 9;
		float filter_threshold;
		int filter_compare_num = 9;

		/* extracts what "action" to perform from POST http request */
		char* action_str =  strstr(request, "action");
		if (action_str) {
			action = atoi(&action_str[7]);
		}
		/* extracts which Column to sort from POST http request */
		char* col_str = strstr(request, "column");
		if (col_str) {
			col_index = atoi(&col_str[7]);
		}
		/* extracts whether to sort in ascending or descending order */
		char* sort_direct_str = strstr(request, "sort_direction");
		if (sort_direct_str) {
			sort_direct_num = atoi(&sort_direct_str[15]);
		}
		/* extracts the filter threshold value from the POST http request */
		char* filter_str = strstr(request, "filter_val");
		if (filter_str) {
			filter_threshold = atof(&filter_str[11]);
		}
		/* extracts filter equality */
		char* filter_compare_str = strstr(request, "compare");
		if (filter_compare_str) {
			filter_compare_num = atoi(&filter_compare_str[8]);
		}
		/* extracts Calculation info from POST http request */
		char* calc_str = strstr(request, "operation");
		if (calc_str) {
			calc_num = atoi(&calc_str[10]);
		}

		 
		switch(action) {
			case 1: // this is a sort request
				reply = sort_request(col_index, sort_direct_num, reply, df);
				printf("called sort_request fxn\n");
				break;
			case 2: // this is a filter request
				reply = filter_request(filter_threshold, filter_compare_num, reply, df);
				printf("called filter_request fxn\n");
				break;
			case 3: // this is a calculation request
				reply = calculate_request(calc_num, reply, df);
				printf("called calculate_request fxn\n");
				break;
		}
		send(*fd, reply, strlen(reply), 0);
		close(*fd);
		printf("Server closed connection\n");

		

	}
	/* free all thread data on the heap */
	if(df) {
		free_data_frame_struct(df);
		if(df) free(df);
	}
	free(fd);
	if(reply) {
		free(reply);
	}
	pthread_mutex_lock(&thr_count);
  	flag_thr_count--;
  	pthread_mutex_unlock(&thr_count);

  	pthread_exit(NULL);
	return NULL;
}





/*
 * Processes & executes SORT request --> sends reply
 */
char* sort_request(int column, int direction, char* html_str, data_frame* df) {
	char* specs = NULL;
	char* col_name;
	char* sort_order;

	///////////////////////////////////////
	//// for testing multi threading:
	printf("This is a sort request --> putting thread to sleep for 10 seconds\n");
	sleep(10);
	///////////////////////////////////////

	specs = malloc(sizeof(char) * 200);
	if (!specs) printf("sort request malloc failed\n");

	if (direction == 0) sort_order = "DESCENDING";
	else sort_order = "ASCENDING";

	if (column == 1) {
		col_name = "Course Name";

		if (direction == 0) {
			sort_by_course_name_descending(df);
		}
		else {
			sort_by_course_name_ascending(df);
		}
		
	}
	else {
		col_name = "Course Quality";
		if (direction == 0) {
			sort_by_course_quality_descending(df);
		}
		else {
			sort_by_course_quality_ascending(df);
		}

	}

	sprintf(specs, "\n<br>\n<h2>Table SORTED by %s in %s order </h2>\n", col_name, sort_order);
	// printf("%s\n", specs);
	html_str = generate_output_html(html_str, specs, df);
	free(specs);

	return html_str;
}





/*
 * Processes & executes FILTER request --> sends reply
 */
char* filter_request(float threshold, int comparison, char* html_str, data_frame* df) {
	// pass in a double pointer data_fram**
	char* specs = NULL;
	char* compare_str;

	printf("threshold = %f\n", threshold);

	specs = malloc(sizeof(char) * 200);
	if (!specs) printf("sort request malloc failed\n");

	switch(comparison) {
		case 0:
			compare_str = "<";
			break;
		case 1:
			compare_str = "<=";
			break;
		case 2:
			compare_str = "=";
			break;
		case 3:
			compare_str = ">=";
			break;
		case 4:
			compare_str = ">";
			break;
	}


	
	// create return data frame
	data_frame* new_df = NULL;

	// <
	if (comparison == 0) {
		new_df = filter_by_course_quality_less(df, threshold);

	}

	// <=
	else if (comparison == 1) {
		new_df = filter_by_course_quality_less_equal(df, threshold);

	}

	// ==
	else if (comparison == 2) {
		new_df = filter_by_course_quality_equal(df, threshold);

	}

	// >=
	else if (comparison == 3) {
		new_df = filter_by_course_quality_greater_equal(df, threshold);

	}

	// >
	else if (comparison == 4) {
		new_df = filter_by_course_quality_greater(df, threshold);

	}

	else {
		new_df = filter_by_course_quality_equal(df, threshold);
		printf("Something went wrong!\n");
	}



	

	sprintf(specs, "\n<br>\n<h2>Table FILTERED: Course Quality %s %0.2f </h2>\n", compare_str, threshold);

	html_str = generate_output_html(html_str, specs, new_df);
	free(specs);

	return html_str;
}





/*
 * Processes & executes CALCULATION request --> sends reply
 */
char* calculate_request(int calc, char* html_str, data_frame* df) {
	char* specs = NULL;
	char* calc_str;
	float ret = 0.0;


	specs = malloc(sizeof(char) * 200);
	if (!specs) printf("sort request malloc failed\n");

	switch(calc) {
		case 0:
			calc_str = "Find Min";
			calculate_min_course_quality(df, &ret);
			break;

		case 1:
			calc_str = "Find Max";
			calculate_max_course_quality(df, &ret);		
			break;

		case 2:
			calc_str = "Compute Average";
			calculate_average_course_quality(df, &ret);	
			break;
	}


	sprintf(specs, "\n<br>\n<h2>Course Quality calculation performed = %s \n \t Result = %f </h2>\n", calc_str, ret);

	html_str = generate_output_html(html_str, specs, df);
	free(specs);


	return html_str;
}








