/* 
   File: simpleclient.C

   Author: R. Bettati
   Department of Computer Science
   Texas A&M University
   Date  : 2013/01/31

   Simple client main program for MP3 in CSCE 313
*/

/* Refferd to the code of alan chtenberg at https://github.com/alanachtenberg/CSCE-313/blob/master/MP3/client.C */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <iomanip>
#include <sys/time.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include <time.h>

#include <vector>
#include <map>

#include "reqchannel.h"
#include "BoundedBuffer.h"
#include "semaphore.h"


using namespace std;

// Globals
int n = 10000; // number of request threads
int w = 50; // number of worker threads
int b = 100; //buffer size
int c;

int joe_req_ct = 0;
int jane_req_ct = 0;
int john_req_ct = 0;

map<int, int> joe_hist;
map<int, int> jane_hist;
map<int, int> john_hist;

BoundedBuffer* main_buf;

BoundedBuffer* joe_buf;
BoundedBuffer* jane_buf;
BoundedBuffer* john_buf;

/*pthread_t joe_req;
pthread_t jane_req;
pthread_t john_req;
	
pthread_t joe_stat;
pthread_t jane_stat;
pthread_t john_stat;
*/
	
void show_hist (map<int, int> m, string name){
  cout << name << "'s histogram" << endl;
  for (auto x : m) {
    cout << '(' << x.first << ", " << x.second << ')' << endl;
  }
}
	
void* req_thread (void* person){
  int requester = *((int*) person);
  string delivery;

  for(int i = 0; i < n; ++i) {
      if(requester == 0) {
	++joe_req_ct;
	delivery = "data joe";
      }
      else if (requester == 1) {
	++jane_req_ct;
	delivery = "data jane";
      }
      else if (requester == 2) {
	++john_req_ct;
	delivery = "data john";
      }
				
      main_buf->add(delivery);
  }
  cout << "Requester " << requester << " is done." << endl;
}

void* worker_thread(void* req_channel){
  cout << "in worker thread" << endl;
  RequestChannel* chan = (RequestChannel*) req_channel;
  string removal;
  string to_add;

  while(true){
    removal = main_buf->remove();

    if(removal == "stop"){
      break; ///neeeeeeeeeeeeeeeeeeeeeeeeeeeeeded?
    }		

    cout << "sending: " << removal << " to server" << endl;
    to_add = chan->send_request(removal);
    
    if (removal == "data joe") {joe_buf->add(to_add);}
    else if (removal == "data jane") {jane_buf->add(to_add);}
    else if (removal == "data john") {john_buf->add(to_add);}
  }

  chan->send_request("quit");
}
	
void* stat_thread(void* person_id){
  int req_id = *((int*)person_id);
  string removal;
  for(int i = 0 ; i < n ; ++i){
    if(req_id == 0){
      removal = joe_buf->remove();

      ++joe_hist[atoi(removal.c_str())];
    }
    else if(req_id == 1){
      removal = jane_buf->remove();

      ++jane_hist[atoi(removal.c_str())];
    }
    else if(req_id == 2){
      removal = john_buf->remove();

      ++john_hist[atoi(removal.c_str())];
    }
	
  }
  cout << "stat thread finished : " << req_id << endl;
}
	
/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
  opterr = 0;
  
  while ((c = getopt(argc, argv, "n:w:b:")) != -1) {
    switch(c) {
    case 'n':
      n = atoi(optarg);
      break;
    case 'w':
      w = atoi(optarg);
      break;
    case 'b':
      b = atoi(optarg);
      break;
    default:
      cout << "flag H for help" << endl;
      cout << "flag n for num req threads" << endl;
      cout << "flag w for num worker threads" << endl;
      cout << "flag b for buf size" << endl;
      abort();
    }
  }

  // App code here
  main_buf = new BoundedBuffer(b);
  joe_buf = new BoundedBuffer(b/3); // b/3 noted on piazza
  jane_buf = new BoundedBuffer(b/3);
  john_buf = new BoundedBuffer(b/3); 

  pthread_t joe_req;
  pthread_t jane_req;
  pthread_t john_req;
	
  pthread_t joe_stat;
  pthread_t jane_stat;
  pthread_t john_stat;

  pthread_t workers[w];
  
  int* joe_id = new int(0); 
  int* jane_id = new int(1);
  int* john_id = new  int(2);
  
  const char **farg = new const char*[0]; // nothing in here
  int pid = fork();
  if (pid != 0 && pid != -1) {execv("./dataserver", (char**) farg);}
  
  else {
    cout << "Client started" << endl;
    cout << "Establishing control channel... " << endl; 
    cout << "Going to check if hello succeeds...\n" << flush;

    RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
    string reply = chan.send_request("hello");

    cout << reply << endl;
    cout << "done" << endl << endl;;

    cout << "Beginning creation of joe's threads" << endl;
    pthread_create(&joe_req, NULL, req_thread, (void*) joe_id);
    pthread_create(&joe_stat, NULL, stat_thread, (void*) joe_id);
    cout << "Finished making joe's threads" << endl;

    cout << "Beginning creation of jane's threads" << endl;
    pthread_create(&jane_req, NULL, req_thread, (void*) jane_id);
    pthread_create(&jane_stat, NULL, stat_thread, (void*) jane_id);
    cout << "Finished making jane's threads" << endl;

    cout << "Beginning creation of john's threads" << endl;
    pthread_create(&john_req, NULL, req_thread, (void*) john_id);
    pthread_create(&john_stat, NULL, stat_thread, (void*) john_id);
    cout << "Finished making john's threads" << endl;

    cout << "Beginning to create the " << w << " worker threads." << endl;
    for (int i = 0; i < w; ++i) {
      string req_name = chan.send_request("newthread");
      RequestChannel* chan2 = new RequestChannel(req_name, RequestChannel::CLIENT_SIDE);
      pthread_create(&workers[i], NULL, worker_thread, (void*) chan2);
    }
    cout << "Finished making the worker theads." << endl;


    //pthread_join(joe_req, NULL);
    //pthread_join(jane_req, NULL);
    //pthread_join(john_req, NULL);

    for (int i = 0; i < w; ++i) {
      main_buf->add("stop");
    }

    // Begin joining all threads
    for (int i = 0; i < w; ++i) {
      pthread_join(workers[i], NULL);
    }

    pthread_join(joe_req, NULL);
    pthread_join(jane_req, NULL);
    pthread_join(john_req, NULL);

    pthread_join(joe_stat, NULL);
    pthread_join(jane_stat, NULL);
    pthread_join(john_stat, NULL);

    // Finished
    chan.send_request("quit");
    usleep(10000);

    // Show stats
    cout << "showing histograms" << endl;
    show_hist(joe_hist, "Joe");
    show_hist(jane_hist, "Jane");
    show_hist(john_hist, "John");

    return 0;
  }
  return -1;
}
