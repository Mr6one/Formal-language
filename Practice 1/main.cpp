#include <iostream>
#include <stack>
#include <vector>
#include <queue>
#include <set>
#include <map>

using namespace std;

struct Node{
    vector<pair<int, char>> to;
    bool finalState = false;
};

class DFA{
public:
    DFA(vector<set<pair<int, char>>>&, int, vector<bool>&);

    size_t SubstringSearching(const string&);
private:
    vector<set<pair<int, char>>> dfa;
    int startState;
    vector<bool> finishedStates;

    void DFS(int, size_t, const string&, size_t&, size_t);
};

DFA::DFA(vector<set<pair<int, char>>>& dfa, int startState, vector<bool>& finishedStates):dfa(dfa), startState(startState), finishedStates(finishedStates){}

void DFA::DFS(int currenNode, size_t currentPosition, const string& str, size_t& m, size_t i){
    for (auto it: dfa[currenNode]){
        if (currentPosition < str.size() && it.second == str[currentPosition]){
            if (currentPosition + 1 - i > m){
                m = currentPosition + 1 - i;
            }
            DFS(it.first, currentPosition + 1, str, m, i);
        }
    }
}

size_t DFA::SubstringSearching(const string& str){
    size_t maxLength = 0;
    for (size_t i = 0; i < str.size(); ++i){
        for (size_t j = 0; j < dfa.size(); ++j){
            DFS(j, i, str, maxLength, i);
        }
    }
    return maxLength;
}

class EpsilonNFA{
public:
    EpsilonNFA(const string&);

    DFA ConverToDFA();
private:
    vector<Node> epsilonNFA;
    stack<int> st;
    size_t sz;
    int startState;

    void AddSymbol(char);
    void Concatenation();
    void Union();
    void KleeneStar();
    vector<set<pair<int, char>>> EpsilonClosure(vector<bool>&);
    void DFS(int, set<pair<int, char>>&, vector<bool>&, char);
    DFA BuildDFA(vector<set<pair<int, char>>>&, vector<bool>&);
    vector<set<pair<int, char>>> Rename(map<pair<set<int>, char>, set<int>>&, map<set<int>, bool>&, vector<bool>&, int&);
};

void EpsilonNFA::AddSymbol(char symbol){
    epsilonNFA[sz].to.push_back(make_pair(sz + 1, symbol));
    st.push(sz);
    ++sz;
    st.push(sz);
    ++sz;
}

void EpsilonNFA::Concatenation(){
    int secondEnd = st.top();
    st.pop();
    int secondStart = st.top();
    st.pop();
    int firstEnd = st.top();
    st.pop();
    int firstStart = st.top();
    st.pop();
    epsilonNFA[firstEnd].to.push_back(make_pair(secondStart, '$'));
    st.push(firstStart);
    st.push(secondEnd);
}

void EpsilonNFA::Union(){
    int secondEnd = st.top();
    st.pop();
    int secondStart = st.top();
    st.pop();
    int firstEnd = st.top();
    st.pop();
    int firstStart = st.top();
    st.pop();
    epsilonNFA[sz].to.push_back(make_pair(firstStart, '$'));
    epsilonNFA[sz].to.push_back(make_pair(secondStart, '$'));
    epsilonNFA[secondEnd].to.push_back(make_pair(sz + 1, '$'));
    epsilonNFA[firstEnd].to.push_back(make_pair(sz + 1, '$'));
    st.push(sz);
    ++sz;
    st.push(sz);
    ++sz;
}

void EpsilonNFA::KleeneStar(){
    int end_ = st.top();
    st.pop();
    int start = st.top();
    st.pop();
    epsilonNFA[sz].to.push_back(make_pair(start, '$'));
    epsilonNFA[end_].to.push_back(make_pair(sz + 1, '$'));
    epsilonNFA[end_].to.push_back(make_pair(start, '$'));
    epsilonNFA[sz].to.push_back(make_pair(sz + 1, '$'));
    st.push(sz);
    ++sz;
    st.push(sz);
    ++sz;
}

EpsilonNFA::EpsilonNFA(const string& regExpr):epsilonNFA(regExpr.size() * 2), sz(0){
    for (size_t i = 0; i < regExpr.size(); ++i){
        if ((regExpr[i] - 'a' >= 0 && regExpr[i] - 'a' < 3) || (regExpr[i] == '1')){
            AddSymbol(regExpr[i]);
        } else if (regExpr[i] == '.'){
            Concatenation();
        } else if (regExpr[i] == '+'){
            Union();
        } else if (regExpr[i] == '*'){
            KleeneStar();
        }
    }
    int finalState = st.top();
    st.pop();
    epsilonNFA[finalState].finalState = true;
    startState = st.top();
    st.pop();
}

void EpsilonNFA::DFS(int currentNode, set<pair<int, char>>& waysTable, vector<bool>& visited, char symbol){
    visited[currentNode] = true;
    for (pair<int, char> nextNode: epsilonNFA[currentNode].to){
        if (nextNode.second == '$'){
            waysTable.insert(make_pair(nextNode.first, symbol));
            if (!visited[nextNode.first]){
                DFS(nextNode.first, waysTable, visited, symbol);
            }
        }
    }
}

