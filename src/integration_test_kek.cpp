#include <include/queue.h>
#include <include/wavfile.h>
#include <vector>
#include <thread>
#include <cstdint>

#include <iostream>
#include <fstream>
#include <queue>
#include <array>
#include <memory>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <mutex>

#define MUL_MEGABYTE (1024*1024)

using namespace std;

//struct StatePtr {
//    short* ptr = nullptr;
//    bool terminate = true;
//};
//
//struct StateArrPtr {
//    unique_ptr<short[]> ptr;
//    streamsize size;
//};
atomic_bool isTerminated(false);

mutex mu;
WavHeader shared_header;
condition_variable cond;

void read_in_queue(ConcurrentQueue<vector<int16_t>>& q) {
    vector<int16_t> buffer(MUL_MEGABYTE);
    WavFileReader in("D:/Movies/twiswethuist.wav", MUL_MEGABYTE);
    shared_header = in.getHeader();
    while (!(isTerminated = !bool(in.read(buffer)))) {
        q.push(buffer);
        buffer = vector<int16_t>(MUL_MEGABYTE);
        if (q.size() >= 5) {
            unique_lock<mutex> locker(mu);
            cout << "I'ma waiting" << endl;
            cond.wait(locker, [&]() { return q.size() < 3; });
            cout << "I'ma  NOT waiting" << endl;
            locker.unlock();
        }
    }
}

void write_from_queue(ConcurrentQueue<vector<int16_t>>& q) {
    int count = 0;
    vector<int16_t> a;
    WavFileWriter out("D:/MCKunda/Desktop/ITESTLEL.wav");
    WavFileReader* in = new WavFileReader("D:/Movies/twiswethuist.wav", MUL_MEGABYTE);
    out.getHeader() = in->getHeader();
    delete in;
    out.writeHeader();
    while (true) {
        if (isTerminated && q.empty()) break;
        a = move(q.pop());
        cout << "Got batch #" << ++count << " with size " << a.size() << " from thread #" << this_thread::get_id() << ": " << endl;
        out.write(a);
        a.clear();
        cond.notify_one();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

int main() {
    ConcurrentQueue<vector<int16_t>> q;
    auto start = std::chrono::high_resolution_clock::now();
    thread t1(read_in_queue, std::ref(q));
    thread t2(write_from_queue, std::ref(q));

    t1.join();
    t2.join();
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";

    return 0;
}