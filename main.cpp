$(head -n 849 /workspace/problem_017/main.cpp)
    void clean() {
        // Delete all users
        // HashTable<User*> → getAll(T** array) → User*** array
        User*** userArray = new User**[users.getSize()];
        int ucount;
        users.getAll(userArray, ucount);
        for (int i = 0; i < ucount; i++) {
            delete *userArray[i];
        }
        delete[] userArray;

        // Delete all trains
        // HashTable<Train*> → getAll(T** array) → Train*** array
        Train*** trainArray = new Train**[trains.getSize()];
        int tcount;
        trains.getAll(trainArray, tcount);
        for (int i = 0; i < tcount; i++) {
            delete *trainArray[i];
        }
        delete[] trainArray;

        // Delete all daily tickets
        // HashTable<HashTable<TicketSale*>*> → getAll(T** array) where T = HashTable<TicketSale*>* → T** = (HashTable<TicketSale*>*)* → HashTable<TicketSale*>* *
        HashTable<TicketSale*>* *dailyArray = new HashTable<TicketSale*>*[dailyTickets.getSize()];
        int dcount;
        dailyTickets.getAll(dailyArray, dcount);
        for (int i = 0; i < dcount; i++) {
            HashTable<TicketSale*>* dateTable = *dailyArray[i];
            TicketSale** *saleArray = new TicketSale* *[dateTable->getSize()];
            int scount;
            dateTable->getAll(saleArray, scount);
            for (int j = 0; j < scount; j++) {
                delete *saleArray[j];
            }
            delete[] saleArray;
            delete dateTable;
        }
        delete[] dailyArray;

        // Delete all waiting queues
        WaitingQueue*** queueArray = new WaitingQueue**[waitingQueues.getSize()];
        int qcount;
        waitingQueues.getAll(queueArray, qcount);
        for (int i = 0; i < qcount; i++) {
            delete *queueArray[i];
        }
        delete[] queueArray;

        // Delete all user order arrays
        DynamicArray<Order*>* *orderArray = new DynamicArray<Order*>*[userOrders.getSize()];
        int ocount;
        userOrders.getAll(orderArray, ocount);
        for (int i = 0; i < ocount; i++) {
            DynamicArray<Order*>* orders = *orderArray[i];
            for (int j = 0; j < orders->getSize(); j++) {
                delete orders[j];
            }
            delete orderArray[i];
        }
        delete[] orderArray;

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
            if (pos >= len) {
                // Empty value invalid
                return false;
            }

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

// Compare functions
bool compareByCost(const QueryTicketResult& a, const QueryTicketResult& b) {
    if (a.price != b.price) {
        return a.price < b.price;
    }
    return a.first.train->trainID < b.first.train->trainID;
}

bool compareByTime(const QueryTicketResult& a, const QueryTicketResult& b) {
    if (a.travelTime != b.travelTime) {
        return a.travelTime < b.travelTime;
    }
    return a.first.train->trainID < b.first.train->trainID;
}
};

