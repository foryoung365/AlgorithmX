#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <chrono>

#include "Solver.h"
#include "Utilities.h"

using namespace AlgorithmX;
using namespace std;

//void MrsKnuthI() {
//    istringstream sin(
//        R"(M 9 10 11 1 2 3 4 Tu 9 10 11 1 2 3 4 W Th F 8 9 10 11 1 2 3
//21
//Bo Trumpet M Tu 11 W Th F 10 2
//Kai Baritone M 9 2 Tu W Th F 2
//Dave Tuba M 3 4 Tu W Th F 3
//Noah Flute M 9 Tu 9 11 1 W Th F 10
//Luna Saxophone M 2 Tu 3 4 W Th F
//Ben Saxophone M 10 Tu 1 3 W Th F
//Isla Flute M 3 Tu 11 W Th F 2
//Ella Clarinet M 9 11 Tu 3 W Th F 11
//Adam Clarinet M 10 1 Tu 3 W Th F 1
//Bob Drums M 9 1 Tu 3 W Th F 3
//Sara Saxophone M 11 Tu 4 W Th F 8 1
//Cole Baritone M 11 2 Tu 2 4 W Th F 2
//Ava Drums M Tu 10 3 W Th F 10 2
//Mae Trumpet M 9 Tu 9 W Th F 10
//Anna Tuba M 9 11 Tu 10 W Th F
//Coop Clarinet M 1 2 4 Tu 11 4 W Th F
//Mia Tuba M 1 4 Tu 3 W Th F 9 11
//Rae Flute M 11 4 Tu 9 W Th F 9 1
//Ayla Baritone M 1 Tu 11 W Th F 8
//Levi Trumpet M Tu 9 11 W Th F 9 11
//Olly Drums M 4 Tu W Th F 10 )");
//
//    string teacher_availability;
//    getline(sin, teacher_availability);
//    istringstream ss(teacher_availability);
//    using R = tuple<string, string, string, int>;
//    vector<R> requirements;
//
//    string day;
//    vector<string> teachingDays;
//    while (!ss.eof()) {
//        string str;
//        ss >> day;
//        bool bAvailable = false;
//
//        int hour = 0;
//        while (ss >> hour) {
//            requirements.push_back(
//                std::make_tuple("slot filled", "teacher", day, hour));
//            bAvailable = true;
//        }
//        if (ss.fail() && !ss.eof()) {
//            ss.clear();
//        }
//
//        if (bAvailable) {
//            teachingDays.push_back(day);
//        }
//    }
//
//    using A = tuple<string, string, string, string, int>;
//    vector<A> actions;
//    int num_students;
//    sin >> num_students;
//    sin.ignore();
//    for (int i = 0; i < num_students; i++) {
//        string student_data;
//        string name;
//        getline(sin, student_data);
//
//        istringstream is(student_data);
//        is >> student_data;
//        requirements.push_back(
//            std::make_tuple("student scheduled", student_data, "", 0));
//        is >> name;
//        for (auto d : teachingDays) {
//            auto req = std::make_tuple(string("Instrument on day"), name, d, 0);
//            if (find(requirements.begin(), requirements.end(), req) !=
//                requirements.end())
//                continue;
//            requirements.push_back(req);
//        }
//
//        while (!is.eof()) {
//            string str;
//            is >> day;
//
//            int hour = 0;
//            char szDesc[256];
//            while (is >> hour) {
//                snprintf(szDesc, 256, "%s_%s_%s_%d", student_data.c_str(),
//                         name.c_str(), day.c_str(), hour);
//                actions.push_back(
//                    std::make_tuple(szDesc, student_data, name, day, hour));
//            }
//            if (is.fail() && !is.eof()) {
//                is.clear();
//            }
//        }
//    }
//
//    cerr << "requirements:" << endl;
//    for (auto req : requirements) {
//        print_tuple(req);
//    }
//
//    cerr << "actions:" << endl;
//    unordered_map<string, vector<R>> mapActions;
//    for (auto action : actions) {
//        for (auto req : requirements) {
//            if (get<0>(req) == "slot filled" &&
//                (get<3>(action) == get<2>(req) &&
//                 get<4>(action) == get<3>(req))) {
//                mapActions[get<0>(action)].push_back(req);
//            } else if (get<0>(req) == "student scheduled" &&
//                       get<1>(action) == get<1>(req)) {
//                mapActions[get<0>(action)].push_back(req);
//            } else if (get<0>(req) == "Instrument on day" &&
//                       get<2>(action) == get<1>(req) &&
//                       get<3>(action) == get<2>(req)) {
//                mapActions[get<0>(action)].push_back(req);
//            }
//        }
//    }
//
//    for (auto [actionid, reqs] : mapActions) {
//        cerr << actionid << ": {" << endl;
//        for (auto req : reqs) {
//            print_tuple(req);
//        }
//        cerr << "}" << endl;
//    }
//
//    Solver s(requirements, mapActions, {});
//
//    // s.DumpMatrix();
//
//    int hours[] = {8, 9, 10, 11, 1, 2, 3, 4};
//    string weekdays[] = {"M", "Tu", "W", "Th", "F"};
//    auto IndexOfHour = [&hours](int nHour) {
//        for (int i = 0; i < std::size(hours); ++i) {
//            if (nHour == hours[i]) {
//                return i;
//            }
//        }
//
//        return -1;
//    };
//    auto IndexOfWeek = [&weekdays](string day) {
//        for (int i = 0; i < std::size(weekdays); ++i) {
//            if (day == weekdays[i]) {
//                return i;
//            }
//        }
//        return -1;
//    };
//    auto GetReq = [&actions](const string& desc) {
//        for (auto req : actions) {
//            if (desc == get<0>(req)) return req;
//        }
//        return A();
//    };
//    vector<vector<string>> result(std::size(hours),
//                                  vector<string>(5, "--------------"));
//
//    s.Solve([&](vector<string> solution) {
//        cerr << "find solution:";
//        for (auto s : solution) {
//            cerr << s << "->";
//            auto req = GetReq(s);
//            int nHour = IndexOfHour(get<4>(req));
//            string day = get<3>(req);
//            int nIdx = IndexOfWeek(day);
//            result[nHour][nIdx] = get<1>(req) + "/" + get<2>(req);
//        }
//        cerr << endl;
//    });
//    // Write an answer using cout. DON'T FORGET THE "<< endl"
//    // To debug: cerr << "Debug messages..." << endl;
//
//    cout << "       Monday        Tuesday       Wednesday       Thursday       "
//            " Friday"
//         << endl;
//    int i = 0;
//    for (; i < 4; ++i) {
//        cout << setw(2) << hours[i] << " ";
//        for (int j = 0; j < std::size(weekdays); ++j) {
//            print_centered(result[i][j], 14, j == std::size(weekdays) - 1);
//            if (j != std::size(weekdays) - 1) {
//                cout << " ";
//            }
//        }
//        cout << endl;
//    }
//
//    cout << "       LUNCH          LUNCH          LUNCH          LUNCH" << endl;
//
//    for (; i < std::size(hours); ++i) {
//        cout << setw(2) << hours[i] << " ";
//        for (int j = 0; j < std::size(weekdays); ++j) {
//            print_centered(result[i][j], 14, j == std::size(weekdays) - 1);
//            if (j != std::size(weekdays) - 1) {
//                cout << " ";
//            }
//        }
//        cout << endl;
//    }
//
//    getchar();
//}

