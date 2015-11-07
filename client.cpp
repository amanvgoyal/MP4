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

map<int, int> joe_hist;
map<int, int> jane_hist;
map<int, int> john_hist;

BoundedBuffer* main_buf;
BoundedBuffer* joe_buf;
BoundedBuffer* jane_buf;
BoundedBuffer* john_buf;

pthread_t joe_req;
pthread_t jane_req;
pthread_t john_req;
	
pthread_t joe_stat;
pthread_t jane_stat;
pthread_t john_stat;
	
//int* joe=new int(0); // to differnce between thread owner
//int* jane = new int(1);
//int* jonh = new int(2);
	

void show_histogram (map<int, int> m, string name){
  cout << name << "'s histogram" << endl;
  for (auto x : m) {
    cout << '(' << x.first << ", " << x.second << ')' << endl;
  }
}
	
void* req_th (void* person_id){
  int req_id = *((int*) person_id);
  string str;
  for(int i=0; i<n ; i++)
    {
      if(req_id==0){
	john_req++;
	str="joe requested";
      }
      else if (req_id==1){
	jane_req++;
	str="jane requested";
      }
      else if (req_id==2)
	{
	  john_req++;
	  str="john requested";
	}
				
      main_buf->add(str);
    }
  cout << "All requests completed for request_id="<< req_id<< endl;
  cout<<"Request thread exiting\n";
}

void* worker_thread(void* req_channel){
  RequestChannel* channel = (RequestChannel*) req_channel;
  string str1;
  int counter =0;
  while(1){
    str1 = main_buf->remove();
			
    if(str1 == "kill"){
      break;
    }
			
    string reply = channel -> send_request(str1);
    str1 = reply;
  }
}
	
void* stat_thread(void* person_id){
  int req_id = *((int*)person_id);
  string r;
  for(int i = 0 ; i < n ; ++i){
    if(req_id == 0){
      r = joe_buf->remove();
      //histo_joe[atoi(r.c_str())]+=1;
      ++joe_hist[atoi(r.c_str())];
    }
    else if(req_id == 1){
      r = jane_buf->remove();
      //histo_jane[atoi(r.c_str())]+=1;
      ++jane_hist[atoi(r.c_str())];
    }
    else if(req_id == 2){
      r = john_buf->remove();
      //histo_jane[atoi(r.c_str())]+=1;
      ++john_hist[atoi(r.c_str())];
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
  const char **farg = new const char*[0]; // nothing in here
  int server = fork();
  if (server == 0) {execv("./dataserver", (char**) farg);}
  
  else {
    cout << "Client started" << endl;
    cout << "Establishing control channel... " << flush;
    RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
    cout << "done" << endl;
  }

}
