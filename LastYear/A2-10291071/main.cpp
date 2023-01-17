//Name: Xiyue Wang, ID:10291071 29 Nov 2020
#include <string>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <random>
#include <chrono>
#include <map>
#include <cassert>
#include <condition_variable>

using namespace std;

// Print mutex so that the messages printed by each thread do not interfere with
// each other
#define PRINT_LOCK printMu.lock()
#define PRINT_UNLOCK printMu.unlock()

// global constants:
int const MAX_NUM_OF_THREADS = 6;
int const NUM_OF_SAMPLES = 50;
int const NUM_OF_LINKS = 2;
int const NUM_OF_SENSORS = 3;

// A prng seeded by time(0).
// time(0) gives the number of seconds elapsed since the start
// of the world according to Unix (00:00:00 on 1st January 1970).
thread_local std::mt19937 gen(time(0));
std::mutex printMu;
std::map<std::thread::id, int> threadIDs;
std::mutex mu;

enum SensorType { TEMPERATURE_SENSOR, PRESSURE_SENSOR, CAPACITIVE_SENSOR };

//sensor type name
const string SensorTypeName[] = {"temperature sensor", "pressure sensor",
                                 "capacitive sensor"};

void initID(int id) {
    std::unique_lock<std::mutex> map_locker(mu);
    threadIDs.insert(std::make_pair(std::this_thread::get_id(), id));
}

// Get the id of this thread
int getID() {
    std::map<std::thread::id, int>::iterator it =
        threadIDs.find(std::this_thread::get_id());
    return (it == threadIDs.end()) ? -1 : it->second;
}

class SensorData {  // Utility class to store sensor data
   public:
    SensorData(string type)  // Constructor
        : sensor_type(type) {}
    string getSensorType() { return sensor_type; }
    std::vector<double> getSensorData() { return sensor_data; }
    void addData(double newData) { sensor_data.push_back(newData); }

   private:
    string sensor_type;
    std::vector<double> sensor_data;
};  // end class SensorData

class Sensor {  // abstract base class that models a sensor
   public:
    Sensor(const string& type) : sensorType(type) {}  // constructor
    // Declare a virtual method to be overridden by derived classes:
    virtual double getValue() = 0;
    // Declare non-virtual method:
    string getType() {  // returns the type of Sensor that this is:
        return sensorType;
    }

   protected:
    // Declare any instance variable(s):
    string sensorType;
};  // end abstract class Sensor

class TempSensor : public Sensor {
   public:
    TempSensor(const string& type) : Sensor(type) {}  // constructor

    virtual double getValue() override {
        // return a random value of ambient temperature between 10 and 30
        std::uniform_int_distribution<> dis(10, 30);
        return dis(gen);
    }
};

class PressureSensor : public Sensor {
   public:
    PressureSensor(const string& type) : Sensor(type) {}  // constructor

    virtual double getValue() override {
        // return a random value of ambient temperature between 95 and 105
        std::uniform_int_distribution<> dis(95, 105);
        return dis(gen);
    }
};

class CapacitiveSensor : public Sensor {
   public:
    CapacitiveSensor(const string& type) : Sensor(type) {}  // constructor

    virtual double getValue() override {
        // return a random value of ambient temperature between 1 and 5
        std::uniform_int_distribution<> dis(1, 5);
        return dis(gen);
    }
};

class BC {
   public:
    BC(std::vector<Sensor*>& sensors) : theSensors(sensors) {
        for (size_t i = 0; i < theSensors.size(); ++i) {
            counters.push_back(0);
        }
    }

    void requestBC() {
        std::unique_lock<std::mutex> lck(BC_mu);
        int id = getID();
        if (lock) {
            PRINT_LOCK;
            cout << " BusContoller is locked, thread " << id
                 << " is about to suspend..." << endl;

            PRINT_UNLOCK;
            while (lock) {  // wait until lock is false
                cond.wait(lck);
            }
        }

        lock = true;
        PRINT_LOCK;
        cout << " BusContoller locked by thread " << id << endl;
        PRINT_UNLOCK;
    }