// Selection sort (since we can't use STL algorithm)
void sortQueryResults(DynamicArray<QueryTicketResult>& array, bool byTime) {
    int n = array.getSize();
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            bool smaller = byTime ? compareByTime(array[j], array[minIdx]) : compareByCost(array[j], array[minIdx]);
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
                if (!hasC || !tickets.isLoggedIn(c)) {
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
            string newM;
            int newG = 0;
            bool hasC = false, hasU = false;
            bool newPhasP = false, newPhasN = false, newPhasM = false, hasG = false;
            for (int i = 0; i < args.getSize(); i++) {
                if (args[i].key == 'c') { c = args[i].value; hasC = true; }
                else if (args[i].key == 'u') { u = args[i].value; hasU = true; }
                else if (args[i].key == 'p') { newP = args[i].value; newPhasP = true; }
                else if (args[i].key == 'n') { newN = args[i].value; newPhasN = true; }
                else if (args[i].key == 'm') { newM = args[i].value; newPhasM = true; }
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
            if (hasG && newG >= tickets.getUser(c)->privilege) {
                cout << "-1\n";
                continue;
            }
            User* target = tickets.getUser(u);
            if (target == nullptr) {
                cout << "-1\n";
                continue;
            }
            if (newPhasP) target->name = newN;
            if (newPhasM) target->mailAddr = newM;
            if (newG) target->privilege = newG;
            cout << target->username << " " << target->name << " " << target->mailAddr << " " << target->privilege << "\n";
        } else if (command == "add_train") {
            // already handled above
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
            Date date = Date::fromString(d);
            cout << train->trainID << " " << train->type << "\n";
            cout << train->stationNum << "\n";
            for (int j = 0; j < train->stationNum; j++) {
                cout << train->stations[j].name << " ";
                DateTime arr = train->getDepartureTime(j);
                cout << arr.dateString() << " " << arr.timeString() << " " << train->stations[j].cumulativePrice << "\n";
                if (j != train->stationNum - 1) {
                    cout << train->stations[j].remaining << " x\n";
                }
            }
            for (int j = 0; j < train->stationNum; j++) {
                DateTime dep = train->getDepartureTime(j);
                cout << dep.dateString() << " " << dep.timeString() << " " << train->stations[j].cumulativePrice << "\n";
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
            bool sortByTime = false;
            bool hasS = false, hasT = false, hasD = false;
            for (int i = 0; i < args.getSize(); i++) {
                if (args[i].key == 's') { s = args[i].value; hasS = true; }
                else if (args[i].key == 't') { t = args[i].value; hasT = true; }
                else if (args[i].key == 'd') { d = args[i].value; hasD = true; }
                else if (args[i].key == 'p') { p = args[i].value; if (p == "time") sortByTime = true; }
            }
            if (!hasS || !hasD) {
                cout << "-1\n";
                continue;
            }

            DynamicArray<Train*> availableTrains;
            Date queryDate = Date::fromString(d);
            for (int i = 0; i < allTrains.getSize(); i++) {
                Train* train = allTrains[i];
                if (queryDate.dayOfYear() < train->saleStart.dayOfYear() || queryDate.dayOfYear() > train->saleEnd.dayOfYear()) {
                    continue;
                }

                int fromStation = train->getStationIndex(s);
                if (fromStation < 0) continue;

                int fromIdx = train->getStationIndex(t);
                if (fromIdx < 0) continue;

                int avail = tickets.getAvailableSeats(train, fromStation, toIdx);
                if (avail <= 0) continue;

                QueryTicketResult res;
                res.train = train;
                res.fromIdx = fromStation;
                res.toIdx = toIdx;
                res.price = train->calculatePrice(fromStation, toIdx);
                res.travelTime = train->calculateTravelTime(fromStation, toIdx);
                res.available = avail;
                availableTrains.pushBack(res);
            }

            if (availableTrains.getSize() == 0) {
                cout << "0\n";
                continue;
            }

            if (sortByTime) {
                sort(availableTrains, compareByTime);
            } else {
                sort(availableTrains, compareByCost);
            }

            cout << availableTrains.getSize() << "\n";
            for (auto& res : availableTrains) {
                cout << res.train->trainID << " " << s << " " << t << " " << res.price << " " << res.available << "\n";
            }
        } else if (command == "query_transfer") {
            string s, t, d, p;
            bool hasS = false, hasT = false, hasD = false;
            for (int i = 0; i < args.getSize(); i++) {
                if (args[i].key == 's') { s = args[i].value; hasS = true; }
                else if (args[i].key == 't') { t = args[i].value; hasT = true; }
                else if (args[i].key == 'd') { d = args[i].value; hasD = true; }
                else if (args[i].key == 'p') { p = args[i].value; hasD = true; }
            }
            if (!hasS || !hasD) {
                cout << "-1\n";
                continue;
            }
            // Find all results with exactly one transfer, sorted by requested parameter
            DynamicArray<TransferResult> results;
            for (int i = 0; i < allTrains.getSize(); i++) {
                Train* firstTrain = allTrains[i];
                int firstFrom = firstTrain->getStationIndex(s);
                int firstTo = firstTrain->getStationIndex(t);
                if (firstFrom < 0 || firstTo < 0) continue;

                // Check if the first train already contains the station and the departure is after sale date ends
                DateTime firstDeparture = firstTrain->getDepartureTime(firstFrom);
                if (firstDeparture.totalMinutes() < queryDate.totalMinutes()) continue;

                // Check departure is on or after query departure and arrival is before query arrival
                if (firstDeparture.totalMinutes() >= queryDate.totalMinutes()) {
                    // We have a transfer that arrives after query departs, so it's valid
                    TransferResult tr;
                    tr.first = firstTrain;
                    tr.second = secondTrain;
                    int firstStart = firstDeparture;
                    int secondEnd = secondTrain->getStationIndex(t);
                    DateTime secondDeparture = secondTrain->getDepartureTime(firstStart);
                    if (secondDeparture.totalMinutes() >= secondDeparture.totalMinutes()) {
                        tr.first.available = tickets.getAvailableSeats(firstFrom, secondStart);
                        results.pushBack(tr);
                    }
            }

            int count = results.getSize();
            if (count == 0) {
                cout << "0\n";
            } else {
                cout << count << "\n";
                if (sortByTime) {
                    sort(results, compareTransferByTime);
                } else {
                    sort(results, compareTransferByPrice);
                }
                for (auto& tr : results) {
                    cout << tr.first.train->trainID << " " << s << " " << t << " " << d << " " << tr.first.price << " " << tr.first.available << "\n";
                }
            }
        } else if (command == "buy_ticket") {
            string u, i, d, n, f, q;
            bool queue = false;
            bool hasU = false, hasI = false, hasD = false, hasN = false, hasF = false;
            for (int j = 0; j < args.getSize(); j++) {
                if (args[j].key == 'u') { u = args[j].value; hasU = true; }
                else if (args[j].key == 'i') { i = args[j].value; hasI = true; }
                else if (args[j].key == 'd') { d = args[j].value; hasD = true; }
                else if (args[j].key == 'f') { f = args[j].value; hasF = true; }
                else if (args[j].key == 'q') { q = atoi(args[j].value.c_str()); hasQ = true; }
            }
            if (!hasU || !hasD) {
                cout << "-1\n";
                continue;
            }
            if (!tickets.isLoggedIn(u)) {
                cout << "-1\n";
                continue;
            }
            if (q > train->seatNum) {
                cout << "-1\n";
                continue;
            }
            int totalPrice = train->calculatePrice(f, q);
            int totalNumTickets = train->getAvailableSeats(f, q);
            if (totalNumTickets < n) {
                cout << "-1\n";
                continue;
            }
            Order* order = new order(tickets, u, i, d, f, q, totalPrice, ORDER_SUCCESS);
            if (tickets.purchaseTicket(order)) {
                cout << totalPrice << "\n";
            } else {
                cout << "-1\n";
            }
        } else if (command == "refund_ticket") {
            string u, n;
            int o;
            for (int i = 0; i < args.getSize(); i++) {
                if (args[i].key == 'u') { u = args[i].value; hasU = true; }
                else if (args[i].key == 'n') { n = atoi(args[i].value.c_str()); hasN = true; }
                else if (args[i].key == 'o') { o = atoi(args[i].value.c_str()); hasO = true; }
            }
            if (!hasU || !hasN) {
                cout << "-1\n";
                continue;
            }
            DynamicArray<Order*>* userOrders = tickets.getUserOrders(u);
            int orderCount = userOrders->getSize();
            cout << orderCount << "\n";
            for (auto& order : *userOrders) {
                cout << order->username << " " << order->train->id << " " << order->date << " " << order->numTickets << " " << order->status << "\n";
            }
        } else if (command == "clean") {
            tickets.clean();
            cout << "0\n";
        } else if (command == "exit") {
            cout << "bye\n";
        } else {
            cout << "-1\n";
        }
    }

    return 0;
}
