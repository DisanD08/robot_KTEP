#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

using namespace std;

//----------------------------------------------------------------------------------------------------------------------

int player_coordX = 1;
int player_coordY = 1;

char playerSymbol[4]{'>', 'V', '<', '^'};
string facing_[4]{"EAST", "SOUTH", "WEST", "NORTH"};
int playerSymbol_index = 0;

string mapName = "map1";
void setMap(string fileName) { mapName = fileName; }

int maxWidth = 0;


//----------------------------------------------------------------------------------------------------------------------

// Функция для чтения карты из файла и сохранения каждого символа в двумерный массив
vector<vector<char>> readMapFromFile() {
    vector<vector<char>> map;
    ifstream file("maps/" + mapName + ".map");
    string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            vector<char> row(line.begin(), line.end()); // Преобразуем строку в массив символов
            map.push_back(row); // Добавляем строку в карту
        }
        file.close();
    }
    else { cerr << "Error: can`t open file  | " << mapName << " |" << endl; }

    return map;
}


// Функция для размещения игрока на карте
void placePlayerOnMap(vector<vector<char>>& map) {
    int height = map.size(); // Количество строк (высота карты)

    // Проверяем, что карта не пуста и содержит хотя бы одну строку
    if (height == 0 || map[0].empty()) {
        cerr << "Error: map is empty." << endl;
        return;
    }

    int width = map[0].size(); // Количество символов в строке (ширина карты)

    // Рассчитываем строку и столбец, где должен появиться игрок
    int targetRow = (height - 3) - 2 * (player_coordY - 1);  // Строка для игрока (по оси Y)
    int targetCol = 2 + 4 * (player_coordX - 1);  // Столбец для игрока (по оси X)

    // Проверяем, что позиция для игрока корректна и что там находится точка
    if (targetRow >= 0 && targetRow < height && targetCol >= 0 && targetCol < width) {
        if (map[targetRow][targetCol] == '.') {
            map[targetRow][targetCol] = playerSymbol[playerSymbol_index];  // Заменяем точку на символ игрока
        }
        else { cerr << "Error: place for player already occupied by another symbol." << endl; }
    }
    else { cerr << "Error: failed to place player on map. Incorrect coordinates." << endl; }
}


// Функция для отображения карты
void displayMap(const vector<vector<char>>& map) {
    int height = map.size(); // Количество строк (высота карты)
    int width = map[0].size(); // Количество символов в строке (ширина карты)

    cout << endl;
    cout << "  COORD.      FACING" << endl;
    cout << "  (" << player_coordX << ", " << player_coordY << ")       " << facing_[playerSymbol_index] << endl;

    // Нумерация строк ось Y
    int rowNum = (height - 1) / 2;

    for (int i = 0; i < height; ++i) {
        if (i % 2 == 1 && i != height - 1) { // Для строк с точками
            cout << rowNum-- << " "; // Вывод номера строки
        }
        else {
            cout << "  "; // Пустые строки
        }

        // Выводим строку карты
        for (int j = 0; j < width; ++j) {
            cout << map[i][j];
        }
        cout << endl;
    }

    // Отображение координат по оси X
    cout << "    ";
    for (int i = 1; i <= (width - 3) / 4; ++i) {
        cout << i << "   ";
        maxWidth = i;
    }
    cout << endl;
}



void rendering() {
    //Очистка терминала
    system("cls");

    // Чтение карты из файла
    vector<vector<char>> map = readMapFromFile();

    // Размещение игрока на карте
    placePlayerOnMap(map);

    // Отображение карты
    displayMap(map);

    //Задержка в 0,25с
    this_thread::sleep_for(chrono::nanoseconds(250000000));
}


void turn_right() { playerSymbol_index = (playerSymbol_index + 1) % 4; rendering(); }
void turn_left() { playerSymbol_index = (playerSymbol_index - 1 + 4) % 4; rendering(); }


//Проверка нет ли преград перед игроком
int front_check() {
    //Запись карты с файла в массив
    vector<vector<char>> map = readMapFromFile();

    int height = map.size(); // Количество строк (высота карты)
    int width = map[0].size(); // Количество символов в строке (ширина карты)

    int targetRow = (height - 3) - 2 * (player_coordY - 1);  // Строка для игрока (по оси Y)
    int targetCol = 2 + 4 * (player_coordX - 1);  // Столбец для игрока (по оси X)

    //Проверка есть ли впереди игрока какое-то препятствие
    if (playerSymbol[playerSymbol_index] == '>') { 
        if (player_coordX == maxWidth) { return 1; } //Если игрок находится справа в самом конце 
        for (int i = 1; i < 4; i++) { if (map[targetRow][targetCol + i] != ' ') { return 1; } } 
    }
    if (playerSymbol[playerSymbol_index] == '<') { for (int i = 1; i < 4; i++) { if (map[targetRow][targetCol - i] != ' ') { return 1; } } }

    if (playerSymbol[playerSymbol_index] == '^') { for (int i = 1; i < 2; i++) { if (map[targetRow - i][targetCol] != ' ') { return 1; } } }
    if (playerSymbol[playerSymbol_index] == 'V') { 
        if (player_coordY == 1) { return 1; } //Если игрок находится в самом низу карты, то еще ниже нет куда 
        for (int i = 1; i < 2; i++) { if (map[targetRow + i][targetCol] != ' ') { return 1; } } 
    }

    return 0;
}


//Делает шаг в ту сторону, куда смотрит игрок
void step() {
    if (front_check() == 0) {
        if (playerSymbol[playerSymbol_index] == '>') { player_coordX++; }
        if (playerSymbol[playerSymbol_index] == 'V') { player_coordY--; }
        if (playerSymbol[playerSymbol_index] == '<') { player_coordX--; }
        if (playerSymbol[playerSymbol_index] == '^') { player_coordY++; }
    }
    rendering();
}


//----------------------------------------------------------------------------------------------------------------------

/*int main() {
    setMap("map3");
    rendering();

    turn_left();
    step(); step();
    turn_right();
    step(); step();
    step(); step();
    turn_left(); step(); turn_right();
    step(); step();
    step(); step();
    step(); step();


    return 0;
}*/
