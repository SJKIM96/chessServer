#include "socket.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>

using namespace Sync;
using namespace std;
int main(void)
{
	// int portNum;
	// cout <<"which port to connect: " ;
	// cin >>portNum;

	Socket * socket = new Socket("127.0.0.1", 2005);

	//try to open that port 
	socket->Open();

	string userInput;
	while (true){
        ByteArray reader;
        socket->Read(reader);
        if (reader.ToString() == "wait for p2"){
            cout <<"waiting for p2" << endl;  
            socket->Read(reader);
        }
        cout <<"response: " <<reader.ToString() <<endl;
    

		cout <<"send coordinate: ";
		cin >> userInput;
		
		ByteArray sender(userInput);
		socket->Write(sender);
        if (userInput == "done")
            break;
	}
	socket->Close();
	
}
