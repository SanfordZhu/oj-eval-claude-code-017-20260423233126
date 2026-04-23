#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <chrono>

using namespace std;

// Date and time utilities
struct Date {
    int month;
    int day;

    Date() : month(0), day(0) {}
    Date(int m, int d) : month(m), day(d) {}

    int compare(const Date& other) const {
        if (month != other.month) return month - other.month;
        return day - other.day;
    }

    int dayOfYear() const {
        const int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31};
        int sum = 0;
        for (int i = 1; i < month; i++) {
            sum += daysInMonth[i];
        }
        sum += day;
        return sum;
    }

    string toString() const {
        char buf[6];
        sprintf(buf, "%02d-%02d", month, day);
        return string(buf);
    }

    static Date fromString(const string& s) {
        int m = (s[0] - '0') * 10 + (s[1] - '0');
        int d = (s[3] - '0') * 10 + (s[4] - '0');
        return Date(m, d);
    }

    int daysBetween(const Date& other) const {
        return dayOfYear() - other.dayOfYear();
    }
};

struct Time {
    int hour;
    int minute;

    Time() : hour(0), minute(0) {}
    Time(int h, int m) : hour(h), minute(m) {}

    int totalMinutes() const {
        return hour * 60 + minute;
    }

    string toString() const {
        char buf[6];
        sprintf(buf, "%02d:%02d", hour, minute);
        return string(buf);
    }

    static Time fromString(const string& s) {
        int h = (s[0] - '0') * 10 + (s[1] - '0');
        int m = (s[3] - '0') * 10 + (s[4] - '0');
        return Time(h, m);
    }
};

struct DateTime {
    Date date;
    Time time;

    DateTime() {}
    DateTime(Date d, Time t) : date(d), time(t) {}

    int totalMinutes() const {
        return date.dayOfYear() * 1440 + time.totalMinutes();
    }

    string dateString() const {
        return date.toString();
    }

    string timeString() const {
        return time.toString();
    }
};

// Hash table node template
template<typename T>
struct HashNode {
    string key;
    T value;
    HashNode* next;
    HashNode(const string& k, const T& v) : key(k), value(v), next(nullptr) {}
};

// Simple hash function for strings
unsigned int hashString(const string& s) {
    unsigned int hash = 5381;
    for (size_t i = 0; i < s.size(); i++) {
        hash = ((hash << 5) + hash) + s[i];
    }
    return hash;
}

// Hash table implementation
template<typename T>
class HashTable {
private:
    HashNode<T>** table;
    int capacity;
    int size;

public:
    HashTable(int cap = 1024) : capacity(cap), size(0) {
        table = new HashNode<T>*[capacity];
        for (int i = 0; i < capacity; i++) {
            table[i] = nullptr;
        }
    }

    ~HashTable() {
        clear();
        delete[] table;
    }

    void clear() {
        for (int i = 0; i < capacity; i++) {
            HashNode<T>* node = table[i];
            while (node != nullptr) {
                HashNode<T>* next = node->next;
                delete node;
                node = next;
            }
            table[i] = nullptr;
        }
        size = 0;
    }

    int getSize() const { return size; }

    bool contains(const string& key) const {
        unsigned int idx = hashString(key) % capacity;
        HashNode<T>* node = table[idx];
        while (node != nullptr) {
            if (node->key == key) {
                return true;
            }
            node = node->next;
        }
        return false;
    }

    T* get(const string& key) const {
        unsigned int idx = hashString(key) % capacity;
        HashNode<T>* node = table[idx];
        while (node != nullptr) {
            if (node->key == key) {
                return &(node->value);
            }
            node = node->next;
        }
        return nullptr;
    }

    bool insert(const string& key, const T& value) {
        if (contains(key)) {
            return false;
        }
        unsigned int idx = hashString(key) % capacity;
        HashNode<T>* newNode = new HashNode<T>(key, value);
        newNode->next = table[idx];
        table[idx] = newNode;
        size++;
        return true;
    }

    bool remove(const string& key) {
        unsigned int idx = hashString(key) % capacity;
        HashNode<T>* node = table[idx];
        HashNode<T>* prev = nullptr;
        while (node != nullptr) {
            if (node->key == key) {
                if (prev == nullptr) {
                    table[idx] = node->next;
                } else {
                    prev->next = node->next;
                }
                delete node;
                size--;
                return true;
            }
            prev = node;
            node = node->next;
        }
        return false;
    }

    // Iterator helper
    void getAll(T** array, int& count) const {
        count = 0;
        for (int i = 0; i < capacity && count < size; i++) {
            HashNode<T>* node = table[i];
            while (node != nullptr) {
                array[count++] = &(node->value);
                node = node->next;
            }
        }
    }
};

// Dynamic array implementation
template<typename T>
class DynamicArray {
private:
    T* data;
    int capacity;
    int size;

public:
    DynamicArray(int initialCap = 16) : capacity(initialCap), size(0) {
        data = new T[capacity];
    }

    ~DynamicArray() {
        delete[] data;
    }

    int& getSizeRef() { return size; }
    int getSize() const { return size; }

    T& operator[](int index) { return data[index]; }
    const T& operator[](int index) const { return data[index]; }

    void pushBack(const T& value) {
        if (size >= capacity) {
            int newCapacity = capacity * 2;
            T* newData = new T[newCapacity];
            for (int i = 0; i < size; i++) {
                newData[i] = data[i];
            }
            delete[] data;
            data = newData;
            capacity = newCapacity;
        }
        data[size++] = value;
    }

    void clear() {
        size = 0;
    }
};

// User structure
struct User {
    string username;
    string password;
    string name;
    string mailAddr;
    int privilege;

    User() : privilege(0) {}
    User(const string& uname, const string& pwd, const string& n, const string& mail, int priv)
        : username(uname), password(pwd), name(n), mailAddr(mail), privilege(priv) {}
};

// Station information for a train
struct StationInfo {
    string name;
    int arrivalOffset;
    int departureOffset;
    int cumulativePrice;
};

// Train structure
struct Train {
    string trainID;
    int stationNum;
    int seatNum;
    Date saleStart;
    Date saleEnd;
    Time startTime;
    char type;
    bool released;

    StationInfo* stations;
    int* prices;
    int* travelTimes;
    int* stopoverTimes;

    Train() : stationNum(0), seatNum(0), type(' '), released(false), stations(nullptr), prices(nullptr), travelTimes(nullptr), stopoverTimes(nullptr) {}

    ~Train() {
        if (stations != nullptr) delete[] stations;
        if (prices != nullptr) delete[] prices;
        if (travelTimes != nullptr) delete[] travelTimes;
        if (stopoverTimes != nullptr) delete[] stopoverTimes;
    }

    // Allow move only
    Train(const Train& other) = delete;
    Train& operator=(const Train& other) = delete;

    // Move constructor
    Train(Train&& other) noexcept
        : trainID(std::move(other.trainID)), stationNum(other.stationNum),
          seatNum(other.seatNum), saleStart(other.saleStart), saleEnd(other.saleEnd),
          startTime(other.startTime), type(other.type), released(other.released),
          stations(other.stations), prices(other.prices),
          travelTimes(other.travelTimes), stopoverTimes(other.stopoverTimes) {
        other.stations = nullptr;
        other.prices = nullptr;
        other.travelTimes = nullptr;
        other.stopoverTimes = nullptr;
        other.stationNum = 0;
    }

