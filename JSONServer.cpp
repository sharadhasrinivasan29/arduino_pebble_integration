/* 
This code primarily comes from 
http://www.prasannatech.net/2008/07/socket-programming-tutorial.html
and
http://www.binarii.com/files/papers/c_sockets.txt
 */


/* TO DO:
Handle middle ware issues
Do q for quit
*/
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <pthread.h>
using namespace std;



// char keyPass;
char key;
string reply;
void* quitFunction(void* ptr);
void configure(int fd);
int keep_looping;

/*
Function to configure Arduino with the JSON Server
*/
void configure(int fd) {
  struct  termios pts;
  tcgetattr(fd, &pts);
  cfsetospeed(&pts, 9600);   
  cfsetispeed(&pts, 9600);   
  tcsetattr(fd, TCSANOW, &pts);
}

/*
Main function. This has embedded in it, the start server, 
as well as functions to read from and write to the Arduino!
*/
int main(int argc, char *argv[]){
    pthread_t quitThread;
    // check the number of arguments
    if (argc != 3)
      {
        cout << endl << "Usage: server [port_number], Name of the serial port (USB) device file " << endl;
        exit(0);
      }
   
    int PORT_NUMBER = atoi(argv[1]);

    int fd1 = open(argv[2], O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);

    if (fd1 < 0) {
      perror("Could not open file");
      exit(1);
    }
    else {
      cout << "Successfully opened " << argv[2] << " for reading/writing" << endl;
    }

    configure(fd1);
    


    /*
    START SERVER FUNCTION
    */


    // structs to represent the server and client
    struct sockaddr_in server_addr,client_addr;    
    
    int sock; // socket descriptor

    // 1. socket: creates a socket descriptor that you later use to make other system calls
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket");
        exit(1);
    }
    int reply;
    if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&reply,sizeof(int)) == -1) {
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

    // 3. listen: indicates that we want to listn to the port to which we bound; second arg is number of allowed connections
    if (listen(sock, 5) == -1) {
        perror("Listen");
        exit(1);
    }
        
    // once you get here, the server is set up and about to start listening
    cout << endl << "Server configured to listen on port " << PORT_NUMBER << endl;
    fflush(stdout);
   
    keep_looping = 1;
    int count = 0;

    int r = pthread_create(&quitThread, NULL, &quitFunction, &keep_looping);
    while(keep_looping == 1) {
      // 4. accept: wait here until we get a connection on that port
      int sin_size = sizeof(struct sockaddr_in);
      int fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);
      cout << "Server got a connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << endl;
      
      // buffer to read data into
      char request[1024];
      
      // 5. recv: read incoming message into buffer
      int bytes_received = recv(fd,request,1024,0);
      // null-terminate the string
      request[bytes_received] = '\0';
      cout << "Here comes the message:" << endl;
      cout << request << endl;

      // for(int i = 5; i < 7; i++) {
      //   key += request[i];
      // }
      key = request[5];
      cout << "Key is: " << key << endl << endl; 

      int returnVal = write(fd1,&key, 1);
      
      // 6. send: send the message over the socket
      // note that the second argument is a char*, and the third is the number of chars
      
      // CODE TO READ & PARSE STUFF FROM ARDUINO

        char buff[100];
        int bytes_read = read(fd1, buff, 100);
        string reply;
        
        if(returnVal == -1) {
          cout << "Arduino failed" << endl;
          string disconnect = "Arduino Disconnected!";
          reply = "{\n\"name\": \"" + disconnect +"\0" + "\"\n}\n";
          cout << "fd1 : " << fd1 << endl;
          fd1 = open(argv[2], O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
          cout << "fd1 after : " << fd1 << endl;
        }

        else {
          if(key == '1') {
            //Farenheit
            string correct_reply = "Fahrenheit set.";
            reply = "{\n\"name\": \"" + correct_reply +"\0" + "\"\n}\n";
          }

          else if(key == '2') {
            //Celsius
            string correct_reply = "Celsius set.";
            reply = "{\n\"name\": \"" + correct_reply +"\0" + "\"\n}\n";
          }
          else if(key == '6') {
            //standby
            string print = "Entering Standby Mode";
            reply = "{\n\"name\": \"" + print +"\0" + "\"\n}\n";
          }
          else if(key == '7') {
            string correct_reply;
            int i = 0;
            if(bytes_read < 0) {

            }
            else {
                for(i = 0; i < bytes_read; i++) {
                  correct_reply += buff[i] ;
                }
            }
            
            if(correct_reply == "") {
              correct_reply = " ";
            }
            reply = "{\n\"name\": \"" + correct_reply +"\0" + "\"\n}\n";
          }
          else if(key == 'c') {
            string correct_reply;
            int i = 0;
            if(bytes_read < 0) {

            }
            else {
                for(i = 0; i < bytes_read; i++) {
                  correct_reply += buff[i] ;
                }
            }
            
            if(correct_reply == "") {
              correct_reply = " ";
            }
            reply = "{\n\"name\": \"" + correct_reply +"\0" + "\"\n}\n";
          }
          else if(key == '8') {
            //print some word to 7SEG
            string print = " ";
            reply = "{\n\"name\": \"" + print +"\0" + "\"\n}\n";
          }
          else if(key == 'e') {
            //print some word to 7SEG
            string print = " ";
            reply = "{\n\"name\": \"" + print +"\0" + "\"\n}\n";
          }
          else if(key == 'f') {
            //print some word to 7SEG
            string print = " ";
            reply = "{\n\"name\": \"" + print +"\0" + "\"\n}\n";
          }
          else if(key == 'h') {
            //print some word to 7SEG
            string print = " ";
            reply = "{\n\"name\": \"" + print +"\0" + "\"\n}\n";
          }
          else if(key == 'i') {
            //print some word to 7SEG
            string print = " ";
            reply = "{\n\"name\": \"" + print +"\0" + "\"\n}\n";
          }
          else if(key == 'j') {
            //print some word to 7SEG
            string print = " ";
            reply = "{\n\"name\": \"" + print +"\0" + "\"\n}\n";
          }
          else if(key == 'l') {
            //print some word to 7SEG
            string print = " ";
            reply = "{\n\"name\": \"" + print +"\0" + "\"\n}\n";
          }
          else if(key == 'm') {
            //print some word to 7SEG
            string print = " ";
            reply = "{\n\"name\": \"" + print +"\0" + "\"\n}\n";
          }
          else if(key == 'n') {
            //print some word to 7SEG
            string print = " ";
            reply = "{\n\"name\": \"" + print +"\0" + "\"\n}\n";
          }
          else if(key == '9') {
            //click for green light/comfy reply
            string print = "Perfect..Green light turning on!";
            reply = "{\n\"name\": \"" + print +"\0" + "\"\n}\n";
          }
          else if(key == 'a') {
            //click for blue light/too cold
            string print = "Brrr..Blue light turning on!";
            reply = "{\n\"name\": \"" + print +"\0" + "\"\n}\n";
          }
          else if(key == 'b') {
            //click for red light/too hot
            string print = "Ooff..Red light turning on!";
            reply = "{\n\"name\": \"" + print +"\0" + "\"\n}\n";
          }
          else if(key == 'd') {
            //standby
            string print = "Exiting Standby Mode";
            reply = "{\n\"name\": \"" + print +"\0" + "\"\n}\n";
          }

          else{
            //if pebble asks for just the current replyerature
            string correct_reply;
            int i = 0;

            if(bytes_read < 0) {

            }
            else {
                for(i = 0; i < bytes_read; i++) {
                  correct_reply += buff[i] ;
                }
                if(buff[2] != '.') {
                  correct_reply = "Click Again";
                }
            }

            if(correct_reply == "") {
                correct_reply = " ";
            }

            reply = "{\n\"name\": \"" + correct_reply  +"\0" + "\"\n}\n";
            buff[0] = '\0';
          }

        }
      // reply = "{\n\"name\": \"" + correct_reply  +"\0" + "\"\n}\n";
      cout << "Sending reply to pebble" << endl;
      send(fd, reply.c_str(), reply.length(), 0);
      printf("Server sent message: %s\n", reply.c_str());

      // buff[0] = '\0';

      // 7. close: close the socket connection
      close(fd);
    }
    
    close(sock);
    cout << "Server closed connection" << endl;
}

/*
* This function attempts to quit the program gracefully, when user enters q in the console
* Called in a thread in startserver
*/
void* quitFunction(void* ptr){
    char quitLetter;
    int keepLooping = 1;

    while(keepLooping == 1){
        cin >> quitLetter;
        if(quitLetter == 'q' || quitLetter == 'Q'){
            keepLooping = 0;
            keep_looping = 0;
        }
    }

    *((int*)ptr) = 0;

    pthread_exit(ptr);
}
