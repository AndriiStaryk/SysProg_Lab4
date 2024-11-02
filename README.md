# ЛР 4. Аналізатор скінченних автоматів на С++

Дана програма парсить скінченний автомат із файлу, аналізує його стани та перевіряє, чи приймається задане слово автоматом. Програма може визначати досяжні, недосяжні та тупикові стани у автоматі.


## Структура Програми

### Основна логіка реалізована у класі `Automaton`, який містить методи для завантаження, аналізу станів та перевірки слів:

- `Transition`: Допоміжна структура:
```cpp
struct Transition {
    int from;
    char symbol;
    int to;
};
```

- `loadFromFile`: Розпарсює автомат та стани з файлу;
```cpp
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

```
  
- `findReachableStates`: Знаходить усі стани, які можна досягти зі стартового стану;

```cpp
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
```
  
- `findDeadEndStates`: Визначає стани, з яких неможливо потрапити в кінцевий стан;

```cpp
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

```
  
- `findUnreachableStates`: Визначає стани, яких не можна досягти зі стартового стану;

```cpp
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
```
  
- `checkWord`: Перевіряє, чи приймається задане слово автоматом;

```cpp
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
```
### Перевірка слова скінченним автоматом

```cpp
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
```
## Демонстрація

```cpp
//Приклад скінченного автомату
3
6
1
1 5
1 a 2
2 a 5
1 b 3
1 c 4
4 c 5
5 b 5 
5 c 5
5 a 1
```

### Результат

![image](https://github.com/user-attachments/assets/ad575fc5-ecdd-449d-8da8-99de4e910593)

