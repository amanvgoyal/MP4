// compile w/ g++ -std=c++11 -Wno-write-strings *.cpp  /usr/lib/x86_64-linux-gnu/libboost_program_options.so

// Skeleton from: 
//  http://www.radmangames.com/programming/how-to-use-boost-program_options

#include "boost/program_options.hpp" 

#include "semaphore.h"
#include "BoundedBuffer.h"
#include "reqchannel.h"

#include <iostream> 
#include <string> 
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include <vector>
#include <map>

namespace 
{ 
  const size_t ERROR_IN_COMMAND_LINE = 1; 
  const size_t SUCCESS = 0; 
  const size_t ERROR_UNHANDLED_EXCEPTION = 2; 
 
} 

// -------------------------------- Globals ----------------------------------

void* stat_thread(void* data) {
  BoundedBuffer b;
  b.add();
}

void* worker_thread(void* data) {

}

void* req_thread(void* data) {

}
 
int main(int argc, char** argv) 
{ 
  int n; // number of data requests per person
  int b; // size of bounded buffer in requests
  int w; // number of worker threads
  try 
    { 
      /** Define and parse the program options 
       */ 
      namespace po = boost::program_options; 
      po::options_description desc("Options"); 
      desc.add_options() 
	("help", "Print help messages")
	(",n", po::value<int>(&n)->required(),
	 "number of data requests per person")
	(",b", po::value<int>(&b)->required(), 
	 "Size of bounded bufffer in requests")
	(",w", po::value<int>(&w)->required(), 
	 "number of worker threads");
      
      po::variables_map vm; 
    try 
      { 
	po::store(po::parse_command_line(argc, argv, desc),  
		  vm); // can throw 
 
	/** --help option 
	 */ 
	if ( vm.count("help")  ) 
	  { 
	    std::cout << "MP1 options" << std::endl 
		      << desc << std::endl; 
	    return SUCCESS; 
	  } 

	po::notify(vm); // throws on error, so do after help in case 
	// there are any problems 
      } 
    catch(po::error& e) 
      { 
	std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
	std::cerr << desc << std::endl; 
	return ERROR_IN_COMMAND_LINE; 
      } 
 
    // ---------------------- application code here -----------------------
    const char **argv = new const char* [0]; // nothing in here
    //int server = execv("./dataserver", (char**) argv);
    int server = fork();
    if (server == 0) {execv("./dataserver", (char**) argv);}
    else {
      cout << "CLIENT STARTED:" << endl;

      cout << "Establishing control channel... " << flush;
      RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
      cout << "done." << endl;

      /* -- Start sending a sequence of requests */

      string reply1 = chan.send_request("hello");
      cout << "Reply to request 'hello' is '" << reply1 << "'" << endl;
    }

    }
  
  catch(std::exception& e) 
    { 
      std::cerr << "Unhandled Exception reached the top of main: " 
		<< e.what() << ", application will now exit" << std::endl; 
      return ERROR_UNHANDLED_EXCEPTION; 
 
    } 
 
  return SUCCESS; 
 
}
