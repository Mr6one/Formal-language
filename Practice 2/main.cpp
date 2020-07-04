#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <gtest/gtest.h>

using namespace std;

enum { shift, reduce, accepted };
enum { None = -1};

typedef tuple<size_t, size_t, char> Point;

class LRParser {
public:
    LRParser(const vector<pair<char, string>>&, const set<char>&);

    void BuildTable();
    bool CheckWord(const string&);
private:
    vector<pair<char, string>> grammar;
    set<char> nonTerminated;
    map<char, set<char>> first;
    vector<set<Point>> states;
    map<pair<int, char>, pair<int, int>> table;

    void ComputeFirstSet();
    void Closure(set<Point>&);
    set<Point> Goto(const set<Point>&, char);
    void AddIntoTable(int, char, int, int);
    void CheckAccept(const set<Point>&, int);
    void CheckReduce(const set<Point>&, int);
};

LRParser::LRParser(const vector<pair<char, string>>& grammar, const set<char>& nonTerminated):
    grammar(grammar), nonTerminated(nonTerminated) {}

void LRParser::ComputeFirstSet() {
    bool flag = true;
    while(flag) {
        flag = false;
        for (auto rule: grammar) {
            set<char> previous = first[rule.first];
            if (rule.second == "") {
                first[rule.first].insert('\0');
                goto check;
            }
            for (size_t i = 0; i < rule.second.length(); ++i) {
                char symbol = rule.second[i];
                if (nonTerminated.find(symbol) != nonTerminated.end()) {
                    bool isEpsilon = false;
                    for (char ch: first[symbol]) {
                        if (ch != '\0') {
                            first[rule.first].insert(ch);
                        } else {
                            isEpsilon = true;
                        }
                    }
                    if (!isEpsilon) {
                        break;
                    }
                    if (i == rule.second.length() - 1) {
                        first[rule.first].insert('\0');
                    }
                } else {
                    first[rule.first].insert(symbol);
                    break;
                }
            }
            check:
            if (previous != first[rule.first]) {
                flag = true;
            }
        }
    }
}

void LRParser::Closure(set<Point>& state) {
    size_t previousSize = 0;
    while (previousSize < state.size()) {
        previousSize = state.size();
        for (auto point: state) {
            auto currentRule = grammar[get<0>(point)];
            if (get<1>(point) == currentRule.second.size() ||
                nonTerminated.find(currentRule.second[get<1>(point)]) == nonTerminated.end()) {
                continue;
            }
            for (size_t i = 0; i < grammar.size(); ++i) {
                if (grammar[i].first == currentRule.second[get<1>(point)]) {
                    if (get<1>(point) + 1 == currentRule.second.size()) {
                        state.insert(Point(i, 0, get<2>(point)));
                    } else if (nonTerminated.find(currentRule.second[get<1>(point) + 1]) == nonTerminated.end()) {
                        state.insert(Point(i, 0, currentRule.second[get<1>(point) + 1]));
                    } else {
                        for (char symbol: first[currentRule.second[get<1>(point) + 1]]) {
                            state.insert(Point(i, 0, symbol));
                        }
                    }
                }
            }
        }
    }
}

set<Point> LRParser::Goto(const set<Point>& state, char symbol) {
    set<Point> newState;
    for (auto rule: state) {
        auto currentRule = grammar[get<0>(rule)];
        if (get<1>(rule) != currentRule.second.length()) {
            if (currentRule.second[get<1>(rule)] == symbol) {
                newState.insert(Point(get<0>(rule), get<1>(rule) + 1, get<2>(rule)));
            }
        }
    }
    Closure(newState);
    return newState;
}

void LRParser::AddIntoTable(int parentNumber, char symbol, int childNumber, int type) {
    if (table.find(make_pair(parentNumber, symbol)) == table.end()) {
        table[make_pair(parentNumber, symbol)] = make_pair(type, childNumber);
    } else if (table[make_pair(parentNumber, symbol)] != make_pair(type, childNumber)) {
        throw "incorrect grammar";
    }
}

void LRParser::CheckAccept(const set<Point>& state, int parentNumber) {
    for (Point rule: state) {
        if (grammar[get<0>(rule)].first == '@' && get<1>(rule) == 1) {
            AddIntoTable(parentNumber, '$', None, accepted);
        }
    }
}

void LRParser::CheckReduce(const set<Point>& state, int parentNumber) {
    for (Point rule: state) {
        if (get<1>(rule) == grammar[get<0>(rule)].second.length() && grammar[get<0>(rule)].first != '@') {
            AddIntoTable(parentNumber, get<2>(rule), get<0>(rule), reduce);
        }
    }
}

void LRParser::BuildTable() {
    ComputeFirstSet();

    grammar.push_back(make_pair('@', "S"));
    set<Point> startState = { Point(grammar.size() - 1, 0, '$') };
    Closure(startState);

    queue<set<Point>> queue_;
    queue_.push(startState);
    set<set<Point>> visited;
    visited.insert(startState);
    map<set<Point>, int> number;
    number[startState] = 0;
    size_t i = 1;
    while (!queue_.empty()) {
        set<Point> state = queue_.front();
        queue_.pop();
        CheckAccept(state, number[state]);
        CheckReduce(state, number[state]);
        for (int ch = 0; ch < 128; ++ch) {
            set<Point> newState = Goto(state, (char)ch);
            if (newState.size() != 0) {
                if (visited.find(newState) == visited.end()) {
                    visited.insert(newState);
                    number[newState] = i;
                    queue_.push(newState);
                    ++i;
                }
                AddIntoTable(number[state], ch, number[newState], shift);
            }
        }
    }
}

bool LRParser::CheckWord(const string& st) {
    string str = st;
    str += '$';
    size_t pos = 0;
    vector<pair<char, int>> lrStack;
    lrStack.push_back(make_pair('\0', 0));
    while (pos < str.length()) {
        char curSymbol = str[pos];
        pair<char, int> change = lrStack.back();
        if (table.find(make_pair(change.second, curSymbol)) != table.end()) {
            if (table[make_pair(change.second, curSymbol)].first == shift) {
                lrStack.push_back(make_pair(curSymbol, table[make_pair(change.second, curSymbol)].second));
                ++pos;
            } else if (table[make_pair(change.second, curSymbol)].first == reduce) {
                auto rule = grammar[table[make_pair(change.second, curSymbol)].second];
                for (size_t i = 0; i < rule.second.length(); ++i) {
                    lrStack.pop_back();
                }
                lrStack.push_back(make_pair(rule.first, table[make_pair(lrStack.back().second, rule.first)].second));
            } else if (table[make_pair(change.second, curSymbol)].first == accepted) {
                return true;
            }
        } else {
            break;
        }
    }
    return false;
}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