//void Sudoku9() {
//    using R =
//        tuple<string /*desc*/, int /*row*/, int /*col*/, string /*value*/>;
//    using A =
//        tuple<string /*desc*/, int /*row*/, int /*col*/, string /*value*/>;
//    using P = vector<vector<string>>;
//
//    vector<R> requirements;
//    vector<A> actions;
//    unordered_map<ActionID, vector<R>> mapActions;
//
//    vector<string> vecValues = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};
//    P prefills(vecValues.size(), vector<string>(vecValues.size(), "0"));
//    istringstream sin(R"(120070560
//507932080
//000001000
//010240050
//308000402
//070085010
//000700000
//080423701
//034010028)");
//
//    for (int i = 0; i < 9; i++) {
//        string line;
//        getline(sin, line);
//        for (int j = 0; j < line.size(); ++j) {
//            prefills[i][j] = line[j];
//        }
//    }
//
//    int nSize = vecValues.size();
//    const int nBoxSize = 3;
//
//    char szDesc[256];
//
//    auto FillRequirements = [&](string desc, int r, int c, string value) {
//        actions.push_back(make_tuple(desc, r, c, value));
//        int nBoxIdx = r / nBoxSize * nBoxSize + c / nBoxSize;
//        mapActions[desc].push_back(make_tuple("value in column", c, 0, value));
//        mapActions[desc].push_back(make_tuple("value in row", r, 0, value));
//        mapActions[desc].push_back(make_tuple("slot filled", r, c, "0"));
//        mapActions[desc].push_back(
//            make_tuple("value in box", nBoxIdx, 0, value));
//    };
//
//    for (int r = 0; r < nSize; ++r) {
//        for (int v = 0; v < vecValues.size(); ++v) {
//            requirements.push_back(
//                make_tuple("value in column", r, 0, vecValues[v]));
//            requirements.push_back(
//                make_tuple("value in row", r, 0, vecValues[v]));
//        }
//        for (int c = 0; c < nSize; ++c) {
//            requirements.push_back(make_tuple("slot filled", r, c, "0"));
//            int nBox = (r * nSize + c) / (nBoxSize*nBoxSize);
//            requirements.push_back(
//                make_tuple("value in box", nBox, 0, vecValues[c]));
//            for (int v = 0; v < vecValues.size(); ++v) {
//                if (prefills[r][c] != "0") {
//                    snprintf(szDesc, 256, "place %d %d %s", r, c,
//                             prefills[r][c].c_str());
//                    FillRequirements(szDesc, r, c, prefills[r][c]);
//                    break;
//                } else {
//                    snprintf(szDesc, 256, "place %d %d %s", r, c,
//                             vecValues[v].c_str());
//                    FillRequirements(szDesc, r, c, vecValues[v]);
//                }
//            }
//        }
//    }
//
//
//    for (auto r : requirements) {
//        print_tuple(r);
//    }
//
//    for (auto [actionid, reqs] : mapActions) {
//        cerr << actionid << ": {" << endl;
//        for (auto req : reqs) {
//            print_tuple(req);
//        }
//        cerr << "}" << endl;
//    }
//    
//    Solver s(requirements, mapActions, {});
//
//    P result(vecValues.size(), vector<string>(vecValues.size(), "0"));
//    s.Solve([&](vector<string> solution) {
//        cerr << "find solution:";
//        for (auto s : solution) {
//            istringstream is(s);
//            string desc,value;
//            int r, c;
//            is >> desc >> r >> c >> value;
//            result[r][c] = value;
//        }
//        cerr << endl;
//    });
//    for (auto row : result) {
//        for (auto col : row) {
//            cout << col;
//        }
//        cout << endl;
//    }
//}

