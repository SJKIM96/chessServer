#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include "socketserver.h"

using namespace std;
using namespace Sync;
//semephore
mutex mtx;
string coordinate = "0000";
//player 2 cannot go first so initiate to p2 that went last.
int previousID =2;

void turn (Socket* soc, int n){
    
    bool continues = true;
    ByteArray data;
    while (continues){
        //lock till get a turn
        mtx.lock();

        //if this thread is same as last turn thread unlock other thread
        //write to client
        if (previousID != n){
            cout <<"player: " << n <<" going" <<endl;
            //send init to client coordinate
            //special case 1: when player 1 plays first, send code 0000 to unblock waiting
            //and wait to get coordinate from player 1
            previousID = n;
            //send coordinate to client 
            data = ByteArray(coordinate);
            soc->Write(data);
            //get response coordinate before 
            soc->Read(data);
            
            //save the coordinate
            coordinate = data.ToString();  
            cout <<"coordinate: " << coordinate << endl;         
        }
    
        
        // exit condition: king dies, player leaves and client sends done before closing connection
        //exit when its done
        if (coordinate == "done")
            continues = false;
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
            ByteArray msg = ByteArray("wait for p2");
            whitesocket->Write(msg);
        }
        else if (count ==1){
            blacksocket = newConnection;
            count++;
            cout <<"client 2 recieved" << endl;
            // ByteArray msg = ByteArray("rdy");
            // whitesocket->Write(msg);
            // blacksocket->Write(msg);
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
 
    //termination
    th1.join();
    th2.join();
    cout <<"exiting the game" <<endl;
    return 0;
}