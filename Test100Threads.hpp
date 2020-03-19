/************************************************************************************************************************
 * Test and demo 100 threads sending messages to 1 thread.																*
 * Each sending thread would send messages at the speed of 10 messages every 10ms (The time accuracy is OS dependent).	*
 * Main thread would wait until 128*1024 messages and then send a termination message back to each thread.				*
 ************************************************************************************************************************/

#include "ITC.h"
#include <thread>
#include <vector>
#include <map>

using namespace std;

namespace Test100Threads{
	
	constexpr size_t threadCount = 100;
	constexpr size_t messageLimit = 128*1024;
	
	//Send 10 messages every 10 milliseconds,
	//Quit when a message is received.
	void send_thread(shared_ptr<ITC<size_t>::FixedSocket> socket){
		size_t i = 0;
		while(true) {
			for(size_t j=0; j<10; ++j) {
				size_t message = i+j;
				socket->send_message(message);
			}
			i += 10;
			if(socket->has_message()){
				break;
			}
			this_thread::sleep_for(chrono::milliseconds(10));
		}
	}
	
	//Create a socket for main thread to receive messages.
	//Create multiple threads defined by threadCount to generate messages back to main thread
	void run(){
		ITC<size_t> anItc;
		auto mainSocket = anItc.create_socket(0);
		//Create the threads {
		vector<thread*> theThreads;
		for(size_t i=1; i<=threadCount; ++i) {
			theThreads.push_back(new thread(send_thread, anItc.create_fixed_socket(i, 0)));
		}
		/////////////////////}
		
		//Listen to messages from the threads{
		map<size_t, size_t> messagesMap;
		size_t totalMessages = 0;
		while(totalMessages < messageLimit) {
			if(mainSocket->wait_message()) {
				auto input = mainSocket->get_message();
				messagesMap[input.sourceSocketId] = input.message;
				++totalMessages;
			}
			else{
				printf("Wait message failed.\n");
				break;
			}
		}
		/////////////////////////////////////}
		
		//Send a message to each thread, the threads would each quit upon receiving a message{
		for(size_t i=1; i<=threadCount; ++i) {
			size_t message = 0;
			mainSocket->message_to_socket(i, message);
		}
		/////////////////////////////////////////////////////////////////////////////////////}
		
		//print out total messages received from each thread {
		for(auto i: messagesMap) {
			printf("Socket %03u total messages %u\n", i.first, i.second);
		}
		/////////////////////////////////////////////////////}
		
		//Clean up {
		for(auto i : theThreads) {
			i->join();
			delete i;
		}
		///////////}
	}
}