    int getStationIndex(const string& stationName) const {
        for (int i = 0; i < stationNum; i++) {
            if (stations[i].name == stationName) {
                return i;
            }
        }
        return -1;
    }

    int calculatePrice(int fromIdx, int toIdx) const {
        int price = 0;
        for (int i = fromIdx; i < toIdx; i++) {
            price += prices[i];
        }
        return price;
    }

    int calculateTravelTime(int fromIdx, int toIdx) const {
        return stations[toIdx].arrivalOffset - stations[fromIdx].departureOffset;
    }

    DateTime getArrivalTime(Date departureDate, int stationIdx) const {
        int totalOffset = stations[stationIdx].arrivalOffset;
        int totalMinutes = departureDate.dayOfYear() * 1440 + startTime.totalMinutes() + totalOffset;
        int days = totalMinutes / 1440;
        int mins = totalMinutes % 1440;
        Date d(6, 1);
        int dayOfY = days;
        // Convert dayOfYear back to month and day (2021 is not leap)
        const int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31};
        int month = 1;
        while (month <= 8 && dayOfY > daysInMonth[month]) {
            dayOfY -= daysInMonth[month];
            month++;
        }
        return DateTime(Date(month, dayOfY), Time(mins / 60, mins % 60));
    }

    DateTime getDepartureTime(Date departureDate, int stationIdx) const {
        int totalOffset = stations[stationIdx].departureOffset;
        int totalMinutes = departureDate.dayOfYear() * 1440 + startTime.totalMinutes() + totalOffset;
        int days = totalMinutes / 1440;
        int mins = totalMinutes % 1440;
        int dayOfY = days;
        const int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31};
        int month = 1;
        while (month <= 8 && dayOfY > daysInMonth[month]) {
            dayOfY -= daysInMonth[month];
            month++;
        }
        return DateTime(Date(month, dayOfY), Time(mins / 60, mins % 60));
    }
};

// Order status enum
enum OrderStatus {
    ORDER_SUCCESS,
    ORDER_PENDING,
    ORDER_REFUNDED
};

// Order structure
struct Order {
    int orderId;
    string username;
    string trainID;
    Date departureDate;
    int fromStationIdx;
    int toStationIdx;
    int numTickets;
    int totalPrice;
    OrderStatus status;
    time_t timestamp;

    Order() : orderId(0), numTickets(0), fromStationIdx(0), toStationIdx(0), totalPrice(0), status(ORDER_SUCCESS), timestamp(0) {}
    Order(int id, const string& uname, const string& tid, Date d, int from, int to, int n, int price, OrderStatus st)
        : orderId(id), username(uname), trainID(tid), departureDate(d), fromStationIdx(from), toStationIdx(to),
          numTickets(n), totalPrice(price), status(st), timestamp(std::time(nullptr)) {}
};

// Ticket availability for a specific date and segment
struct TicketSale {
    int* remaining;

    TicketSale(int segments) {
        remaining = new int[segments];
        for (int i = 0; i < segments; i++) {
            remaining[i] = 0;
        }
    }

    ~TicketSale() {
        delete[] remaining;
    }

    TicketSale(const TicketSale& other) = delete;
    TicketSale& operator=(const TicketSale& other) = delete;
};

// Waiting queue node
struct QueueNode {
    Order* order;
    QueueNode* next;
    QueueNode(Order* o) : order(o), next(nullptr) {}
};

class WaitingQueue {
private:
    QueueNode* head;
    QueueNode* tail;
    int size;

public:
    WaitingQueue() : head(nullptr), tail(nullptr), size(0) {}

    ~WaitingQueue() {
        while (head != nullptr) {
            QueueNode* next = head->next;
            delete head;
            head = next;
        }
    }

    int getSize() const { return size; }

    void enqueue(Order* order) {
        QueueNode* node = new QueueNode(order);
        if (tail == nullptr) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        size++;
    }

    Order* dequeue() {
        if (head == nullptr) return nullptr;
        QueueNode* node = head;
        head = head->next;
        if (head == nullptr) {
            tail = nullptr;
        }
        Order* order = node->order;
        delete node;
        size--;
        return order;
    }

