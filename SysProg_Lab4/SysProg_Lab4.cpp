#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <sstream>

struct Transition {
    int from;
    char symbol;
    int to;
};

class Automaton {
public:
    int alphabetSize;
    int stateCount;
    int startState;
    std::set<int> finalStates;
    std::vector<Transition> transitions;

    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Error opening file.\n";
            return false;
        }

        file >> alphabetSize >> stateCount >> startState;

      
        file.ignore(); 

       
        std::string finalStatesLine;
        std::getline(file, finalStatesLine);
        std::istringstream finalStatesStream(finalStatesLine);
        int state;
        while (finalStatesStream >> state) {
            finalStates.insert(state);
        }

        int from, to;
        char symbol;
        while (file >> from >> symbol >> to) {
            transitions.push_back({ from, symbol, to });
        }

        return true;
    }

    
    std::set<int> findReachableStates() {
        std::set<int> reachable;
        std::queue<int> queue;
        queue.push(startState);
        reachable.insert(startState);

        while (!queue.empty()) {
            int current = queue.front();
            queue.pop();

            for (const auto& transition : transitions) {
                if (transition.from == current && reachable.find(transition.to) == reachable.end()) {
                    reachable.insert(transition.to);
                    queue.push(transition.to);
                }
            }
        }

        return reachable;
    }

    std::set<int> findDeadEndStates() {
        std::set<int> deadEnds;

       
        std::set<int> reachableFromStart = findReachableStates();

        std::set<int> reachableFromFinal;
        std::map<int, std::set<int>> reverseTransitions;
        for (const auto& transition : transitions) {
            reverseTransitions[transition.to].insert(transition.from);
        }

        std::queue<int> queue;
        for (int state : finalStates) {
            queue.push(state);
            reachableFromFinal.insert(state);
        }

        while (!queue.empty()) {
            int current = queue.front();
            queue.pop();

            for (int fromState : reverseTransitions[current]) {
                if (reachableFromFinal.find(fromState) == reachableFromFinal.end()) {
                    reachableFromFinal.insert(fromState);
                    queue.push(fromState);
                }
            }
        }

        for (int i = 0; i < stateCount; ++i) {
            if (reachableFromStart.find(i) != reachableFromStart.end() &&
                reachableFromFinal.find(i) == reachableFromFinal.end()) {
                deadEnds.insert(i);
            }
        }

        return deadEnds;
    }



    std::set<int> findUnreachableStates() {
       
        std::set<int> reachable = findReachableStates();
        std::set<int> unreachable;
       
        for (int i = 0; i < stateCount; ++i) {
            if (reachable.find(i) == reachable.end()) {
                unreachable.insert(i);
            }
        }

        return unreachable;
    }
    
    bool checkWord(const std::string& word) {
        int currentState = startState;
        for (char symbol : word) {
            bool foundTransition = false;
            for (const auto& transition : transitions) {
                if (transition.from == currentState && transition.symbol == symbol) {
                    currentState = transition.to;
                    foundTransition = true;
                    break;
                }
            }
            if (!foundTransition) return false; 
        }
        return finalStates.find(currentState) != finalStates.end();
    }
};

int main() {
    Automaton automaton;
    std::string filename;
    std::cout << "Enter the filename with the automaton: ";
    std::cin >> filename;

    if (!automaton.loadFromFile(filename)) {
        std::cerr << "Failed to load automaton.\n";
        return 1;
    }

 
    std::set<int> reachableStates = automaton.findReachableStates();
    std::set<int> unreachableStates = automaton.findUnreachableStates();
    std::set<int> deadEndStates = automaton.findDeadEndStates();

    std::cout << "Reachable states: ";
    for (int state : reachableStates) std::cout << state << " ";
    std::cout << "\nUnreachable states: ";
    for (int state : unreachableStates) std::cout << state << " ";
    std::cout << "\nDead-end states: ";
    for (int state : deadEndStates) std::cout << state << " ";
    std::cout << "\n";

   
    std::string word;
    while (true) {
        std::cout << "Enter a word to check (or 'exit' to quit): ";
        std::cin >> word;
        if (word == "exit") break;

        if (automaton.checkWord(word)) {
            std::cout << "The word is accepted by the automaton.\n";
        }
        else {
            std::cout << "The word is not accepted by the automaton.\n";
        }
    }

    return 0;
}
