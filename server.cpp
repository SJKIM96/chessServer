#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include "socketserver.h"

using namespace std;
using namespace Sync;
//semephore
mutex mtx;
//coordinate structure is (iy,ix,fy,f)x
string coordinate = "0";
//player 2 cannot go first so initiate to p2 that went last.
int previousID =2;

void turn (Socket* soc, int n){
    
    bool continues = true;
    ByteArray data;
    while (continues){
        //lock till get a turn
        mtx.lock();

		// exit condition: king dies, player leaves and client sends done before closing connection
		//exit when its done
		if (coordinate == "done"){
			mtx.unlock();
			break;
		}
		
        //if this thread is same as last turn thread unlock other thread
        //write to client
        else if (previousID != n){
            cout <<"player: " << n <<" going ";
            //send init to client coordinate
            //special case 1: when player 1 plays first, send code 0 to unblock waiting
            //and wait to get coordinate from player 1
            previousID = n;

            //send coordinate to client 
            data = ByteArray(coordinate);
            soc->Write(data);
            //get response coordinate before 
            soc->Read(data);
            
            //save the coordinate
            coordinate = data.ToString();  
            cout <<"with coordinate: " << coordinate << endl;         
        }

        
        //signal other thread
        mtx.unlock();
    }
}

int main()
{
    SocketServer server(2005);
    Socket* whitesocket;
    Socket* blacksocket;
    int count =0;
    cout <<"Starting game, waiting for player" << endl;
    while (true){
        // Wait for a client socket connection
        Socket* newConnection = new Socket(server.Accept());
        
        if (count == 0){
            whitesocket =newConnection;
            count++;
            cout << "client 1 recieved" << endl;
            ByteArray msg = ByteArray("1");
			whitesocket->Write(msg);
        }
        else if (count ==1){
            blacksocket = newConnection;
            count++;
            cout <<"client 2 recieved" << endl;
            ByteArray msg = ByteArray("2");
			
			blacksocket->Write(msg);
			break;
        }
        else {
            cout <<"too many player" << endl;
            newConnection->Close();
        }
    }
    //start game
    //cout <<"starting game" <<endl;
    std::thread th1 (turn, whitesocket, 1);
    std::thread th2 (turn, blacksocket, 2);
	
	th1.join();
	th2.join();
    //termination
	cout << "closing thead 1" << endl;
	cout << "closing thread 2" << endl;
	
	
	
	
    cout <<"exiting the game" <<endl;
	
	cout << "closing socket" << endl;
	whitesocket->Close();
	blacksocket->Close();
    return 0;
}