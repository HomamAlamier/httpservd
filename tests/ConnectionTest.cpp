#include <iostream>
#include <stdlib.h>
#include <thread>

#include <Web/HttpRequest.h>
#include <Network/Socket.h>
#include <Core/ByteArray.h>



const char* get_req = "GET / HTTP/1.1\r\n\r\n";
static long last_time = 0;

pthread_mutex_t mutex;


void start_stop_watch()
{
    last_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
long stop_stop_watch()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - last_time;
}

bool createNewConnection()
{
    Socket sock(Socket::Stream, Socket::TCP);
    if (sock.Connect(IPEndPoint("127.0.0.1", 8089, IPEndPoint::IPv4)))
    {
        sock.Write(ByteArray(get_req, 19));
        ByteArray resp;
        int c = sock.Read(&resp, 1024);
        if (c > 0)
        {
            sock.close();
            return true;
        }
    }
    sock.close();
    return false;
}

void connectionsPatch(int count, int microSec)
{
    std::thread** pool = new std::thread*[count];
    std::vector<bool> st;
    int f = 0;
    for(int i = 0; i < count; ++i)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(microSec));
        pool[i] = new std::thread([&] {
            bool x = createNewConnection();

            pthread_mutex_lock(&mutex);
            st.push_back(x);
            f++;
            pthread_mutex_unlock(&mutex);
            std::cout << "thread(" << std::this_thread::get_id() << ") : finished ! " << (x ? "True" : "False") << std::endl;
        });
    }


    while(f < count)
    {
        if (f == count - 1)
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::cout << "Finished: " << std::to_string(f) << std::endl;
    }
    std::cout << "Connection Report: ------------" << std::endl;
    int fail = 0, success = 0;
    for(int i = 0; i < count; ++i)
    {
        if (st[i]) success++;
        else fail++;
    }
    std::cout << "Success: " << std::to_string(success) << std::endl;
    std::cout << "Failed: " << std::to_string(fail) << std::endl;

}

int main()
{

    pthread_mutex_init(&mutex, NULL);
    while(true)
    {
        std::cout << "Enter connection patch count : ";
        std::string ln;
        std::getline(std::cin, ln);        
        std::cout << "Enter between connections time(micro second) : ";
        std::string ln2;
        std::getline(std::cin, ln2);
        if (ln != "")
        {
            if (ln == "x")
                return 0;
            int x = atoi(ln.c_str());
            int x2 = atoi(ln2.c_str());
            start_stop_watch();
            connectionsPatch(x, x2);
            long clk1 = stop_stop_watch();
            std::cout << ln << " patch completed in " << std::to_string(clk1) << "ms" << std::endl;
        }
    }
}