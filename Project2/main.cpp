#include "floatComparison.h"
#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <fstream>
#include <cmath>

//constants for appyl
constexpr int MAX_GIFT_PER_HACKER = 3;
constexpr int MINIMUM_VALID_COMMIT_LINE_NUMBER = 20;
constexpr int MINIMUM_COMIT_QUEUE = 3;
constexpr float epsilon = 0.00001f;

using namespace std;

// comparison mechanism for floats because floats are broken
bool largerFloat(float a, float b) { return a - b > epsilon; }
bool equalFloat(float a, float b) { return fabs(a - b) < epsilon; }
bool smallFloat(float a, float b) { return largerFloat(b, a); }

//struct for Hacker. variables are self explanatory.
struct Hacker
{
    int id, numberOfCommits=0, gift_number=0;
    float arrival_time, hoodie_entry_time = 0.0 , sticker_entry_time = 0.0, total_time_queue = 0.0;

    Hacker(int id) : id(id) {}
};

//struct for desks. variables are self explanatory.
struct desk
{
    int id, hackerAtDest;
    float service_time;
    bool is_empty = true;

    desk(int id, float service_time) :
         id(id), hackerAtDest(hackerAtDest), service_time(service_time) {}
};

//enum for events. improves readability and implementation.
// four types of events are implemented.
enum Events
{
    commit, stickerQueueEntry, stickerDeskLeave, hoodieDeskLeave
};

//struct for event. for every enum type there is a event. Event stores all the things that are affected by event. 
struct Event
{
    int hackerID, deskID, committedLines;
    Events type;
    float eventTime;

    Event(const int hackerID, const float time,const Events type, const int committedLines=0, const int deskID=0) :
        hackerID(hackerID), eventTime(time),type(type), committedLines(committedLines), deskID(deskID) {}
};

//compares events with respect to time
bool eventComparator(const Event& e1,const Event& e2) {
    if (smallFloat(e1.eventTime, e2.eventTime))
        return true;

    return equalFloat(e1.eventTime, e2.eventTime) && e1.hackerID < e2.hackerID;

}

//compares hackers with respect to entry time and ID
bool sticker_queue_comparator(const Hacker &h1, const Hacker &h2) {
    if (smallFloat(h1.sticker_entry_time, h2.sticker_entry_time))
        return false;
    else if (!equalFloat(h1.sticker_entry_time, h2.sticker_entry_time))
        return true;
    return  h1.id > h2.id;
}

//compares hackers with respect to numberOfCommits, queue entry time and  id.
bool hoodie_queue_comparator(const Hacker& h1, const Hacker& h2) {
    if (h1.numberOfCommits > h2.numberOfCommits)
        return false;
    else if (h1.numberOfCommits == h2.numberOfCommits && smallFloat(h1.hoodie_entry_time, h2.hoodie_entry_time))
        return false;
    else if (h1.numberOfCommits != h2.numberOfCommits)
        return true;
    else if (!equalFloat(h1.hoodie_entry_time, h2.hoodie_entry_time))
        return true;
    return  h1.id > h2.id;

}