    bool removeOrder(Order* order) {
        QueueNode* curr = head;
        QueueNode* prev = nullptr;
        while (curr != nullptr) {
            if (curr->order == order) {
                if (prev == nullptr) {
                    head = curr->next;
                } else {
                    prev->next = curr->next;
                }
                if (curr == tail) {
                    tail = prev;
                }
                delete curr;
                size--;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }
};

// Global tickets state
class TrainTicketSystem {
private:
    HashTable<User*> users;
    HashTable<Train*> trains;
    HashTable<DynamicArray<Order*>*> userOrders;
    HashTable<HashTable<TicketSale*>*> dailyTickets;
    HashTable<WaitingQueue*> waitingQueues;
    DynamicArray<string> loggedInUsers;
    int nextOrderId;

public:
    TrainTicketSystem() : nextOrderId(1) {}

    ~TrainTicketSystem() {
        // Clean up users
        // HashTable<T> → getAll(T* array[]) → expects an array of pointers to T, passed as T**
        // T = User*, so we need array of (User*)* → which is User** → passed as User***
        User** *userArray = new User**[users.getSize()];
        int ucount;
        users.getAll(userArray, ucount);
        for (int j = 0; j < ucount; j++) {
            delete *userArray[j];
        }
        delete[] userArray;

        // Clean up trains
        // T = Train*, so same pattern → array of (Train*)* → Train** → passed as Train***
        Train** *trainArray = new Train**[trains.getSize()];
        int tcount;
        trains.getAll(trainArray, tcount);
        for (int j = 0; j < tcount; j++) {
            delete *trainArray[j];
        }
        delete[] trainArray;

        // Clean up daily tickets
        // T = HashTable<TicketSale*>*, so array of (HashTable<TicketSale*>)* → HashTable<TicketSale*>** → passed as HashTable<TicketSale*>***
        HashTable<TicketSale*>** *dailyArray = new HashTable<TicketSale*>**[dailyTickets.getSize()];
        int dcount;
        dailyTickets.getAll(dailyArray, dcount);
        for (int i = 0; i < dcount; i++) {
            HashTable<TicketSale*>* dateTable = *dailyArray[i];
            // In dateTable, T = TicketSale*, so array of (TicketSale*)* → TicketSale** → passed as TicketSale***
            TicketSale** *saleArray = new TicketSale**[dateTable->getSize()];
            int scount;
            dateTable->getAll(saleArray, scount);
            for (int j = 0; j < scount; j++) {
                delete *saleArray[j];
            }
            delete[] saleArray;
            delete dateTable;
        }
        delete[] dailyArray;

        // Clean up waiting queues
        // T = WaitingQueue*, array of (WaitingQueue*)* → WaitingQueue** → passed as WaitingQueue***
        WaitingQueue** *queueArray = new WaitingQueue**[waitingQueues.getSize()];
        int qcount;
        waitingQueues.getAll(queueArray, qcount);
        for (int i = 0; i < qcount; i++) {
            delete *queueArray[i];
        }
        delete[] queueArray;

        // Clean up user order arrays
        // T = DynamicArray<Order*>*, array of (DynamicArray<Order*>)* → DynamicArray<Order*>** → passed as DynamicArray<Order*>***
        DynamicArray<Order*>** *orderArray = new DynamicArray<Order*>**[userOrders.getSize()];
        int ocount;
        userOrders.getAll(orderArray, ocount);
        for (int i = 0; i < ocount; i++) {
            DynamicArray<Order*>* orders = *orderArray[i];
            for (int j = 0; j < orders->getSize(); j++) {
                delete (*orders)[j];
            }
            delete orders;
        }
        delete[] orderArray;
    }

    // User operations
    bool hasUser(const string& username) const {
        return users.contains(username);
    }

    bool addUser(const string& username, const string& password, const string& name, const string& mail, int privilege) {
        User* user = new User(username, password, name, mail, privilege);
        if (!users.insert(username, user)) {
            delete user;
            return false;
        }
        userOrders.insert(username, new DynamicArray<Order*>());
        return true;
    }
    bool login(const string& username, const string& password) {
        // Check if already logged in
        for (int i = 0; i < loggedInUsers.getSize(); i++) {
            if (loggedInUsers[i] == username) {
                return false;
            }
        }
        auto userPtr = users.get(username);
        if (userPtr == nullptr) return false;
        User* user = *userPtr;
        if (user->password != password) {
            return false;
        }
        loggedInUsers.pushBack(username);
        return true;
    }

    bool logout(const string& username) {
        for (int i = 0; i < loggedInUsers.getSize(); i++) {
            if (loggedInUsers[i] == username) {
                // Shift elements
                for (int j = i; j < loggedInUsers.getSize() - 1; j++) {
                    loggedInUsers[j] = loggedInUsers[j + 1];
                }
                loggedInUsers.getSizeRef()--;
                return true;
            }
        }
        return false;
    }

    bool isLoggedIn(const string& username) const {
        for (int i = 0; i < loggedInUsers.getSize(); i++) {
            if (loggedInUsers[i] == username) {
                return true;
            }
        }
        return false;
    }

    User* getUser(const string& username) {
        auto userPtr = users.get(username);
        if (userPtr == nullptr) return nullptr;
        return *userPtr;
    }

    bool checkPermission(const string& requester, const string& target) const {
        auto reqPtr = const_cast<TrainTicketSystem*>(this)->users.get(requester);
        auto tgtPtr = const_cast<TrainTicketSystem*>(this)->users.get(target);
        if (reqPtr == nullptr || tgtPtr == nullptr) return false;
        User* req = *reqPtr;
        User* tgt = *tgtPtr;
        if (!isLoggedIn(requester)) return false;
        return req->privilege >= tgt->privilege;
    }

    bool canModifyPrivilege(const string& requester, int newPrivilege) const {
        auto reqPtr = const_cast<TrainTicketSystem*>(this)->users.get(requester);
        if (reqPtr == nullptr) return false;
        User* req = *reqPtr;
        return newPrivilege < req->privilege;
    }

    // Train operations
    bool addTrain(Train* train) {
        string id = train->trainID;
        if (trains.contains(id)) {
            return false;
        }
        trains.insert(id, train);
        return true;
    }

    bool deleteTrain(const string& id) {
        if (!trains.contains(id)) return false;
        auto trainPtr = trains.get(id);
        Train* train = *trainPtr;
        if (train->released) return false;
        trains.remove(id);
        delete train;
        return true;
    }

    bool releaseTrain(const string& id) {
        if (!trains.contains(id)) return false;
        auto trainPtr = trains.get(id);
        Train* train = *trainPtr;
        if (train->released) return false;

        // Initialize ticket sales for all dates in sale range
        Date start = train->saleStart;
        Date end = train->saleEnd;
        int startDay = start.dayOfYear();
        int endDay = end.dayOfYear();

        for (int day = startDay; day <= endDay; day++) {
            // Convert day to date
            const int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31};
            int month = 1;
            int d = day;
            while (month <= 8 && d > daysInMonth[month]) {
                d -= daysInMonth[month];
                month++;
            }
            Date date(month, d);
            string key = id + "_" + date.toString();

            auto dateTablePtr = dailyTickets.get(date.toString());
            HashTable<TicketSale*>* dateTable = nullptr;
            if (dateTablePtr == nullptr) {
                dailyTickets.insert(date.toString(), new HashTable<TicketSale*>(16));
                dateTablePtr = dailyTickets.get(date.toString());
                dateTable = *dateTablePtr;
            } else {
                dateTable = *dateTablePtr;
            }
            TicketSale* sale = new TicketSale(train->stationNum - 1);
            for (int i = 0; i < train->stationNum - 1; i++) {
                sale->remaining[i] = train->seatNum;
            }
            dateTable->insert(id, sale);

            // Initialize waiting queue
            string queueKey = id + "_" + date.toString();
            waitingQueues.insert(queueKey, new WaitingQueue());
        }

        train->released = true;
        return true;
    }

    Train* getTrain(const string& id) {
        auto trainPtr = trains.get(id);
        if (trainPtr == nullptr) return nullptr;
        return *trainPtr;
    }

    TicketSale* getTicketSale(const string& trainId, const string& dateStr) {
        auto dateTablePtr = dailyTickets.get(dateStr);
        if (dateTablePtr == nullptr) return nullptr;
        HashTable<TicketSale*>* dateTable = *dateTablePtr;
        auto salePtr = dateTable->get(trainId);
        if (salePtr == nullptr) return nullptr;
        return *salePtr;
    }

    WaitingQueue* getWaitingQueue(const string& trainId, const string& dateStr) {
        string key = trainId + "_" + dateStr;
        auto queuePtr = waitingQueues.get(key);
        if (queuePtr == nullptr) return nullptr;
        return *queuePtr;
    }

    // Order operations
    int getNextOrderId() { return nextOrderId++; }

    void addOrder(Order* order) {
        auto ordersPtr = userOrders.get(order->username);
        DynamicArray<Order*>* orders = *ordersPtr;
        orders->pushBack(order);
    }

    DynamicArray<Order*>* getUserOrders(const string& username) {
        auto ptr = userOrders.get(username);
        if (ptr == nullptr) return nullptr;
        return *ptr;
    }

    bool purchaseTickets(Train* train, const string& dateStr, int fromIdx, int toIdx, int numTickets) {
        TicketSale* sale = getTicketSale(train->trainID, dateStr);
        if (sale == nullptr) return false;

        // Check if enough tickets available across all segments
        bool enough = true;
        for (int i = fromIdx; i < toIdx; i++) {
            if (sale->remaining[i] < numTickets) {
                enough = false;
                break;
            }
        }

        if (!enough) return false;

        // Deduct tickets
        for (int i = fromIdx; i < toIdx; i++) {
            sale->remaining[i] -= numTickets;
        }

        return true;
    }

    void refundTickets(Train* train, const string& dateStr, int fromIdx, int toIdx, int numTickets) {
        TicketSale* sale = getTicketSale(train->trainID, dateStr);
        if (sale == nullptr) return;

        // Add tickets back
        for (int i = fromIdx; i < toIdx; i++) {
            sale->remaining[i] += numTickets;
        }

        // Process waiting queue
        WaitingQueue* queue = getWaitingQueue(train->trainID, dateStr);
        if (queue == nullptr) return;

        // Try to satisfy pending orders in FIFO order
        while (queue->getSize() > 0) {
            Order* pendingOrder = queue->dequeue();
            TicketSale* pendingSale = getTicketSale(pendingOrder->trainID, pendingOrder->departureDate.toString());

            bool canSatisfy = true;
            for (int i = pendingOrder->fromStationIdx; i < pendingOrder->toStationIdx; i++) {
                if (pendingSale->remaining[i] < pendingOrder->numTickets) {
                    canSatisfy = false;
                    break;
                }
            }

            if (canSatisfy) {
                // Satisfy this order
                pendingOrder->status = ORDER_SUCCESS;
                for (int i = pendingOrder->fromStationIdx; i < pendingOrder->toStationIdx; i++) {
                    pendingSale->remaining[i] -= pendingOrder->numTickets;
                }
                break;
            } else {
                // Put it back
                queue->enqueue(pendingOrder);
                break;
            }
        }
    }

    int getAvailableSeats(Train* train, const string& dateStr, int fromIdx, int toIdx) {
        TicketSale* sale = getTicketSale(train->trainID, dateStr);
        if (sale == nullptr) {
            return train->seatNum;
        }
        int minSeats = train->seatNum;
        for (int i = fromIdx; i < toIdx; i++) {
            if (sale->remaining[i] < minSeats) {
                minSeats = sale->remaining[i];
            }
        }
        return minSeats;
    }

    void getAllTrains(DynamicArray<Train*>& result) {
        Train*** trainArray = new Train**[trains.getSize()];
        int count;
        trains.getAll(trainArray, count);
        for (int i = 0; i < count; i++) {
            result.pushBack(*trainArray[i]);
        }
        delete[] trainArray;
    }

    void clean() {
        // Delete all users
        User*** userArray = new User**[users.getSize()];
        int ucount;
        users.getAll(userArray, ucount);
        for (int i = 0; i < ucount; i++) {
            delete *userArray[i];
        }
        delete[] userArray;

        // Delete all trains
        Train*** trainArray = new Train**[trains.getSize()];
        int tcount;
        trains.getAll(trainArray, tcount);
        for (int i = 0; i < tcount; i++) {
            delete *trainArray[i];
        }
        delete[] trainArray;

        // Clear everything
        users.clear();
        trains.clear();
        userOrders.clear();
        dailyTickets.clear();
        waitingQueues.clear();
        loggedInUsers.clear();
        nextOrderId = 1;
    }

    int getUserCount() const { return users.getSize(); }
};

// Command parser
struct CommandArg {
    char key;
    string value;
};

class CommandParser {
public:
    static bool parseCommand(const string& line, string& command, DynamicArray<CommandArg>& args) {
        args.clear();
        size_t pos = 0;
        size_t len = line.size();

        // Skip leading whitespace
        while (pos < len && isspace(line[pos])) pos++;
        if (pos >= len) return false;

        // Parse command name
        size_t cmdStart = pos;
        while (pos < len && !isspace(line[pos])) pos++;
        command = line.substr(cmdStart, pos - cmdStart);

        // Parse arguments
        while (pos < len) {
            // Skip whitespace
            while (pos < len && isspace(line[pos])) pos++;
            if (pos >= len) break;

            if (line[pos] != '-') return false;
            pos++;
            if (pos >= len) return false;
            char key = line[pos];
            pos++;

            // Skip whitespace after -key
            while (pos < len && isspace(line[pos])) pos++;
            if (pos >= len) return false;

            // Parse value until next whitespace or -
            size_t valStart = pos;
            while (pos < len && !isspace(line[pos])) pos++;
            string value = line.substr(valStart, pos - valStart);

            CommandArg arg;
            arg.key = key;
            arg.value = value;
            args.pushBack(arg);
        }

        return true;
    }
};

// Split string by delimiter
void splitString(const string& s, char delim, DynamicArray<string>& result) {
    result.clear();
    size_t start = 0;
    size_t pos = 0;
    while (pos < s.size()) {
        if (s[pos] == delim) {
            result.pushBack(s.substr(start, pos - start));
            start = pos + 1;
        }
        pos++;
    }
    if (start < s.size()) {
        result.pushBack(s.substr(start, pos - start));
    }
}

// Ticket query result
struct QueryTicketResult {
    Train* train;
    int fromIdx;
    int toIdx;
    int price;
    int travelTime;
    int availableSeats;
};

// Transfer query result
struct TransferResult {
    QueryTicketResult first;
    QueryTicketResult second;
    int totalPrice;
    int totalTime;
};

// Compare functions for sorting
bool compareByCost(const QueryTicketResult& a, const QueryTicketResult& b) {
    if (a.price != b.price) {
        return a.price < b.price;
    }
    return a.train->trainID < b.train->trainID;
}

bool compareByTime(const QueryTicketResult& a, const QueryTicketResult& b) {
    if (a.travelTime != b.travelTime) {
        return a.travelTime < b.travelTime;
    }
    return a.train->trainID < b.train->trainID;
}

bool compareTransferByCost(const TransferResult& a, const TransferResult& b) {
    if (a.totalPrice != b.totalPrice) {
        return a.totalPrice < b.totalPrice;
    }
    return a.first.train->trainID < b.first.train->trainID;
}

bool compareTransferByTime(const TransferResult& a, const TransferResult& b) {
    if (a.totalTime != b.totalTime) {
        return a.totalTime < b.totalTime;
    }
    if (a.first.travelTime != b.first.travelTime) {
        return a.first.travelTime < b.first.travelTime;
    }
    return a.first.train->trainID < b.first.train->trainID;
}

// Selection sort (since we can't use STL algorithm)
void sortQueryResults(DynamicArray<QueryTicketResult>& array, bool byTime) {
    int n = array.getSize();
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            bool smaller;
            if (byTime) {
                smaller = compareByTime(array[j], array[minIdx]);
            } else {
                smaller = compareByCost(array[j], array[minIdx]);
            }
            if (smaller) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            QueryTicketResult temp = array[i];
            array[i] = array[minIdx];
            array[minIdx] = temp;
        }
    }
}