    void releaseBC() {
        std::unique_lock<std::mutex> lck(BC_mu);
        lock = false;
        int id = getID();
        PRINT_LOCK;
        cout << "BusContoller unlocked by thread " << id << endl;
        PRINT_UNLOCK;
        cond.notify_all();  // notify other threads
    }

    // Return the sensor's value.
    double getSensorValue(SensorType type) {
        return theSensors[type]->getValue();
    }

    // Return the sensor's type string.
    string getSensorType(SensorType type) {
        return theSensors[type]->getType();
    }

    // Increment counter for sensor chosen.
    void accessSensor(SensorType type) { counters[type]++; }

    // Print out the number of times each sensor was accessed
    void printCounter() {
        for (size_t i = 0; i < counters.size(); ++i) {
            cout << getSensorType(SensorType(i)) << " " << counters[i] << endl;
        }
    }

   private:
    bool lock = false;
    std::vector<Sensor*>& theSensors;
    std::vector<int> counters;
    std::mutex BC_mu;
    std::condition_variable cond;
};  // end of BC

class Receiver {
   public:
    Receiver() {  // constructor
        for (int i = 0; i < NUM_OF_SENSORS; ++i) {
            data.push_back(std::vector<double>());
        }
    }
    // Receives a SensorData object:
    void receiveData(SensorData sd) {
        std::unique_lock<std::mutex> lck(Recv_mu);
        SensorType type = SensorType(0);
        for (int i = 0; i < NUM_OF_SENSORS; ++i) {
            if (sd.getSensorType() == SensorTypeName[i]) {
                type = SensorType(i);
                break;
            }
        }

        // Copy data into the appropriate vector
        auto sensorData = sd.getSensorData();
        for (size_t i = 0; i < sensorData.size(); ++i) {
            data[type].push_back(sensorData[i]);
        }
    }
    // print out all data for each sensor:
    void printSensorData() {
        for (int i = 0; i < NUM_OF_SENSORS; ++i) {
            cout << SensorTypeName[i] << ": (" << data[i].size() << ")" << endl;
            for (size_t j = 0; j < data[i].size(); ++j) {
                cout << " " << data[i][j];
            }
            cout << endl;
        }
    }

   private:
    // mutex:
    std::mutex Recv_mu;
    // vectors to store sensor numeric data received from threads:
    std::vector<std::vector<double>> data;

};  // end class Receiver

class Link {
   public:
    Link(Receiver& r, int linkNum)  // Constructor
        : inUse(false), myReceiver(r), linkId(linkNum) {}
    // check if the link is currently in use
    bool isInUse() { return inUse; }
    // set the link status to busy
    void setInUse() { inUse = true; }
    // set the link status to idle
    void setIdle() { inUse = false; }
    // write data to the receiver
    void writeToDataLink(SensorData sd) { myReceiver.receiveData(sd); }
    // returns the link Id
    int getLinkId() { return linkId; }

   private:
    bool inUse;
    Receiver& myReceiver;  // Receiver reference
    int linkId;
};  // end class Link

class LinkAccessController {
   public:
    LinkAccessController(Receiver& r)  // Constructor
        : myReceiver(r), numOfAvailableLinks(NUM_OF_LINKS) {
        for (int i = 0; i < NUM_OF_LINKS; i++) {
            commsLinks.push_back(Link(myReceiver, i));
        }
    }
    // Request a comm's link: returns a reference to an available Link.
    // If none are available, the calling thread is suspended.
    Link& requestLink() {
        std::unique_lock<std::mutex> lck(LAC_mu);

        int id = getID();
        if (numOfAvailableLinks == 0) {
            PRINT_LOCK;
            cout << " LinkAccessController has no available Link, thread " << id
                 << " is about to suspend..." << endl;
            PRINT_UNLOCK;
            // wait until there is at least one available Link
            while (numOfAvailableLinks == 0) {
                cond.wait(lck);
            }
        }

        // Check which Link is available
        size_t linkNum = 0;
        while (commsLinks[linkNum].isInUse()) {
            linkNum++;
        }
        assert(linkNum < commsLinks.size());

        // Lock that Link and decrease the numOfAvailableLinks;
        commsLinks[linkNum].setInUse();
        --numOfAvailableLinks;

        PRINT_LOCK;
        cout << " Link " << linkNum << " locked by thread " << id << endl;
        PRINT_UNLOCK;

        return std::ref(commsLinks[linkNum]);
    }
    // Release a comms link:
    void releaseLink(Link& releasedLink) {
        std::unique_lock<std::mutex> lck(LAC_mu);

    // Unlock the Link and increase the numOfAvailableLinks;
        releasedLink.setIdle();
        ++numOfAvailableLinks;

    //Display which link is released by which thread
        int id = getID();
        PRINT_LOCK;
        cout << " Link " << releasedLink.getLinkId() << " unlocked by thread "
             << id << endl;
        PRINT_UNLOCK;

        cond.notify_all();  // notify other suspended threads
    }