void Sudoku25() {
    using R =
        tuple<string /*desc*/, int /*row*/, int /*col*/, string /*value*/>;
    using A =
        tuple<string /*desc*/, int /*row*/, int /*col*/, string /*value*/>;
    using P = vector<vector<string>>;

    std::chrono::high_resolution_clock hc;

    auto t1 = hc.now();

    vector<R> requirements;
    unordered_map<Solver<R>::ActionID, vector<R>> mapActions;

    vector<string> vecValues;
    for (char ch = 'A'; ch <= 'Y'; ++ch) {
        vecValues.push_back(string(1, ch));
    }
    P prefills(vecValues.size(), vector<string>(vecValues.size(), "."));
    istringstream sin(R"(E.....QRB..W...SOC.G..NMU
QHT.G.YDUFLOBN..I.AP...R.
...C.E.P....AK...X.U.HT.F
PRK..H.O..Q..U..DL.N.....
.Y..DJ.NTLC.VXE.R.....I..
J..EY.WXNKSV.IQ..U..HTBLM
.TIMV......G.....P.W.S...
NKC......QU...B.YIL.W.R..
...HRTA.C..KLJFE.VS.D...I
FGB....V.ST.R..OHM..A..KJ
.Q...D...TI....MN...EF..C
W.M..R....D.Q....B...VLNK
.C.....I.A.PMYV....ETWO..
.I.AONX..B......PFQ.R...D
.F.K..M....NT...AS.J.IXHB
S.....P.W.X.OR.KJ.....V..
...Y......BU...P..ERC...Q
IN.RC..QJ.EA.FGB.T..SK...
KA.V.FR.....J..I...S.U...
...U.V.CO..Y..H.WD.QP.E.R
.BQ.P.OJI.F...KU.G..V...N
M.NLT.KA...C.QI.EY.X.P.F.
G..O.U..LC...BN.F..A...W.
Y..JKQ...XW.EL.NSO.IUMHAG
RU.IA..HVP.X.S..MQB.L....)");

    const int RowSize = 25;
    for (int i = 0; i < RowSize; i++) {
        string line;
        getline(sin, line);
        for (int j = 0; j < line.size(); ++j) {
            prefills[i][j] = line[j];
        }
    }

    auto t2 = hc.now();

    int nSize = vecValues.size();
    const int nBoxSize = 5;

    auto IsValidAction = [&](const P& prefills, int r, int c, string value) {
        // RowSize 是数独的规模 (例如，9x9 数独中 RowSize = 9)
        // nBoxSize 是一个宫格的规模 (例如，9x9 数独中 nBoxSize = 3)

        // 检查行
        for (int i = 0; i < RowSize; i++) {
            if (prefills[r][i] == value) {
                return false;
            }
        }

        // 检查列
        for (int i = 0; i < RowSize; i++) {
            if (prefills[i][c] == value) {
                return false;
            }
        }

        // 检查宫格
        // 正确计算宫格的起始行和列
        int boxStartRow = (r / nBoxSize) * nBoxSize;
        int boxStartCol = (c / nBoxSize) * nBoxSize;

        for (int i = boxStartRow; i < boxStartRow + nBoxSize; i++) {
            for (int j = boxStartCol; j < boxStartCol + nBoxSize; j++) {
                if (prefills[i][j] == value) {
                    return false;
                }
            }
        }

        return true;
    };

    auto FillRequirements = [&](const Solver<R>::ActionID& desc, int r, int c, string value) {
        int nBoxIdx = r / nBoxSize * nBoxSize + c / nBoxSize;
        mapActions[desc].push_back(make_tuple("value in column", c, 0, value));
        mapActions[desc].push_back(make_tuple("value in row", r, 0, value));
        mapActions[desc].push_back(make_tuple("slot filled", r, c, "."));
        mapActions[desc].push_back(
            make_tuple("value in box", nBoxIdx, 0, value));
    };

    for (int r = 0; r < nSize; ++r) {
        for (int v = 0; v < vecValues.size(); ++v) {
            requirements.push_back(
                make_tuple("value in column", r, 0, vecValues[v]));
            requirements.push_back(
                make_tuple("value in row", r, 0, vecValues[v]));
        }
        for (int c = 0; c < nSize; ++c) {
            requirements.push_back(make_tuple("slot filled", r, c, "."));
            int nBox = (r * nSize + c) / (nBoxSize * nBoxSize);
            requirements.push_back(
                make_tuple("value in box", nBox, 0, vecValues[c]));
            for (int v = 0; v < vecValues.size(); ++v) {
                if (prefills[r][c] != ".") {
                    auto t = make_tuple("place", r, c, prefills[r][c]);
                    FillRequirements(t, r, c, prefills[r][c]);
                    break;
                } else {
                    auto t = make_tuple("place", r, c, vecValues[v]);
                    if (!IsValidAction(prefills, r, c, vecValues[v])) continue;

                    FillRequirements(t, r, c, vecValues[v]);
                }
            }
        }
    }

    auto t3 = hc.now();

    class SudokuSolver : Solver<R> {
    public:
        SudokuSolver(const P& prefills, const vector<R>& requirements, const unordered_map<ActionID, vector<R>>& mapActions):Solver(requirements, mapActions, {}), m_prefills(prefills) {
            
        }
        virtual void processRowSelection(DlxCell<R>* row) {
        }
        virtual void processRowUnselection(DlxCell<R>* row){}
    protected:
        P m_prefills;
    };

    Solver s(requirements, mapActions, {});
    auto t4 = hc.now();
    string out(RowSize * (RowSize + 1), '.');
    s.Solve([&](vector<Solver<R>::ActionID> solution) {
        // cerr << "find solution:";
        for (auto s : solution) {
            string desc, value;
            int r, c;
            std::tie(desc, r, c, value) = s;
            out[r * (RowSize + 1) + c] = value[0];
        }
        for (int i = 0; i < RowSize; ++i) {
            out[i * (RowSize + 1) + RowSize] = '\n';
        }

        cout << out;
    });

    auto t5 = hc.now();

    cout << "1->2:"
         << chrono::duration_cast<chrono::microseconds>((t2 - t1)).count()
         << ", 2->3:"
         << chrono::duration_cast<chrono::microseconds>((t3 - t2)).count()
         << ", 3->4:"
         << chrono::duration_cast<chrono::microseconds>((t4 - t3)).count()
         << ", 4->5:"
         << chrono::duration_cast<chrono::microseconds>((t5 - t4)).count()
         << ", total:"
         << chrono::duration_cast<chrono::microseconds>((t5 - t1)).count()
         << endl;
}



int main() {
    Sudoku25();

    return 0;
}