int main(int argc, char* argv[])
{
    ifstream input_file(argv[1]);
    //N for number of hackers, C for commited lines, Q for number of queue entrance attempts, Ds for number of sticker desks
    //Dh for number og hoodie desks, T for time
    int N, C, Q, Ds, Dh;
    float T;

    //vectors for storing hackers and desks.
    vector<Hacker> hackers;
    vector<desk> sticker_desk, hoodie_desk;
    
    //using for storing events and sorting them with using event comparator method and sorting them after every insertion.
    set<Event, bool(*)(const Event&, const Event&)> timeline(eventComparator);

    //using priority queue for sticker_queue and hoodie_queue and sorting them after every insertion.
    priority_queue<Hacker, vector<Hacker>,bool(*)(const Hacker&, const Hacker&)> sticker_queue(sticker_queue_comparator);
    priority_queue<Hacker, vector<Hacker>, bool(*)(const Hacker&, const Hacker&)> hoodie_queue(hoodie_queue_comparator);

    //variables are self explanatory
    int  total_gift_count = 0, total_commit_count = 0, total_line_changes = 0, total_invalid_attempts = 0, total_greedy_attempts = 0;
    size_t max_sticker_queue_length = 0, max_hoodie_queue_length = 0;
    float total_sticker_waiting_time = 0.0, total_hoodie_waiting_time = 0.0, total_turnaround_time = 0.0;

    input_file >> N;

    //reading arrival times from input file. creating hacker with empalce and inserting it to vector.
    for (int i = 0; i < N; i++) {
        input_file >> T; 
        hackers.emplace_back(i+1);
    }

    input_file >> C;

    //reading number of commits, hackerID and time from file then emplacing event to timeline set. 
    for (int i = 0, hackerId, line_changes; i < C; i++) {
        input_file >> hackerId >> line_changes >> T;
        timeline.emplace(hackerId, T, commit, line_changes);
    }

    input_file >> Q; 

    //reading queue entrance attemps from file and emplacing them to timeline set.
    for (int i = 0, hackerId; i < Q; i++) {
        input_file >> hackerId >> T;
        timeline.emplace(hackerId, T, stickerQueueEntry);
    }

    input_file >> Ds;

    //reading service times from file and creating desk with id and given service time.
    for (int i = 0; i < Ds; i++) {
        input_file >> T;
        sticker_desk.emplace_back(i + 1, T);
    }


    input_file >> Dh;

    //reading service times from file and creating desk with id and given service time.
    for (int i = 0; i < Dh; i++) {
        input_file >> T;
        hoodie_desk.emplace_back(i + 1 , T);
    }
    input_file.close();

    //traversing in timeline set and simulating the DES event by event.
    for (const auto event : timeline) {
        switch (event.type)
        {
        case commit:
            if (event.committedLines >= MINIMUM_VALID_COMMIT_LINE_NUMBER) {
                hackers.at(event.hackerID-1).numberOfCommits++;
                }   
                total_commit_count++;
                total_line_changes += event.committedLines;
            
            break;

        case stickerQueueEntry:
            if (hackers.at(event.hackerID - 1).numberOfCommits >= MINIMUM_COMIT_QUEUE && hackers[event.hackerID - 1].gift_number < MAX_GIFT_PER_HACKER) {
                hackers.at(event.hackerID - 1).sticker_entry_time = event.eventTime;
                sticker_queue.emplace(hackers.at(event.hackerID - 1));
            }
            else {
                if (hackers.at(event.hackerID - 1).gift_number >= MAX_GIFT_PER_HACKER)
                    total_greedy_attempts++;
                else
                    total_invalid_attempts++;
            }
            break;

        case stickerDeskLeave:
            sticker_desk[event.deskID - 1].is_empty = true;
            hackers.at(event.hackerID-1).hoodie_entry_time=event.eventTime;
            hoodie_queue.emplace(hackers.at(event.hackerID - 1));
            break;

        case hoodieDeskLeave:
            hoodie_desk[event.deskID - 1].is_empty = true;
            hackers[event.hackerID - 1].gift_number++;
            total_gift_count++;
            total_turnaround_time += event.eventTime - hackers.at(event.hackerID - 1).sticker_entry_time;
            break;
        }

        //queueing hackers in the sticker_queue 
        for (auto& desk : sticker_desk) {
            if (desk.is_empty && !sticker_queue.empty()) {
                const auto hacker = sticker_queue.top();
                sticker_queue.pop();
                desk.hackerAtDest = hacker.id;
                if (largerFloat(event.eventTime, hacker.sticker_entry_time)) {
                    const float waiting_time = event.eventTime - hacker.sticker_entry_time;
                    hackers[desk.hackerAtDest - 1].total_time_queue += waiting_time;
                    total_sticker_waiting_time += waiting_time;
                }
                desk.is_empty = false;
                timeline.emplace(desk.hackerAtDest, event.eventTime + desk.service_time, stickerDeskLeave, 0, desk.id);
            }
        }

        //queueing hackers in the hoodie_queue
        for (auto& desk : hoodie_desk) {
            if (desk.is_empty && !hoodie_queue.empty()) {
                const auto hacker = hoodie_queue.top();
                hoodie_queue.pop();
                desk.hackerAtDest = hacker.id;
                if (largerFloat(event.eventTime, hacker.hoodie_entry_time)) {
                    const float waiting_time = event.eventTime - hacker.hoodie_entry_time;
                    hackers[desk.hackerAtDest-1].total_time_queue += waiting_time;
                    total_hoodie_waiting_time += waiting_time;
                }
                desk.is_empty = false;
                timeline.emplace(desk.hackerAtDest, event.eventTime + desk.service_time, hoodieDeskLeave, 0, desk.id);
            }
        }
        //assinging values maximum length for each queue
        if (sticker_queue.size() > max_sticker_queue_length)
            max_sticker_queue_length = sticker_queue.size();
        else if (hoodie_queue.size() > max_hoodie_queue_length)
            max_hoodie_queue_length = hoodie_queue.size();
    }

    int max_waiting_hacker_id=-1, min_waiting_hacker_id=-1;
    float max_queue_waiting_time=-1.0, min_queue_waiting_time=-1.0;
    
    //finds the hackers with maxiumum and minimumu waiting times and assigns them to variables above.
    for (const auto hacker : hackers) {
        if (largerFloat(hacker.total_time_queue, max_queue_waiting_time)) {
            max_waiting_hacker_id = hacker.id;
            max_queue_waiting_time = hacker.total_time_queue;
        }
        else if (equalFloat(hacker.total_time_queue, max_queue_waiting_time)&&hacker.id<max_waiting_hacker_id) {
            max_waiting_hacker_id = hacker.id;
        }
        if (hacker.gift_number >= MAX_GIFT_PER_HACKER) {
            if (smallFloat(hacker.total_time_queue, min_queue_waiting_time)||min_waiting_hacker_id==-1) {
                min_waiting_hacker_id = hacker.id;
                min_queue_waiting_time = hacker.total_time_queue;
            }
            else if (equalFloat(hacker.total_time_queue, min_queue_waiting_time) && hacker.id < min_waiting_hacker_id) {
                min_waiting_hacker_id = hacker.id;
            }
        }
    }

    ofstream output_file(argv[2]);
    //for printing 3 decimals.
    output_file.setf(ios::fixed, ios::floatfield);
    output_file.precision(3);

    output_file << max_sticker_queue_length << endl << max_hoodie_queue_length << endl << (float)total_gift_count / N << endl << total_sticker_waiting_time / total_gift_count << endl;
    output_file << total_hoodie_waiting_time / total_gift_count << endl << (float)total_commit_count / N << endl << (float)total_line_changes / total_commit_count << endl;
    output_file << total_turnaround_time / total_gift_count << endl << total_invalid_attempts << endl << total_greedy_attempts << endl;
    output_file << max_waiting_hacker_id << " " << max_queue_waiting_time << endl;
    output_file << min_waiting_hacker_id << " " << min_queue_waiting_time << endl;
    output_file << timeline.crbegin()->eventTime;
    output_file.close();
    return 0;
}