   private:
    Receiver& myReceiver;  // Receiver reference
    int numOfAvailableLinks;
    std::vector<Link> commsLinks;
    std::mutex LAC_mu;  // mutex
    std::condition_variable cond;
};  // end class LinkAccessController

// run function ¨C executed by each thread:
void run(BC& theBC, LinkAccessController& lac, int idx) {
    std::uniform_int_distribution<> dis(1, 10);  // set up the distribution
    std::uniform_int_distribution<> dis2(0, 2);  // set up the distribution
    // from which integers will be drawn.
    initID(idx);

    vector<SensorData> data;
    for (int i = 0; i < NUM_OF_SENSORS; ++i) {
        data.push_back(SensorData(SensorTypeName[i]));
    }
    for (int i = 0; i < NUM_OF_SAMPLES; i++) {
        // NUM_OF_SAMPLES = 50 (initially)
        // request use of the BC:
        theBC.requestBC();

        // generate a random value between 0 and 2, and use it to
        // select a sensor and obtain a value and the sensor's type:
        int selector = dis2(gen);

        double sampleValue = theBC.getSensorValue(SensorType(selector));
        string sensorType = theBC.getSensorType(SensorType(selector));

        PRINT_LOCK;
        cout << "       sample value from thread " << idx << " from "
             << sensorType << " = " << sampleValue << endl;
        PRINT_UNLOCK;

        // Collect data
        data[selector].addData(sampleValue);

        // increment counter for sensor chosen (to keep count of
        // how many times each was used)
        theBC.accessSensor(SensorType(selector));

        // release the BC:
        theBC.releaseBC();

        // delay for random period between 0.001s ¨C 0.01s:
        int n = dis(gen);  // generate a pseudo-random integer between 1-10.
        std::this_thread::sleep_for(std::chrono::microseconds(n));
    }  // end of for

    for (size_t i = 0; i < data.size(); ++i) {
        Link* link = &lac.requestLink();
        link->writeToDataLink(data[i]);  // note pointer syntax
        lac.releaseLink(*link);

        // delay for random period between 0.001s ¨C 0.01s:
        int n = dis(gen);  // generate a pseudo-random integer between 1-10.
        std::this_thread::sleep_for(std::chrono::microseconds(n));
    }

}  // end of run

int main() {
    // declare a vector of Sensor pointers:
    std::vector<Sensor*> sensors;
    // initialise each sensor and insert into the vector:
    sensors.push_back(new TempSensor(
        SensorTypeName[TEMPERATURE_SENSOR]));  // push_back is a vector method.
    sensors.push_back(new PressureSensor(SensorTypeName[PRESSURE_SENSOR]));
    sensors.push_back(new CapacitiveSensor(SensorTypeName[CAPACITIVE_SENSOR]));

    // Instantiate the BC:
    BC theBC(std::ref(sensors));

    // Instantiate the Receiver:
    Receiver theReceiver;

    // Instantiate the LinkAccessController:
    LinkAccessController theLAC(std::ref(theReceiver));

    // Instantiate and start the threads:
    std::thread the_threads[MAX_NUM_OF_THREADS];  // array of threads
    for (int i = 0; i < MAX_NUM_OF_THREADS; i++) {
        // launch the threads:
        the_threads[i] = std::thread(run, std::ref(theBC), std::ref(theLAC), i);
    }

    // Wait for the threads to finish:
    for (auto& t : the_threads) {
        t.join();
    }

    // Print out all the data in the Receiver:
    theReceiver.printSensorData();

    cout << "All threads terminated" << endl;

    return 0;
}  // end of main
