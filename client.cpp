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

#include <errno.h>
#include <unistd.h>

#include "reqchannel.h"
#include "Bouded_buffer.H"
using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/
	int n = 10000; // number of request
	int w = 100; // number of worker threads
	int b = 500; //buffer size
	
	int histogram_size = 10000;
	
	vector<int> histo_joe(histogram_size);//histogram to store data
	vector<int> histo_jane(histogram_size);
	vector<int> histo_john(histogram_size);

	int joe_req =0;
	int jane_req =0;
	int john_req=0;
	
	BoundedBuffer* common_buffer; // buffer to hold user requests
	BoundedBuffer* joe_buffer;
	BoundedBuffer* jane_buffer;
	BoundedBuffer* john_buffer;
		

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    int* joe=new int(0); // to differnce between thread owner
	int* jane = new int(1);
	int* jonh = new int(2);
	

/*--------------------------------------------------------------------------*/
/* FUNCTIONS */
/*--------------------------------------------------------------------------*/

    void show_histogram (vector<int> data, string name){
		/*int low, high;
		cout<<"Enter the range you want";
		cin>>low>>high;
		if(high>histogram_size || low<0){
			cout<<"range out of bounds, enter again";
			cin>>low>>high;
		}*/
		
		for(int i=0 ;i<histogram_size ; ++i )
			cout<<i<<" - "<<data[i]<<"  ";
		cout<<endl<<endl;
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
				
		common_buffer -> add(str);
			}
		cout << "All requests completed for request_id="<< req_id<< endl;
		cout<<"Request thread exiting\n";
	}

	/*void* worker_thread(void* req_channel){
		RequestChannel* channel = (RequestChannel*) req_channel;
		string str1;
		int counter =0;
		while(1){
			str1 = common_buffer -> remove();
			
			if(str1 == "kill"){
				break;
			}
			
			string reply = channel -> send_request(str1);
			str1 = reply;
			
			
			
			
			
		}
	}*/
	
	void* stat_thread(void* person_id){
		int req_id = *((int*)person_id);
		string r;
		for(int i=0 ; i<n ; ++i){
			if(req_id ==0){
				r=joe_buffer->remove();
				histo_joe[atoi(r.c_str())]+=1;
				
			}
			else if(req_id==1){
				r=jane_buffer->remove();
				histo_jane[atoi(r.c_str())]+=1;
			}
			else if(req_id==2){
				r=john_buffer->remove();
				histo_jane[atoi(r.c_str())]+=1;
			}
	
		}
		cout<<"stat thread finished : "<<req_id<<endl;
	}
	
/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {

  cout << "CLIENT STARTED:" << endl;

  cout << "Establishing control channel... " << flush;
  RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
  cout << "done." << endl;;

  /* -- Start sending a sequence of requests */

  string reply1 = chan.send_request("hello");
  cout << "Reply to request 'hello' is '" << reply1 << "'" << endl;

  string reply2 = chan.send_request("data Joe Smith");
  cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << endl;

  string reply3 = chan.send_request("data Jane Smith");
  cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << endl;

  string reply5 = chan.send_request("newthread");
  cout << "Reply to request 'newthread' is " << reply5 << "'" << endl;
  RequestChannel chan2(reply5, RequestChannel::CLIENT_SIDE);

  string reply6 = chan2.send_request("data John Doe");
  cout << "Reply to request 'data John Doe' is '" << reply6 << "'" << endl;

  string reply7 = chan2.send_request("quit");
  cout << "Reply to request 'quit' is '" << reply7 << "'" << endl;

  string reply4 = chan.send_request("quit");
  cout << "Reply to request 'quit' is '" << reply4 << "'" << endl;

  usleep(1000000);
}