vector<set<pair<int, char>>> EpsilonNFA::EpsilonClosure(vector<bool>& finalStates){
    vector<set<pair<int, char>>> NFA(sz);

    for (size_t node = 0; node < sz; ++node){
        NFA[node].insert(make_pair(node, '$'));
        vector<bool> visited(sz);
        DFS(node, NFA[node], visited, '$');
        for (pair<int, char> i: NFA[node]){
            if (epsilonNFA[i.first].finalState){
                finalStates[node] = true;
            }
        }
    }

    vector<Node> waysTable(sz);
    for (size_t node = 0; node < sz; ++node){
        for (pair<int, char> nextNode: NFA[node]){
            for (pair<int, char> ways: epsilonNFA[nextNode.first].to){
                if (ways.second != '$'){
                    waysTable[node].to.push_back(ways);
                }
            }
        }
    }

    NFA.clear();
    NFA.resize(sz);
    for (size_t node = 0; node < sz; ++node){
        for (pair<int, char> nextNode: waysTable[node].to){
            NFA[node].insert(nextNode);
            vector<bool> visited(sz);
            DFS(nextNode.first, NFA[node], visited, nextNode.second);
        }
    }
    return NFA;
}

vector<set<pair<int, char>>> EpsilonNFA::Rename(map<pair<set<int>, char>, set<int>>& DFA, map<set<int>, bool>& finishedStatesOfDFA,
                                                vector<bool>& finishedStatesOfRenamedDFA, int& startStateOfRenamedDFA){
    map<set<int>, int> rename;
    int counter = 0;
    for (auto it = DFA.begin(); it != DFA.end(); ++it){
        if (rename.find(it->first.first) == rename.end()){
            rename[it->first.first] = counter;
            ++counter;
        }
        if (rename.find(it->second) == rename.end()){
            rename[it->second] = counter;
            ++counter;
        }
    }

    vector<set<pair<int, char>>> renamedDFA(counter);
    finishedStatesOfRenamedDFA.resize(counter);
    for (auto it = DFA.begin(); it != DFA.end(); ++it){
        renamedDFA[rename[it->first.first]].insert(make_pair(rename[it->second], it->first.second));
        if (finishedStatesOfDFA[it->first.first]){
            finishedStatesOfRenamedDFA[rename[it->first.first]] = true;
        }
        if (finishedStatesOfDFA[it->second]){
            finishedStatesOfRenamedDFA[rename[it->second]] = true;
        }
    }

    set<int> tmp;
    tmp.insert(startState);
    startStateOfRenamedDFA = rename[tmp];
    return renamedDFA;
}

DFA EpsilonNFA::BuildDFA(vector<set<pair<int, char>>>& reversed, vector<bool>& finalStates){
    map<pair<set<int>, char>, set<int>> tmpDFA;
    map<set<int>, bool> finishedStatesOfDFA;

    queue<set<int>> que;
    set<set<int>> visited;
    set<int> tmp;
    tmp.insert(startState);
    visited.insert(tmp);
    que.push(tmp);

    while (!que.empty()){
        set<int> nextVertex = que.front();
        que.pop();
        for (int i: nextVertex){
            if (finalStates[i]){
                finishedStatesOfDFA[nextVertex] = true;
                break;
            }
        }

        map<char, set<int>> ways;
        for (auto vert = nextVertex.begin(); vert != nextVertex.end(); ++vert){
            for (auto vertex: reversed[*vert]){
                ways[vertex.second].insert(vertex.first);
            }
        }

        for (auto edge = ways.begin(); edge != ways.end(); ++edge){
            tmpDFA[make_pair(nextVertex, edge->first)] = edge->second;
        }

        for (auto edge = ways.begin(); edge != ways.end(); ++edge){
            if (visited.find(edge->second) == visited.end()){
                visited.insert(edge->second);
                que.push(edge->second);
            }
        }
    }

    vector<bool> finishedStatesOfRenamedDFA;
    int startStateOfRenamedDFA;
    vector<set<pair<int, char>>> dfaGraph = Rename(tmpDFA, finishedStatesOfDFA, finishedStatesOfRenamedDFA, startStateOfRenamedDFA);
    DFA dfa(dfaGraph, startStateOfRenamedDFA, finishedStatesOfRenamedDFA);
    return dfa;
}

DFA EpsilonNFA::ConverToDFA(){
    vector<bool> finalStates(sz);
    vector<set<pair<int, char>>> NFA = EpsilonClosure(finalStates);

    return BuildDFA(NFA, finalStates);;
}

size_t LenthOfTheLCS(const string& regExpr, const string& str){
    EpsilonNFA epsilonNFA(regExpr);
    DFA dfa = epsilonNFA.ConverToDFA();

    return dfa.SubstringSearching(str);
}

/*
int main()
{
    string regExpr;
    cin >> regExpr;
    string str;
    cin >> str;
    cout << LenthOfTheLCS(regExpr, str);
    return 0;
}*/