// Main program
TrainTicketSystem tickets;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string line;
    while (getline(cin, line)) {
        if (line.empty()) continue;

        string command;
        DynamicArray<CommandArg> args;
        if (!CommandParser::parseCommand(line, command, args)) {
            cout << "-1\n";
            continue;
        }

        if (command == "add_user") {
            string c, u, p, n, m;
            int g = 0;
            bool hasC = false, hasU = false, hasP = false, hasN = false, hasM = false, hasG = false;
            for (int i = 0; i < args.getSize(); i++) {
                if (args[i].key == 'c') { c = args[i].value; hasC = true; }
                else if (args[i].key == 'u') { u = args[i].value; hasU = true; }
                else if (args[i].key == 'p') { p = args[i].value; hasP = true; }
                else if (args[i].key == 'n') { n = args[i].value; hasN = true; }
                else if (args[i].key == 'm') { m = args[i].value; hasM = true; }
                else if (args[i].key == 'g') { g = atoi(args[i].value.c_str()); hasG = true; }
            }
            if (!hasU || !hasP || !hasN || !hasM) {
                cout << "-1\n";
                continue;
            }

            if (tickets.getUserCount() == 0) {
                // First user
                if (tickets.addUser(u, p, n, m, 10)) {
                    cout << "0\n";
                } else {
                    cout << "-1\n";
                }
            } else {
                if (!hasC || !hasG || !tickets.isLoggedIn(c)) {
                    cout << "-1\n";
                    continue;
                }
                User* curr = tickets.getUser(c);
                if (g >= curr->privilege) {
                    cout << "-1\n";
                    continue;
                }
                if (tickets.addUser(u, p, n, m, g)) {
                    cout << "0\n";
                } else {
                    cout << "-1\n";
                }
            }
        } else if (command == "login") {
            string u, p;
            bool hasU = false, hasP = false;
            for (int i = 0; i < args.getSize(); i++) {
                if (args[i].key == 'u') { u = args[i].value; hasU = true; }
                else if (args[i].key == 'p') { p = args[i].value; hasP = true; }
            }
            if (!hasU || !hasP) {
                cout << "-1\n";
                continue;
            }
            if (tickets.login(u, p)) {
                cout << "0\n";
            } else {
                cout << "-1\n";
            }
        } else if (command == "logout") {
            string u;
            bool hasU = false;
            for (int i = 0; i < args.getSize(); i++) {
                if (args[i].key == 'u') { u = args[i].value; hasU = true; }
            }
            if (!hasU) {
                cout << "-1\n";
                continue;
            }
            if (tickets.logout(u)) {
                cout << "0\n";
            } else {
                cout << "-1\n";
            }
        } else if (command == "query_profile") {
            string c, u;
            bool hasC = false, hasU = false;
            for (int i = 0; i < args.getSize(); i++) {
                if (args[i].key == 'c') { c = args[i].value; hasC = true; }
                else if (args[i].key == 'u') { u = args[i].value; hasU = true; }
            }
            if (!hasC || !hasU) {
                cout << "-1\n";
                continue;
            }
            if (!tickets.isLoggedIn(c)) {
                cout << "-1\n";
                continue;
            }
            if (!tickets.checkPermission(c, u)) {
                cout << "-1\n";
                continue;
            }
            User* target = tickets.getUser(u);
            if (target == nullptr) {
                cout << "-1\n";
                continue;
            }
            cout << target->username << " " << target->name << " " << target->mailAddr << " " << target->privilege << "\n";
        } else if (command == "modify_profile") {
            string c, u;
            string newP, newN, newM;
            int newG = 0;
            bool hasC = false, hasU = false;
            bool hasP = false, hasN = false, hasM = false, hasG = false;
            for (int i = 0; i < args.getSize(); i++) {
                if (args[i].key == 'c') { c = args[i].value; hasC = true; }
                else if (args[i].key == 'u') { u = args[i].value; hasU = true; }
                else if (args[i].key == 'p') { newP = args[i].value; hasP = true; }
                else if (args[i].key == 'n') { newN = args[i].value; hasN = true; }
                else if (args[i].key == 'm') { newM = args[i].value; hasM = true; }
                else if (args[i].key == 'g') { newG = atoi(args[i].value.c_str()); hasG = true; }
            }
            if (!hasC || !hasU) {
                cout << "-1\n";
                continue;
            }
            if (!tickets.isLoggedIn(c)) {
                cout << "-1\n";
                continue;
            }
            if (!tickets.checkPermission(c, u)) {
                cout << "-1\n";
                continue;
            }
            if (hasG && !tickets.canModifyPrivilege(c, newG)) {
                cout << "-1\n";
                continue;
            }

            User* target = tickets.getUser(u);
            if (target == nullptr) {
                cout << "-1\n";
                continue;
            }

            if (hasP) target->password = newP;
            if (hasN) target->name = newN;
            if (hasM) target->mailAddr = newM;
            if (hasG) target->privilege = newG;

            cout << target->username << " " << target->name << " " << target->mailAddr << " " << target->privilege << "\n";
        } else if (command == "add_train") {
            string i, n, m, s, p, x, t, o, d, y;
            for (int j = 0; j < args.getSize(); j++) {
                if (args[j].key == 'i') { i = args[j].value; }
                else if (args[j].key == 'n') { n = args[j].value; }
                else if (args[j].key == 'm') { m = args[j].value; }
                else if (args[j].key == 's') { s = args[j].value; }
                else if (args[j].key == 'p') { p = args[j].value; }
                else if (args[j].key == 'x') { x = args[j].value; }
                else if (args[j].key == 't') { t = args[j].value; }
                else if (args[j].key == 'o') { o = args[j].value; }
                else if (args[j].key == 'd') { d = args[j].value; }
                else if (args[j].key == 'y') { y = args[j].value; }
            }
            // Check required fields are present
            if (i.empty() || n.empty() || m.empty() || s.empty() || p.empty() || x.empty() || t.empty() || o.empty() || d.empty() || y.empty()) {
                cout << "-1\n";
                continue;
            }

            Train* train = new Train();
            train->trainID = i;
            train->stationNum = atoi(n.c_str());
            train->seatNum = atoi(m.c_str());
            train->startTime = Time::fromString(x);
            train->type = y[0];
            train->released = false;

            // Parse sale date
            DynamicArray<string> saleDates;
            splitString(d, '|', saleDates);
            if (saleDates.getSize() != 2) {
                cout << "-1\n";
                delete train;
                continue;
            }
            train->saleStart = Date::fromString(saleDates[0]);
            train->saleEnd = Date::fromString(saleDates[1]);

            // Parse stations
            DynamicArray<string> stations;
            splitString(s, '|', stations);
            if (stations.getSize() != train->stationNum) {
                cout << "-1\n";
                delete train;
                continue;
            }

            train->stations = new StationInfo[train->stationNum];
            for (int j = 0; j < train->stationNum; j++) {
                train->stations[j].name = stations[j];
            }

            // Parse prices
            DynamicArray<string> prices;
            splitString(p, '|', prices);
            if (prices.getSize() != train->stationNum - 1) {
                cout << "-1\n";
                delete train;
                continue;
            }
            train->prices = new int[train->stationNum - 1];
            for (int j = 0; j < train->stationNum - 1; j++) {
                train->prices[j] = atoi(prices[j].c_str());
            }

            // Parse travel times
            DynamicArray<string> travelTimes;
            splitString(t, '|', travelTimes);
            if (travelTimes.getSize() != train->stationNum - 1) {
                cout << "-1\n";
                delete train;
                continue;
            }
            train->travelTimes = new int[train->stationNum - 1];
            for (int j = 0; j < train->stationNum - 1; j++) {
                train->travelTimes[j] = atoi(travelTimes[j].c_str());
            }

            // Parse stopover times
            train->stopoverTimes = new int[train->stationNum - 2];
            if (!(train->stationNum == 2 && o == "_")) {
                DynamicArray<string> stopovers;
                splitString(o, '|', stopovers);
                if (stopovers.getSize() != train->stationNum - 2) {
                    cout << "-1\n";
                    delete train;
                    continue;
                }
                for (int j = 0; j < train->stationNum - 2; j++) {
                    train->stopoverTimes[j] = atoi(stopovers[j].c_str());
                }
            }

            // Calculate arrival and departure offsets
            int currentOffset = 0;
            // Starting station
            train->stations[0].departureOffset = currentOffset;
            train->stations[0].cumulativePrice = 0;

            for (int j = 0; j < train->stationNum - 1; j++) {
                // Travel to next station
                currentOffset += train->travelTimes[j];
                train->stations[j + 1].arrivalOffset = currentOffset;
                train->stations[j + 1].cumulativePrice = train->stations[j].cumulativePrice + train->prices[j];

                if (j < train->stationNum - 2) {
                    currentOffset += train->stopoverTimes[j];
                }
                if (j + 1 < train->stationNum) {
                    train->stations[j + 1].departureOffset = currentOffset;
                }
            }

            if (tickets.addTrain(train)) {
                cout << "0\n";
            } else {
                delete train;
                cout << "-1\n";
            }
        } else if (command == "release_train") {
            string i;
            bool hasI = false;
            for (int j = 0; j < args.getSize(); j++) {
                if (args[j].key == 'i') { i = args[j].value; hasI = true; }
            }
            if (!hasI) {
                cout << "-1\n";
                continue;
            }
            if (tickets.releaseTrain(i)) {
                cout << "0\n";
            } else {
                cout << "-1\n";
            }
        } else if (command == "query_train") {
            string i, d;
            bool hasI = false, hasD = false;
            for (int j = 0; j < args.getSize(); j++) {
                if (args[j].key == 'i') { i = args[j].value; hasI = true; }
                else if (args[j].key == 'd') { d = args[j].value; hasD = true; }
            }
            if (!hasI || !hasD) {
                cout << "-1\n";
                continue;
            }
            Train* train = tickets.getTrain(i);
            if (train == nullptr) {
                cout << "-1\n";
                continue;
            }
            Date startDate = train->saleStart;
            cout << train->trainID << " " << train->type << "\n";

            for (int j = 0; j < train->stationNum; j++) {
                DateTime arr = train->getArrivalTime(startDate, j);
                DateTime dep = train->getDepartureTime(startDate, j);
                string seatStr;

                if (j == train->stationNum - 1) {
                    seatStr = "x";
                } else {
                    int avail = tickets.getAvailableSeats(train, d, j, j + 1);
                    char buf[20];
                    sprintf(buf, "%d", avail);
                    seatStr = buf;
                }

                string arrStr;
                if (j == 0) {
                    arrStr = "xx-xx xx:xx";
                } else {
                    arrStr = arr.dateString() + " " + arr.timeString();
                }

                string depStr;
                if (j == train->stationNum - 1) {
                    depStr = "xx-xx xx:xx";
                } else {
                    depStr = dep.dateString() + " " + dep.timeString();
                }

                cout << train->stations[j].name << " " << arrStr << " -> " << depStr << " " << train->stations[j].cumulativePrice << " " << seatStr << "\n";
            }
        } else if (command == "delete_train") {
            string i;
            bool hasI = false;
            for (int j = 0; j < args.getSize(); j++) {
                if (args[j].key == 'i') { i = args[j].value; hasI = true; }
            }
            if (!hasI) {
                cout << "-1\n";
                continue;
            }
            if (tickets.deleteTrain(i)) {
                cout << "0\n";
            } else {
                cout << "-1\n";
            }
        } else if (command == "query_ticket") {
            string s, t, d, p;
            bool hasS = false, hasT = false, hasD = false;
            bool sortByTime = false;
            for (int j = 0; j < args.getSize(); j++) {
                if (args[j].key == 's') { s = args[j].value; hasS = true; }
                else if (args[j].key == 't') { t = args[j].value; hasT = true; }
                else if (args[j].key == 'd') { d = args[j].value; hasD = true; }
                else if (args[j].key == 'p') { p = args[j].value; if (p == "time") sortByTime = true; else sortByTime = false; }
            }
            if (!hasS || !hasT || !hasD) {
                cout << "-1\n";
                continue;
            }

            DynamicArray<Train*> allTrains;
            tickets.getAllTrains(allTrains);

            DynamicArray<QueryTicketResult> results;

            Date queryDate = Date::fromString(d);

            for (int j = 0; j < allTrains.getSize(); j++) {
                Train* train = allTrains[j];
                if (queryDate.dayOfYear() < train->saleStart.dayOfYear() || queryDate.dayOfYear() > train->saleEnd.dayOfYear()) {
                    continue;
                }

                int fromIdx = train->getStationIndex(s);
                int toIdx = train->getStationIndex(t);
                if (fromIdx < 0 || toIdx < 0 || fromIdx >= toIdx) {
                    continue;
                }

                // Check that departure from s is on the requested date
                int daysOffset = queryDate.dayOfYear() - train->saleStart.dayOfYear();
                DateTime dep = train->getDepartureTime(Date(train->saleStart.month, train->saleStart.day + daysOffset), fromIdx);
                if (dep.date.compare(queryDate) != 0) {
                    continue;
                }

                QueryTicketResult res;
                res.train = train;
                res.fromIdx = fromIdx;
                res.toIdx = toIdx;
                res.price = train->stations[toIdx].cumulativePrice - train->stations[fromIdx].cumulativePrice;
                res.travelTime = train->calculateTravelTime(fromIdx, toIdx);
                res.availableSeats = tickets.getAvailableSeats(train, d, fromIdx, toIdx);
                results.pushBack(res);
            }

            sortQueryResults(results, sortByTime);

            cout << results.getSize() << "\n";
            for (int j = 0; j < results.getSize(); j++) {
                QueryTicketResult& res = results[j];
                Train* train = res.train;
                Date startDate = train->saleStart;
                int daysOffset = queryDate.dayOfYear() - startDate.dayOfYear();
                DateTime dep = train->getDepartureTime(Date(startDate.month, startDate.day + daysOffset), res.fromIdx);
                DateTime arr = train->getArrivalTime(Date(startDate.month, startDate.day + daysOffset), res.toIdx);

                cout << res.train->trainID << " " << s << " " << dep.dateString() << " " << dep.timeString() << " -> " << t << " " << arr.dateString() << " " << arr.timeString() << " " << res.price << " " << res.availableSeats << "\n";
            }
        } else if (command == "query_transfer") {
            string s, t, d, p;
            bool hasS = false, hasT = false, hasD = false;
            bool sortByTime = false;
            for (int j = 0; j < args.getSize(); j++) {
                if (args[j].key == 's') { s = args[j].value; hasS = true; }
                else if (args[j].key == 't') { t = args[j].value; hasT = true; }
                else if (args[j].key == 'd') { d = args[j].value; hasD = true; }
                else if (args[j].key == 'p') { p = args[j].value; if (p == "time") sortByTime = true; else sortByTime = false; }
            }
            if (!hasS || !hasT || !hasD) {
                cout << "0\n";
                continue;
            }

            Date queryDate = Date::fromString(d);
            DynamicArray<Train*> allTrains;
            tickets.getAllTrains(allTrains);

            DynamicArray<TransferResult> results;

            for (int j = 0; j < allTrains.getSize(); j++) {
                Train* firstTrain = allTrains[j];
                if (queryDate.dayOfYear() < firstTrain->saleStart.dayOfYear() || queryDate.dayOfYear() > firstTrain->saleEnd.dayOfYear()) {
                    continue;
                }

                int fromIdx = firstTrain->getStationIndex(s);
                if (fromIdx < 0) continue;

                for (int midIdx = fromIdx + 1; midIdx < firstTrain->stationNum; midIdx++) {
                    string midStation = firstTrain->stations[midIdx].name;
                    DateTime firstArrival = firstTrain->getArrivalTime(firstTrain->saleStart, midIdx);
                    DateTime firstDeparture = firstTrain->getDepartureTime(firstTrain->saleStart, fromIdx);

                    if (firstDeparture.date.compare(queryDate) != 0) {
                        int daysOffset = queryDate.dayOfYear() - firstTrain->saleStart.dayOfYear();
                        firstDeparture = firstTrain->getDepartureTime(Date(firstTrain->saleStart.month, firstTrain->saleStart.day + daysOffset), fromIdx);
                        if (firstDeparture.date.compare(queryDate) != 0) {
                            continue;
                        }
                        firstArrival = firstTrain->getArrivalTime(Date(firstTrain->saleStart.month, firstTrain->saleStart.day + daysOffset), midIdx);
                    }

                    for (int k = 0; k < allTrains.getSize(); k++) {
                        if (j == k) continue;
                        Train* secondTrain = allTrains[k];

                        int secondMidIdx = secondTrain->getStationIndex(midStation);
                        int secondToIdx = secondTrain->getStationIndex(t);
                        if (secondMidIdx < 0 || secondToIdx < 0 || secondMidIdx >= secondToIdx) continue;

                        if (firstArrival.totalMinutes() > secondTrain->getDepartureTime(secondTrain->saleStart, secondMidIdx).totalMinutes()) {
                            continue;
                        }

                        // Check if the second train's departure from mid is on or after first arrival
                        // date must be valid
                        if (queryDate.dayOfYear() < secondTrain->saleStart.dayOfYear() || queryDate.dayOfYear() > secondTrain->saleEnd.dayOfYear()) {
                            continue;
                        }

                        int daysOffset = queryDate.dayOfYear() - secondTrain->saleStart.dayOfYear();
                        DateTime secondDep = secondTrain->getDepartureTime(Date(secondTrain->saleStart.month, secondTrain->saleStart.day + daysOffset), secondMidIdx);
                        if (secondDep.totalMinutes() < firstArrival.totalMinutes()) {
                            continue;
                        }

                        DateTime secondArr = secondTrain->getArrivalTime(Date(secondTrain->saleStart.month, secondTrain->saleStart.day + daysOffset), secondToIdx);

                        QueryTicketResult firstRes;
                        firstRes.train = firstTrain;
                        firstRes.fromIdx = fromIdx;
                        firstRes.toIdx = midIdx;
                        firstRes.price = firstTrain->stations[midIdx].cumulativePrice - firstTrain->stations[fromIdx].cumulativePrice;
                        firstRes.travelTime = firstTrain->calculateTravelTime(fromIdx, midIdx);
                        firstRes.availableSeats = tickets.getAvailableSeats(firstTrain, d, fromIdx, midIdx);

                        QueryTicketResult secondRes;
                        string secondDateStr = secondDep.dateString();
                        secondRes.train = secondTrain;
                        secondRes.fromIdx = secondMidIdx;
                        secondRes.toIdx = secondToIdx;
                        secondRes.price = secondTrain->stations[secondToIdx].cumulativePrice - secondTrain->stations[secondMidIdx].cumulativePrice;
                        secondRes.travelTime = secondTrain->calculateTravelTime(secondMidIdx, secondToIdx);
                        secondRes.availableSeats = tickets.getAvailableSeats(secondTrain, secondDateStr, secondMidIdx, secondToIdx);

                        TransferResult tr;
                        tr.first = firstRes;
                        tr.second = secondRes;
                        tr.totalPrice = firstRes.price + secondRes.price;
                        tr.totalTime = (secondArr.totalMinutes() - firstDeparture.totalMinutes());
                        results.pushBack(tr);
                    }
                }
            }

            if (results.getSize() == 0) {
                cout << "0\n";
                continue;
            }

            // Find the optimal result
            TransferResult* best = &results[0];
            for (int j = 1; j < results.getSize(); j++) {
                bool better;
                if (sortByTime) {
                    if (results[j].totalTime != best->totalTime) {
                        better = results[j].totalTime < best->totalTime;
                    } else if (results[j].first.travelTime != best->first.travelTime) {
                        better = results[j].first.travelTime < best->first.travelTime;
                    } else {
                        better = results[j].first.train->trainID < best->first.train->trainID;
                    }
                } else {
                    if (results[j].totalPrice != best->totalPrice) {
                        better = results[j].totalPrice < best->totalPrice;
                    } else {
                        better = results[j].first.train->trainID < best->first.train->trainID;
                    }
                }
                if (better) {
                    best = &results[j];
                }
            }

            // Output best result
            QueryTicketResult& first = best->first;
            QueryTicketResult& second = best->second;

            Train* firstTrain = first.train;
            Date firstQueryDate = queryDate;
            int daysOffset = firstQueryDate.dayOfYear() - firstTrain->saleStart.dayOfYear();
            DateTime firstDep = firstTrain->getDepartureTime(Date(firstTrain->saleStart.month, firstTrain->saleStart.day + daysOffset), first.fromIdx);
            DateTime firstArr = firstTrain->getArrivalTime(Date(firstTrain->saleStart.month, firstTrain->saleStart.day + daysOffset), first.toIdx);

            Train* secondTrain = second.train;
            Date secondQueryDate = firstArr.date;
            if (secondQueryDate.dayOfYear() >= secondTrain->saleStart.dayOfYear() && secondQueryDate.dayOfYear() <= secondTrain->saleEnd.dayOfYear()) {
                // OK
            } else {
                secondQueryDate = queryDate;
            }
            int secondDaysOffset = secondQueryDate.dayOfYear() - secondTrain->saleStart.dayOfYear();
            DateTime secondDep = secondTrain->getDepartureTime(Date(secondTrain->saleStart.month, secondTrain->saleStart.day + secondDaysOffset), second.fromIdx);
            DateTime secondArr = secondTrain->getArrivalTime(Date(secondTrain->saleStart.month, secondTrain->saleStart.day + secondDaysOffset), second.toIdx);

            cout << first.train->trainID << " " << s << " " << firstDep.dateString() << " " << firstDep.timeString() << " -> " << first.train->stations[first.toIdx].name << " " << firstArr.dateString() << " " << firstArr.timeString() << " " << first.price << " " << first.availableSeats << "\n";
            cout << second.train->trainID << " " << second.train->stations[second.fromIdx].name << " " << secondDep.dateString() << " " << secondDep.timeString() << " -> " << t << " " << secondArr.dateString() << " " << secondArr.timeString() << " " << second.price << " " << second.availableSeats << "\n";

        } else if (command == "buy_ticket") {
            string u, i, d, f, t;
            int n = 0;
            bool queueAllowed = false;
            bool hasU = false, hasI = false, hasD = false, hasN = false, hasF = false, hasT = false;

            for (int j = 0; j < args.getSize(); j++) {
                if (args[j].key == 'u') { u = args[j].value; hasU = true; }
                else if (args[j].key == 'i') { i = args[j].value; hasI = true; }
                else if (args[j].key == 'd') { d = args[j].value; hasD = true; }
                else if (args[j].key == 'n') { n = atoi(args[j].value.c_str()); hasN = true; }
                else if (args[j].key == 'f') { f = args[j].value; hasF = true; }
                else if (args[j].key == 't') { t = args[j].value; hasT = true; }
                else if (args[j].key == 'q') { if (args[j].value == "true") queueAllowed = true; else queueAllowed = false; }
            }

            if (!hasU || !hasI || !hasD || !hasN || !hasF || !hasT) {
                cout << "-1\n";
                continue;
            }

            if (n <= 0) {
                cout << "-1\n";
                continue;
            }

            if (!tickets.isLoggedIn(u)) {
                cout << "-1\n";
                continue;
            }

            Train* train = tickets.getTrain(i);
            if (train == nullptr) {
                cout << "-1\n";
                continue;
            }

            if (!train->released) {
                cout << "-1\n";
                continue;
            }

            Date date = Date::fromString(d);
            if (date.dayOfYear() < train->saleStart.dayOfYear() || date.dayOfYear() > train->saleEnd.dayOfYear()) {
                cout << "-1\n";
                continue;
            }

            int fromIdx = train->getStationIndex(f);
            int toIdx = train->getStationIndex(t);
            if (fromIdx < 0 || toIdx < 0 || fromIdx >= toIdx) {
                cout << "-1\n";
                continue;
            }

            // Check departure date matches query date (departure from f)
            int daysOffset = date.dayOfYear() - train->saleStart.dayOfYear();
            DateTime dep = train->getDepartureTime(Date(train->saleStart.month, train->saleStart.day + daysOffset), fromIdx);
            if (dep.date.compare(date) != 0) {
                cout << "-1\n";
                continue;
            }

            int price = train->stations[toIdx].cumulativePrice - train->stations[fromIdx].cumulativePrice;
            int totalPrice = price * n;

            bool success = tickets.purchaseTickets(train, d, fromIdx, toIdx, n);
            if (success) {
                Order* order = new Order(tickets.getNextOrderId(), u, i, date, fromIdx, toIdx, n, totalPrice, ORDER_SUCCESS);
                tickets.addOrder(order);
                cout << totalPrice << "\n";
            } else {
                if (queueAllowed && n <= train->seatNum) {
                    Order* order = new Order(tickets.getNextOrderId(), u, i, date, fromIdx, toIdx, n, totalPrice, ORDER_PENDING);
                    tickets.addOrder(order);
                    WaitingQueue* queue = tickets.getWaitingQueue(i, d);
                    if (queue != nullptr) {
                        queue->enqueue(order);
                    }
                    cout << "queue\n";
                } else {
                    cout << "-1\n";
                }
            }

        } else if (command == "query_order") {
            string u;
            bool hasU = false;
            for (int j = 0; j < args.getSize(); j++) {
                if (args[j].key == 'u') { u = args[j].value; hasU = true; }
            }
            if (!hasU) {
                cout << "-1\n";
                continue;
            }
            if (!tickets.isLoggedIn(u)) {
                cout << "-1\n";
                continue;
            }

            DynamicArray<Order*>* orders = tickets.getUserOrders(u);
            if (orders == nullptr) {
                cout << "0\n";
                return 0;
            }

            cout << orders->getSize() << "\n";
            for (int j = orders->getSize() - 1; j >= 0; j--) {
                Order* order = (*orders)[j];
                Train* train = tickets.getTrain(order->trainID);
                if (train == nullptr) {
                    continue; // shouldn't happen
                }

                string statusStr;
                switch (order->status) {
                    case ORDER_SUCCESS: statusStr = "success"; break;
                    case ORDER_PENDING: statusStr = "pending"; break;
                    case ORDER_REFUNDED: statusStr = "refunded"; break;
                }

                int daysOffset = order->departureDate.dayOfYear() - train->saleStart.dayOfYear();
                DateTime dep = train->getDepartureTime(Date(train->saleStart.month, train->saleStart.day + daysOffset), order->fromStationIdx);
                DateTime arr = train->getArrivalTime(Date(train->saleStart.month, train->saleStart.day + daysOffset), order->toStationIdx);

                string fromStation = train->stations[order->fromStationIdx].name;
                string toStation = train->stations[order->toStationIdx].name;

                cout << "[" << statusStr << "] " << order->trainID << " " << fromStation << " "
                     << dep.dateString() << " " << dep.timeString() << " -> " << toStation << " "
                     << arr.dateString() << " " << arr.timeString() << " " << order->totalPrice << " " << order->numTickets << "\n";
            }

        } else if (command == "refund_ticket") {
            string u;
            int n = 1;
            bool hasU = false;
            for (int j = 0; j < args.getSize(); j++) {
                if (args[j].key == 'u') { u = args[j].value; hasU = true; }
                else if (args[j].key == 'n') { n = atoi(args[j].value.c_str()); }
            }
            if (!hasU) {
                cout << "-1\n";
                continue;
            }
            if (!tickets.isLoggedIn(u)) {
                cout << "-1\n";
                continue;
            }

            DynamicArray<Order*>* orders = tickets.getUserOrders(u);
            if (orders == nullptr || n > orders->getSize() || n < 1) {
                cout << "-1\n";
                continue;
            }

            // n-th from newest to oldest = size - n in 0-index
            int idx = orders->getSize() - n;
            Order* order = (*orders)[idx];

            if (order->status == ORDER_REFUNDED) {
                cout << "-1\n";
                continue;
            }

            if (order->status == ORDER_PENDING) {
                WaitingQueue* queue = tickets.getWaitingQueue(order->trainID, order->departureDate.toString());
                if (queue != nullptr) {
                    queue->removeOrder(order);
                }
                order->status = ORDER_REFUNDED;
                cout << "0\n";
                return 0;
            }

            order->status = ORDER_REFUNDED;
            Train* train = tickets.getTrain(order->trainID);
            tickets.refundTickets(train, order->departureDate.toString(), order->fromStationIdx, order->toStationIdx, order->numTickets);
            cout << "0\n";

        } else if (command == "clean") {
            tickets.clean();
            cout << "0\n";
        } else if (command == "exit") {
            cout << "bye\n";
            return 0;
        } else {
            cout << "-1\n";
        }
    }

    return 0;
}
