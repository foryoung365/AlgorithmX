#include <iostream>
#include <sstream>
#include <iomanip>
#include <iterator>

#include "Solver.h"
#include "Utilities.h"


using namespace AlgorithmX;
using namespace std;


int main(){
    istringstream sin(R"(M 9 10 11 1 2 3 4 Tu 9 10 11 1 2 3 4 W Th F 8 9 10 11 1 2 3
21
Bo Trumpet M Tu 11 W Th F 10 2
Kai Baritone M 9 2 Tu W Th F 2
Dave Tuba M 3 4 Tu W Th F 3
Noah Flute M 9 Tu 9 11 1 W Th F 10
Luna Saxophone M 2 Tu 3 4 W Th F
Ben Saxophone M 10 Tu 1 3 W Th F
Isla Flute M 3 Tu 11 W Th F 2
Ella Clarinet M 9 11 Tu 3 W Th F 11
Adam Clarinet M 10 1 Tu 3 W Th F 1
Bob Drums M 9 1 Tu 3 W Th F 3
Sara Saxophone M 11 Tu 4 W Th F 8 1
Cole Baritone M 11 2 Tu 2 4 W Th F 2
Ava Drums M Tu 10 3 W Th F 10 2
Mae Trumpet M 9 Tu 9 W Th F 10
Anna Tuba M 9 11 Tu 10 W Th F
Coop Clarinet M 1 2 4 Tu 11 4 W Th F
Mia Tuba M 1 4 Tu 3 W Th F 9 11
Rae Flute M 11 4 Tu 9 W Th F 9 1
Ayla Baritone M 1 Tu 11 W Th F 8
Levi Trumpet M Tu 9 11 W Th F 9 11
Olly Drums M 4 Tu W Th F 10 )");

    string teacher_availability;
    getline(sin, teacher_availability);
    istringstream ss(teacher_availability);
    using R = tuple<string, string, string, int>;
    vector<R> requirements;

    string day;
    vector<string> teachingDays;
    while (!ss.eof()) {
        string str;
        ss >> day;
        bool bAvailable = false;

        int hour = 0;
        while (ss >> hour) {
            requirements.push_back(
                std::make_tuple("slot filled", "teacher", day, hour));
            bAvailable = true;
        }
        if (ss.fail() && !ss.eof()) {
            ss.clear();
        }

        if (bAvailable) {
            teachingDays.push_back(day);
        }
    }

    using A = tuple<string, string, string, string, int>;
    vector<A> actions;
    int num_students;
    sin >> num_students;
    sin.ignore();
    for (int i = 0; i < num_students; i++) {
        string student_data;
        string name;
        getline(sin, student_data);

        istringstream is(student_data);
        is >> student_data;
        requirements.push_back(
            std::make_tuple("student scheduled", student_data, "", 0));
        is >> name;
        for (auto d : teachingDays) {
            auto req = std::make_tuple(string("Instrument on day"), name, d, 0);
            if (find(requirements.begin(), requirements.end(), req) != requirements.end()) continue;
            requirements.push_back(req
                );
        }

        while (!is.eof()) {
            string str;
            is >> day;

            int hour = 0;
            char szDesc[256];
            while (is >> hour) {
                snprintf(szDesc, 256, "%s_%s_%s_%d", student_data.c_str(),
                         name.c_str(), day.c_str(), hour);
                actions.push_back(
                    std::make_tuple(szDesc, student_data, name, day, hour));
            }
            if (is.fail() && !is.eof()) {
                is.clear();
            }
        }
    }

    cerr << "requirements:" << endl;
    for (auto req : requirements) {
        print_tuple(req);
    }

    cerr << "actions:" << endl;
    unordered_map<string, vector<R>> mapActions;
    for (auto action : actions) {
        for (auto req : requirements) {
            if (get<0>(req) == "slot filled" &&
                (get<3>(action) == get<2>(req) &&
                 get<4>(action) == get<3>(req))) {
                mapActions[get<0>(action)].push_back(req);
            } else if (get<0>(req) == "student scheduled" &&
                       get<1>(action) == get<1>(req)) {
                mapActions[get<0>(action)].push_back(req);
            } else if (get<0>(req) == "Instrument on day" &&
                       get<2>(action) == get<1>(req) &&
                       get<3>(action) == get<2>(req)) {
                mapActions[get<0>(action)].push_back(req);
            }
        }
    }

    for (auto [actionid, reqs] : mapActions) {
        cerr << actionid << ": {" << endl;
        for (auto req : reqs) {
            print_tuple(req);
        }
        cerr << "}" << endl;
    }

    Solver s(requirements, mapActions, {});

    //s.DumpMatrix();

    int hours[] = {8, 9, 10, 11, 1, 2, 3, 4};
    string weekdays[] = {"M", "Tu", "W", "Th", "F"};
    auto IndexOfHour = [&hours](int nHour) {
        for (int i = 0; i < std::size(hours); ++i) {
            if (nHour == hours[i]) {
                return i;
            }
        }

        return -1;
    };
    auto IndexOfWeek = [&weekdays](string day) {
        for (int i = 0; i < std::size(weekdays); ++i) {
            if (day == weekdays[i]) {
                return i;
            }
        }
        return -1;
    };
    auto GetReq = [&actions](const string &desc) {
        for (auto req : actions) {
            if (desc == get<0>(req)) return req;
        }
        return A();
    };
    vector<vector<string>> result(std::size(hours), vector<string>(5, "--------------"));

    s.Solve([&](vector<string> solution) {
        cerr << "find solution:";
        for (auto s : solution) {
            cerr << s << "->";
            auto req = GetReq(s);
            int nHour = IndexOfHour(get<4>(req));
            string day = get<3>(req);
            int nIdx = IndexOfWeek(day);
            result[nHour][nIdx] = get<1>(req) + "/" + get<2>(req);
        }
        cerr << endl;
    });
    // Write an answer using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;

    cout << "       Monday        Tuesday       Wednesday       Thursday        Friday" << endl;
    int i = 0;
    for (; i < 4; ++i) {
        cout << setw(2) << hours[i] << " ";
        for (int j = 0; j < std::size(weekdays); ++j) {
            print_centered(result[i][j], 14, j == std::size(weekdays)-1);
            if (j != std::size(weekdays)-1) {
                cout << " ";
            }
        }
        cout << endl;
    }
    
    cout << "       LUNCH          LUNCH          LUNCH          LUNCH" << endl;

    for (; i < std::size(hours); ++i) {
        cout << setw(2) << hours[i] << " ";
        for (int j = 0; j < std::size(weekdays); ++j) {
            print_centered(result[i][j], 14, j == std::size(weekdays) - 1);
            if (j != std::size(weekdays) - 1) {
                cout << " ";
            }
        }
        cout << endl;
    }
  
    getchar();
}