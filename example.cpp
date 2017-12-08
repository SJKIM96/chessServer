#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include "socketserver.h"

using namespace std;
//semephore
mutex mtx;
string coordinate "0000";
int turn =3;

void turn (socket* soc, int n){
    bool continues = true;
    ByteArray data;
    while (continues){
        //lock till get a turn
        mtx.lock();
        //if this thread is same as last turn thread unlock other thread
        //write to client
        if (turn != n){
            //send init to client coordinate
            //special case 1: when player 1 plays first, send code 0000 to unblock waiting
            //and wait to get coordinate from player 1
            turn = n;
            //send coordinate to client 
            data = ByteArray(coordinate);
            soc->Write(data);
            //get response coordinate before 
            soc->Read(data);
            //save the coordinate
            coordinate = data.ToString();            
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
    SocketServer server(3000);
    Socket* whitesocket;
    socket* blacksocket;
    int count =0;
    while (true){
        // Wait for a client socket connection
        Socket* newConnection = new Socket(server.Accept());
        
        if (count == 0){
            whitesocket =newConnection;
            count++;
            ByteArray msg = ByteArray("wait for p2");
            whitesocket->Write(msg);
        }
        else if (count ==1){
            blacksocket = newConnection;
            count++;
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
    thread th1 (turn,whitesocket,1);
    thread th2 (turn,blacksocket,2);

    //termination
    th1.join();
    th2.join();
    cout <<"exiting the game" <<endl;
    return 0;
}