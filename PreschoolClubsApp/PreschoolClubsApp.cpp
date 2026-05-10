// PreschoolClubsApp.cpp : Консольное приложение учета кружков дошкольного развития                                                                                                                 
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <sstream>
#include <locale>
#include <cctype>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif


//КОНСТАНТЫ

// Имена файлов
const std::string FILE_USERS = "users.txt";
const std::string FILE_CLUBS = "clubs.txt";

// Роли пользователей
const std::string ROLE_ADMIN = "admin";
const std::string ROLE_USER = "user";

// Сообщения для пользователя
const std::string MSG_WELCOME = "Добро пожаловать в систему учета кружков дошкольного развития!";
const std::string MSG_LOGIN = "Введите логин: ";
const std::string MSG_PASSWORD = "Введите пароль: ";
const std::string MSG_LOGIN_SUCCESS = "Авторизация успешна!";
const std::string MSG_LOGIN_FAILED = "Неверный логин или пароль!";
const std::string MSG_EXIT = "Выход из программы...";
const std::string MSG_INVALID_INPUT = "Ошибка: неверный ввод данных!";
const std::string MSG_FILE_NOT_FOUND = "Ошибка: файл не найден!";
const std::string MSG_OPERATION_SUCCESS = "Операция выполнена успешно!";
const std::string MSG_OPERATION_CANCELLED = "Операция отменена.";
const std::string MSG_CONFIRM_DELETE = "Вы действительно хотите удалить запись? (y/n): ";
const std::string MSG_NO_DATA = "Данные не найдены.";
const std::string MSG_INVALID_NUMBER = "Ошибка: введено неверное число!";
const std::string MSG_INVALID_PRICE = "Ошибка: стоимость не может быть отрицательной!";
const std::string MSG_INVALID_STUDENTS = "Ошибка: количество учеников не может быть отрицательным!";
const std::string MSG_INDEX_OUT_OF_RANGE = "Ошибка: индекс выходит за пределы массива!";

//СТРУКТУРЫ ДАННЫХ

// Структура для учетной записи пользователя
struct User {
    std::string login;
    std::string password;
    std::string role; // "admin" или "user"
};

// Структура для данных о кружке
struct Club {
    std::string type;           // Вид кружка
    std::string name;            // Название
    std::string director_name;   // ФИО руководителя
    std::string schedule;        // График кружка
    std::string time;            // Время занятия
    double price;                // Стоимость обучения
    int number_of_students;      // Количество учеников
};

//ПРОТОТИПЫ ФУНКЦИЙ

// Функции работы с файлами
bool loadUsersFromFile(std::vector<User>& users);
bool saveUsersToFile(const std::vector<User>& users);
bool loadClubsFromFile(std::vector<Club>& clubs);
bool saveClubsToFile(const std::vector<Club>& clubs);

// Функции авторизации
bool authenticateUser(const std::vector<User>& users, std::string& current_role);
bool isLoginUnique(const std::vector<User>& users, const std::string& login);

// Функции работы с пользователями (администратор)
void showAllUsers(const std::vector<User>& users);
void addUser(std::vector<User>& users);
void editUser(std::vector<User>& users);
void deleteUser(std::vector<User>& users);
void manageUserAccounts(std::vector<User>& users);

// Функции работы с кружками (администратор - редактирование)
void showAllClubs(const std::vector<Club>& clubs);
void addClub(std::vector<Club>& clubs);
void deleteClub(std::vector<Club>& clubs);
void editClub(std::vector<Club>& clubs);
void editClubData(std::vector<Club>& clubs);

// Функции обработки данных (администратор)
void deleteClubsByStudentCount(std::vector<Club>& clubs);
void changeClubSchedule(std::vector<Club>& clubs);
void searchClubs(const std::vector<Club>& clubs);
void sortClubs(std::vector<Club>& clubs);
void processClubData(std::vector<Club>& clubs);

// Функции работы с данными (пользователь)
void showClubsByType(const std::vector<Club>& clubs);
void showClubsByDirector(const std::vector<Club>& clubs);
void searchClubsUser(const std::vector<Club>& clubs);
void sortClubsUser(const std::vector<Club>& clubs);
void userDataModule(const std::vector<Club>& clubs);

// Функции валидации и ввода
bool isNumberNumeric(const std::string& str);
bool isValidPrice(double price);
bool isValidStudentCount(int count);
int getIntInput(const std::string& prompt);
double getDoubleInput(const std::string& prompt);
std::string getStringInput(const std::string& prompt);
bool confirmAction(const std::string& message);

// Функции меню
void adminMenu(std::vector<User>& users, std::vector<Club>& clubs);
void userMenu(const std::vector<Club>& clubs);

//РЕАЛИЗАЦИЯ ФУНКЦИЙ

// Загрузка пользователей из файла
bool loadUsersFromFile(std::vector<User>& users) {
    std::ifstream file(FILE_USERS);
    if (!file.is_open()) {
        // Если файл не существует, создаем администратора по умолчанию
        User admin;
        admin.login = "admin";
        admin.password = "admin";
        admin.role = ROLE_ADMIN;
        users.push_back(admin);
        saveUsersToFile(users);
        return true;
    }

    users.clear();
    User user;
    while (file >> user.login >> user.password >> user.role) {
        users.push_back(user);
    }
    file.close();
    return true;
}

// Сохранение пользователей в файл
bool saveUsersToFile(const std::vector<User>& users) {
    std::ofstream file(FILE_USERS);
    if (!file.is_open()) {
        std::cout << MSG_FILE_NOT_FOUND << std::endl;
        return false;
    }

    for (const auto& user : users) {
        file << user.login << " " << user.password << " " << user.role << std::endl;
    }
    file.close();
    return true;
}

// Загрузка кружков из файла
bool loadClubsFromFile(std::vector<Club>& clubs) {
    std::ifstream file(FILE_CLUBS);
    if (!file.is_open()) {
        return true; // Файл может не существовать, это нормально
    }

    clubs.clear();
    Club club;
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        std::getline(iss, club.type, '|');
        std::getline(iss, club.name, '|');
        std::getline(iss, club.director_name, '|');
        std::getline(iss, club.schedule, '|');
        std::getline(iss, club.time, '|');
        
        std::string priceStr, countStr;
        std::getline(iss, priceStr, '|');
        std::getline(iss, countStr, '|');
        
        try {
            club.price = std::stod(priceStr);
            club.number_of_students = std::stoi(countStr);
            clubs.push_back(club);
        } catch (...) {
            continue; // Пропускаем некорректные строки
        }
    }
    file.close();
    return true;
}

// Сохранение кружков в файл
bool saveClubsToFile(const std::vector<Club>& clubs) {
    std::ofstream file(FILE_CLUBS);
    if (!file.is_open()) {
        std::cout << MSG_FILE_NOT_FOUND << std::endl;
        return false;
    }

    for (const auto& club : clubs) {
        file << club.type << "|" << club.name << "|" << club.director_name << "|"
             << club.schedule << "|" << club.time << "|" << club.price << "|"
             << club.number_of_students << std::endl;
    }
    file.close();
    return true;
}

// Проверка уникальности логина
bool isLoginUnique(const std::vector<User>& users, const std::string& login) {
    for (const auto& user : users) {
        if (user.login == login) {
            return false;
        }
    }
    return true;
}

// Авторизация пользователя
bool authenticateUser(const std::vector<User>& users, std::string& current_role) {
    std::string login, password;
    
    std::cout << MSG_LOGIN;
    std::cin >> login;
    std::cout << MSG_PASSWORD;
    std::cin >> password;

    for (const auto& user : users) {
        if (user.login == login && user.password == password) {
            current_role = user.role;
            std::cout << MSG_LOGIN_SUCCESS << std::endl;
            return true;
        }
    }

    std::cout << MSG_LOGIN_FAILED << std::endl;
    return false;
}

// Показать всех пользователей
void showAllUsers(const std::vector<User>& users) {
    if (users.empty()) {
        std::cout << MSG_NO_DATA << std::endl;
        return;
    }

    std::cout << "\n=== Список всех пользователей ===" << std::endl;
    std::cout << std::left << std::setw(20) << "Логин" 
              << std::setw(20) << "Пароль" 
              << std::setw(15) << "Роль" << std::endl;
    std::cout << std::string(55, '-') << std::endl;

    for (size_t i = 0; i < users.size(); ++i) {
        std::cout << std::left << std::setw(20) << users[i].login
                  << std::setw(20) << users[i].password
                  << std::setw(15) << users[i].role << std::endl;
    }
    std::cout << std::endl;
}

// Добавить пользователя
void addUser(std::vector<User>& users) {
    User newUser;
    
    std::cout << "\n=== Добавление нового пользователя ===" << std::endl;
    std::cout << "Введите логин: ";
    std::cin >> newUser.login;

    if (!isLoginUnique(users, newUser.login)) {
        std::cout << "Ошибка: пользователь с таким логином уже существует!" << std::endl;
        return;
    }

    std::cout << "Введите пароль: ";
    std::cin >> newUser.password;

    std::cout << "Введите роль (admin/user): ";
    std::cin >> newUser.role;

    if (newUser.role != ROLE_ADMIN && newUser.role != ROLE_USER) {
        std::cout << "Ошибка: неверная роль! Установлена роль 'user'." << std::endl;
        newUser.role = ROLE_USER;
    }

    users.push_back(newUser);
    saveUsersToFile(users);
    std::cout << MSG_OPERATION_SUCCESS << std::endl;
}

// Редактировать пользователя
void editUser(std::vector<User>& users) {
    if (users.empty()) {
        std::cout << MSG_NO_DATA << std::endl;
        return;
    }

    showAllUsers(users);
    int index = getIntInput("Введите номер пользователя для редактирования: ") - 1;

    if (index < 0 || index >= static_cast<int>(users.size())) {
        std::cout << MSG_INDEX_OUT_OF_RANGE << std::endl;
        return;
    }

    std::cout << "\n=== Редактирование пользователя ===" << std::endl;
    std::cout << "Введите новый логин (текущий: " << users[index].login << "): ";
    std::string newLogin;
    std::cin >> newLogin;

    if (newLogin != users[index].login && !isLoginUnique(users, newLogin)) {
        std::cout << "Ошибка: пользователь с таким логином уже существует!" << std::endl;
        return;
    }

    users[index].login = newLogin;

    std::cout << "Введите новый пароль: ";
    std::cin >> users[index].password;

    std::cout << "Введите новую роль (admin/user): ";
    std::cin >> users[index].role;

    if (users[index].role != ROLE_ADMIN && users[index].role != ROLE_USER) {
        std::cout << "Ошибка: неверная роль! Установлена роль 'user'." << std::endl;
        users[index].role = ROLE_USER;
    }

    saveUsersToFile(users);
    std::cout << MSG_OPERATION_SUCCESS << std::endl;
}

// Удалить пользователя
void deleteUser(std::vector<User>& users) {
    if (users.empty()) {
        std::cout << MSG_NO_DATA << std::endl;
        return;
    }

    showAllUsers(users);
    int index = getIntInput("Введите номер пользователя для удаления: ") - 1;

    if (index < 0 || index >= static_cast<int>(users.size())) {
        std::cout << MSG_INDEX_OUT_OF_RANGE << std::endl;
        return;
    }

    if (confirmAction(MSG_CONFIRM_DELETE)) {
        users.erase(users.begin() + index);
        saveUsersToFile(users);
        std::cout << MSG_OPERATION_SUCCESS << std::endl;
    } else {
        std::cout << MSG_OPERATION_CANCELLED << std::endl;
    }
}

// Управление учетными записями пользователей
void manageUserAccounts(std::vector<User>& users) {
    int choice;
    do {
        std::cout << "\n=== Управление учетными записями пользователей ===" << std::endl;
        std::cout << "1. Просмотр всех учетных записей" << std::endl;
        std::cout << "2. Добавление новой учетной записи" << std::endl;
        std::cout << "3. Редактирование учетной записи" << std::endl;
        std::cout << "4. Удаление учетной записи" << std::endl;
        std::cout << "0. Назад" << std::endl;
        std::cout << "Выберите действие: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << MSG_INVALID_INPUT << std::endl;
            continue;
        }

        switch (choice) {
        case 1:
            showAllUsers(users);
            break;
        case 2:
            addUser(users);
            break;
        case 3:
            editUser(users);
            break;
        case 4:
            deleteUser(users);
            break;
        case 0:
            return;
        default:
            std::cout << MSG_INVALID_INPUT << std::endl;
        }
    } while (choice != 0);
}

// Показать все кружки
void showAllClubs(const std::vector<Club>& clubs) {
    if (clubs.empty()) {
        std::cout << MSG_NO_DATA << std::endl;
        return;
    }

    const int w_no = 8, w_type = 16, w_name = 16, w_director = 20, w_schedule = 16, w_time = 14, w_price = 12, w_students = 12;

    std::cout << "\n=== Список всех кружков ===" << std::endl;
    std::cout << "|" << std::left << std::setw(w_no) << " Номер " << "|"
              << std::setw(w_type) << " Вид " << "|"
              << std::setw(w_name) << " Название " << "|"
              << std::setw(w_director) << " Руководитель " << "|"
              << std::setw(w_schedule) << " График " << "|"
              << std::setw(w_time) << " Время " << "|"
              << std::setw(w_price) << " Стоимость " << "|"
              << std::setw(w_students) << " Учеников " << "|" << std::endl;
    std::cout << "+" << std::string(w_no, '-') << "+" << std::string(w_type, '-') << "+"
              << std::string(w_name, '-') << "+" << std::string(w_director, '-') << "+"
              << std::string(w_schedule, '-') << "+" << std::string(w_time, '-') << "+"
              << std::string(w_price, '-') << "+" << std::string(w_students, '-') << "+" << std::endl;

    for (size_t i = 0; i < clubs.size(); ++i) {
        std::cout << "|" << std::left << std::setw(w_no) << (i + 1) << "|"
                  << std::setw(w_type) << clubs[i].type << "|"
                  << std::setw(w_name) << clubs[i].name << "|"
                  << std::setw(w_director) << clubs[i].director_name << "|"
                  << std::setw(w_schedule) << clubs[i].schedule << "|"
                  << std::setw(w_time) << clubs[i].time << "|"
                  << std::right << std::setw(w_price) << std::fixed << std::setprecision(2) << clubs[i].price << "|"
                  << std::setw(w_students) << clubs[i].number_of_students << "|" << std::endl;
    }
    std::cout << std::endl;
}

// Добавить кружок
void addClub(std::vector<Club>& clubs) {
    Club newClub;
    
    std::cout << "\n=== Добавление нового кружка ===" << std::endl;
    
    std::cin.ignore();
    newClub.type = getStringInput("Введите вид кружка: ");
    newClub.name = getStringInput("Введите название кружка: ");
    newClub.director_name = getStringInput("Введите ФИО руководителя: ");
    newClub.schedule = getStringInput("Введите график кружка: ");
    newClub.time = getStringInput("Введите время занятия: ");
    
    newClub.price = getDoubleInput("Введите стоимость обучения: ");
    if (!isValidPrice(newClub.price)) {
        std::cout << MSG_INVALID_PRICE << std::endl;
        return;
    }

    newClub.number_of_students = getIntInput("Введите количество учеников: ");
    if (!isValidStudentCount(newClub.number_of_students)) {
        std::cout << MSG_INVALID_STUDENTS << std::endl;
        return;
    }

    clubs.push_back(newClub);
    saveClubsToFile(clubs);
    std::cout << MSG_OPERATION_SUCCESS << std::endl;
}

// Удалить кружок
void deleteClub(std::vector<Club>& clubs) {
    if (clubs.empty()) {
        std::cout << MSG_NO_DATA << std::endl;
        return;
    }

    showAllClubs(clubs);
    int index = getIntInput("Введите номер кружка для удаления: ") - 1;

    if (index < 0 || index >= static_cast<int>(clubs.size())) {
        std::cout << MSG_INDEX_OUT_OF_RANGE << std::endl;
        return;
    }

    if (confirmAction(MSG_CONFIRM_DELETE)) {
        clubs.erase(clubs.begin() + index);
        saveClubsToFile(clubs);
        std::cout << MSG_OPERATION_SUCCESS << std::endl;
    } else {
        std::cout << MSG_OPERATION_CANCELLED << std::endl;
    }
}

// Редактировать кружок
void editClub(std::vector<Club>& clubs) {
    if (clubs.empty()) {
        std::cout << MSG_NO_DATA << std::endl;
        return;
    }

    showAllClubs(clubs);
    int index = getIntInput("Введите номер кружка для редактирования: ") - 1;

    if (index < 0 || index >= static_cast<int>(clubs.size())) {
        std::cout << MSG_INDEX_OUT_OF_RANGE << std::endl;
        return;
    }

    std::cout << "\n=== Редактирование кружка ===" << std::endl;
    
    std::cin.ignore();
    std::cout << "Вид кружка (текущий: " << clubs[index].type << "): ";
    std::string input = getStringInput("");
    if (!input.empty()) clubs[index].type = input;

    std::cout << "Название (текущее: " << clubs[index].name << "): ";
    input = getStringInput("");
    if (!input.empty()) clubs[index].name = input;

    std::cout << "ФИО руководителя (текущее: " << clubs[index].director_name << "): ";
    input = getStringInput("");
    if (!input.empty()) clubs[index].director_name = input;

    std::cout << "График кружка (текущий: " << clubs[index].schedule << "): ";
    input = getStringInput("");
    if (!input.empty()) clubs[index].schedule = input;

    std::cout << "Время занятия (текущее: " << clubs[index].time << "): ";
    input = getStringInput("");
    if (!input.empty()) clubs[index].time = input;

    std::cout << "Стоимость обучения (текущая: " << clubs[index].price << "): ";
    double newPrice = getDoubleInput("");
    if (newPrice >= 0) {
        clubs[index].price = newPrice;
    } else {
        std::cout << MSG_INVALID_PRICE << std::endl;
    }

    std::cout << "Количество учеников (текущее: " << clubs[index].number_of_students << "): ";
    int newCount = getIntInput("");
    if (newCount >= 0) {
        clubs[index].number_of_students = newCount;
    } else {
        std::cout << MSG_INVALID_STUDENTS << std::endl;
    }

    saveClubsToFile(clubs);
    std::cout << MSG_OPERATION_SUCCESS << std::endl;
}

// Режим редактирования данных (администратор)
void editClubData(std::vector<Club>& clubs) {
    int choice;
    do {
        std::cout << "\n=== Режим редактирования данных ===" << std::endl;
        std::cout << "1. Просмотр всех данных" << std::endl;
        std::cout << "2. Добавление новой записи" << std::endl;
        std::cout << "3. Удаление записи" << std::endl;
        std::cout << "4. Редактирование записи" << std::endl;
        std::cout << "0. Назад" << std::endl;
        std::cout << "Выберите действие: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << MSG_INVALID_INPUT << std::endl;
            continue;
        }

        switch (choice) {
        case 1:
            showAllClubs(clubs);
            break;
        case 2:
            addClub(clubs);
            break;
        case 3:
            deleteClub(clubs);
            break;
        case 4:
            editClub(clubs);
            break;
        case 0:
            return;
        default:
            std::cout << MSG_INVALID_INPUT << std::endl;
        }
    } while (choice != 0);
}

// Удалить кружки с количеством участников меньше указанного
void deleteClubsByStudentCount(std::vector<Club>& clubs) {
    if (clubs.empty()) {
        std::cout << MSG_NO_DATA << std::endl;
        return;
    }

    int minCount = getIntInput("Введите минимальное количество участников: ");
    
    if (minCount < 0) {
        std::cout << MSG_INVALID_STUDENTS << std::endl;
        return;
    }

    std::vector<Club> clubsToDelete;
    for (const auto& club : clubs) {
        if (club.number_of_students < minCount) {
            clubsToDelete.push_back(club);
        }
    }

    if (clubsToDelete.empty()) {
        std::cout << "Кружки с количеством участников меньше " << minCount << " не найдены." << std::endl;
        return;
    }

    std::cout << "\nНайдено кружков для удаления: " << clubsToDelete.size() << std::endl;
    for (const auto& club : clubsToDelete) {
        std::cout << "- " << club.name << " (участников: " << club.number_of_students << ")" << std::endl;
    }

    if (confirmAction(MSG_CONFIRM_DELETE)) {
        clubs.erase(
            std::remove_if(clubs.begin(), clubs.end(),
                [minCount](const Club& club) {
                    return club.number_of_students < minCount;
                }),
            clubs.end()
        );
        saveClubsToFile(clubs);
        std::cout << MSG_OPERATION_SUCCESS << std::endl;
    } else {
        std::cout << MSG_OPERATION_CANCELLED << std::endl;
    }
}

// Изменить график проведения для выбранного кружка
void changeClubSchedule(std::vector<Club>& clubs) {
    if (clubs.empty()) {
        std::cout << MSG_NO_DATA << std::endl;
        return;
    }

    showAllClubs(clubs);
    int index = getIntInput("Введите номер кружка для изменения графика: ") - 1;

    if (index < 0 || index >= static_cast<int>(clubs.size())) {
        std::cout << MSG_INDEX_OUT_OF_RANGE << std::endl;
        return;
    }

    std::cout << "Текущий график: " << clubs[index].schedule << std::endl;
    std::cin.ignore();
    std::string newSchedule = getStringInput("Введите новый график: ");

    if (!newSchedule.empty()) {
        clubs[index].schedule = newSchedule;
        saveClubsToFile(clubs);
        std::cout << MSG_OPERATION_SUCCESS << std::endl;
    }
}

// Поиск кружков (администратор)
void searchClubs(const std::vector<Club>& clubs) {
    if (clubs.empty()) {
        std::cout << MSG_NO_DATA << std::endl;
        return;
    }

    std::cout << "\n=== Поиск кружков ===" << std::endl;
    std::cout << "1. По виду кружка" << std::endl;
    std::cout << "2. По названию" << std::endl;
    std::cout << "3. По руководителю" << std::endl;
    std::cout << "4. По стоимости (диапазон)" << std::endl;
    std::cout << "Выберите критерий поиска: ";

    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << MSG_INVALID_INPUT << std::endl;
        return;
    }

    std::vector<Club> results;
    std::cin.ignore();

    switch (choice) {
    case 1: {
        std::string type = getStringInput("Введите вид кружка: ");
        for (const auto& club : clubs) {
            if (club.type.find(type) != std::string::npos) {
                results.push_back(club);
            }
        }
        break;
    }
    case 2: {
        std::string name = getStringInput("Введите название: ");
        for (const auto& club : clubs) {
            if (club.name.find(name) != std::string::npos) {
                results.push_back(club);
            }
        }
        break;
    }
    case 3: {
        std::string director = getStringInput("Введите ФИО руководителя: ");
        for (const auto& club : clubs) {
            if (club.director_name.find(director) != std::string::npos) {
                results.push_back(club);
            }
        }
        break;
    }
    case 4: {
        double minPrice = getDoubleInput("Введите минимальную стоимость: ");
        double maxPrice = getDoubleInput("Введите максимальную стоимость: ");
        for (const auto& club : clubs) {
            if (club.price >= minPrice && club.price <= maxPrice) {
                results.push_back(club);
            }
        }
        break;
    }
    default:
        std::cout << MSG_INVALID_INPUT << std::endl;
        return;
    }

    if (results.empty()) {
        std::cout << MSG_NO_DATA << std::endl;
    } else {
        std::cout << "\n=== Результаты поиска ===" << std::endl;
        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << "\nКружок " << (i + 1) << ":" << std::endl;
            std::cout << "  Вид: " << results[i].type << std::endl;
            std::cout << "  Название: " << results[i].name << std::endl;
            std::cout << "  Руководитель: " << results[i].director_name << std::endl;
            std::cout << "  График: " << results[i].schedule << std::endl;
            std::cout << "  Время: " << results[i].time << std::endl;
            std::cout << "  Стоимость: " << results[i].price << std::endl;
            std::cout << "  Учеников: " << results[i].number_of_students << std::endl;
        }
    }
}

// Сортировка кружков (администратор)
void sortClubs(std::vector<Club>& clubs) {
    if (clubs.empty()) {
        std::cout << MSG_NO_DATA << std::endl;
        return;
    }

    std::cout << "\n=== Сортировка кружков ===" << std::endl;
    std::cout << "1. По названию" << std::endl;
    std::cout << "2. По стоимости" << std::endl;
    std::cout << "3. По количеству учеников" << std::endl;
    std::cout << "4. По виду кружка" << std::endl;
    std::cout << "Выберите критерий сортировки: ";

    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << MSG_INVALID_INPUT << std::endl;
        return;
    }

    switch (choice) {
    case 1:
        std::sort(clubs.begin(), clubs.end(),
            [](const Club& a, const Club& b) { return a.name < b.name; });
        break;
    case 2:
        std::sort(clubs.begin(), clubs.end(),
            [](const Club& a, const Club& b) { return a.price < b.price; });
        break;
    case 3:
        std::sort(clubs.begin(), clubs.end(),
            [](const Club& a, const Club& b) { return a.number_of_students < b.number_of_students; });
        break;
    case 4:
        std::sort(clubs.begin(), clubs.end(),
            [](const Club& a, const Club& b) { return a.type < b.type; });
        break;
    default:
        std::cout << MSG_INVALID_INPUT << std::endl;
        return;
    }

    saveClubsToFile(clubs);
    std::cout << MSG_OPERATION_SUCCESS << std::endl;
    showAllClubs(clubs);
}

// Режим обработки данных (администратор)
void processClubData(std::vector<Club>& clubs) {
    int choice;
    do {
        std::cout << "\n=== Режим обработки данных ===" << std::endl;
        std::cout << "1. Выполнение индивидуального задания" << std::endl;
        std::cout << "2. Поиск данных" << std::endl;
        std::cout << "3. Сортировка" << std::endl;
        std::cout << "0. Назад" << std::endl;
        std::cout << "Выберите действие: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << MSG_INVALID_INPUT << std::endl;
            continue;
        }

        switch (choice) {
        case 1: {
            std::cout << "\n=== Индивидуальное задание ===" << std::endl;
            std::cout << "1. Удалить кружки с количеством участников меньше указанного" << std::endl;
            std::cout << "2. Изменить график проведения для выбранного кружка" << std::endl;
            std::cout << "Выберите задание: ";
            int taskChoice;
            if (!(std::cin >> taskChoice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << MSG_INVALID_INPUT << std::endl;
                break;
            }
            if (taskChoice == 1) {
                deleteClubsByStudentCount(clubs);
            } else if (taskChoice == 2) {
                changeClubSchedule(clubs);
            } else {
                std::cout << MSG_INVALID_INPUT << std::endl;
            }
            break;
        }
        case 2:
            searchClubs(clubs);
            break;
        case 3:
            sortClubs(clubs);
            break;
        case 0:
            return;
        default:
            std::cout << MSG_INVALID_INPUT << std::endl;
        }
    } while (choice != 0);
}

// Вывести все сведения о кружках определенного вида (пользователь)
void showClubsByType(const std::vector<Club>& clubs) {
    if (clubs.empty()) {
        std::cout << MSG_NO_DATA << std::endl;
        return;
    }

    std::cin.ignore();
    std::string type = getStringInput("Введите вид кружка: ");

    std::vector<Club> results;
    for (const auto& club : clubs) {
        if (club.type == type) {
            results.push_back(club);
        }
    }

    if (results.empty()) {
        std::cout << MSG_NO_DATA << std::endl;
    } else {
        std::cout << "\n=== Кружки вида '" << type << "' ===" << std::endl;
        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << "\nКружок " << (i + 1) << ":" << std::endl;
            std::cout << "  Вид: " << results[i].type << std::endl;
            std::cout << "  Название: " << results[i].name << std::endl;
            std::cout << "  Руководитель: " << results[i].director_name << std::endl;
            std::cout << "  График: " << results[i].schedule << std::endl;
            std::cout << "  Время: " << results[i].time << std::endl;
            std::cout << "  Стоимость: " << results[i].price << std::endl;
            std::cout << "  Учеников: " << results[i].number_of_students << std::endl;
        }
    }
}

// Вывести все сведения о кружках выбранного руководителя (пользователь)
void showClubsByDirector(const std::vector<Club>& clubs) {
    if (clubs.empty()) {
        std::cout << MSG_NO_DATA << std::endl;
        return;
    }

    std::cin.ignore();
    std::string director = getStringInput("Введите ФИО руководителя: ");

    std::vector<Club> results;
    for (const auto& club : clubs) {
        if (club.director_name == director) {
            results.push_back(club);
        }
    }

    if (results.empty()) {
        std::cout << MSG_NO_DATA << std::endl;
    } else {
        std::cout << "\n=== Кружки руководителя '" << director << "' ===" << std::endl;
        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << "\nКружок " << (i + 1) << ":" << std::endl;
            std::cout << "  Вид: " << results[i].type << std::endl;
            std::cout << "  Название: " << results[i].name << std::endl;
            std::cout << "  Руководитель: " << results[i].director_name << std::endl;
            std::cout << "  График: " << results[i].schedule << std::endl;
            std::cout << "  Время: " << results[i].time << std::endl;
            std::cout << "  Стоимость: " << results[i].price << std::endl;
            std::cout << "  Учеников: " << results[i].number_of_students << std::endl;
        }
    }
}

// Поиск кружков (пользователь)
void searchClubsUser(const std::vector<Club>& clubs) {
    if (clubs.empty()) {
        std::cout << MSG_NO_DATA << std::endl;
        return;
    }

    std::cout << "\n=== Поиск кружков ===" << std::endl;
    std::cout << "1. По названию" << std::endl;
    std::cout << "2. По стоимости (диапазон)" << std::endl;
    std::cout << "Выберите критерий поиска: ";

    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << MSG_INVALID_INPUT << std::endl;
        return;
    }

    std::vector<Club> results;
    std::cin.ignore();

    switch (choice) {
    case 1: {
        std::string name = getStringInput("Введите название: ");
        for (const auto& club : clubs) {
            if (club.name.find(name) != std::string::npos) {
                results.push_back(club);
            }
        }
        break;
    }
    case 2: {
        double minPrice = getDoubleInput("Введите минимальную стоимость: ");
        double maxPrice = getDoubleInput("Введите максимальную стоимость: ");
        for (const auto& club : clubs) {
            if (club.price >= minPrice && club.price <= maxPrice) {
                results.push_back(club);
            }
        }
        break;
    }
    default:
        std::cout << MSG_INVALID_INPUT << std::endl;
        return;
    }

    if (results.empty()) {
        std::cout << MSG_NO_DATA << std::endl;
    } else {
        std::cout << "\n=== Результаты поиска ===" << std::endl;
        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << "\nКружок " << (i + 1) << ":" << std::endl;
            std::cout << "  Вид: " << results[i].type << std::endl;
            std::cout << "  Название: " << results[i].name << std::endl;
            std::cout << "  Руководитель: " << results[i].director_name << std::endl;
            std::cout << "  График: " << results[i].schedule << std::endl;
            std::cout << "  Время: " << results[i].time << std::endl;
            std::cout << "  Стоимость: " << results[i].price << std::endl;
            std::cout << "  Учеников: " << results[i].number_of_students << std::endl;
        }
    }
}

// Сортировка кружков (пользователь)
void sortClubsUser(const std::vector<Club>& clubs) {
    if (clubs.empty()) {
        std::cout << MSG_NO_DATA << std::endl;
        return;
    }

    std::cout << "\n=== Сортировка кружков ===" << std::endl;
    std::cout << "1. По названию" << std::endl;
    std::cout << "2. По стоимости" << std::endl;
    std::cout << "3. По количеству учеников" << std::endl;
    std::cout << "Выберите критерий сортировки: ";

    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << MSG_INVALID_INPUT << std::endl;
        return;
    }

    std::vector<Club> sortedClubs = clubs; // Создаем копию для сортировки

    switch (choice) {
    case 1:
        std::sort(sortedClubs.begin(), sortedClubs.end(),
            [](const Club& a, const Club& b) { return a.name < b.name; });
        break;
    case 2:
        std::sort(sortedClubs.begin(), sortedClubs.end(),
            [](const Club& a, const Club& b) { return a.price < b.price; });
        break;
    case 3:
        std::sort(sortedClubs.begin(), sortedClubs.end(),
            [](const Club& a, const Club& b) { return a.number_of_students < b.number_of_students; });
        break;
    default:
        std::cout << MSG_INVALID_INPUT << std::endl;
        return;
    }

    std::cout << "\n=== Отсортированные кружки ===" << std::endl;
    for (size_t i = 0; i < sortedClubs.size(); ++i) {
        std::cout << "\nКружок " << (i + 1) << ":" << std::endl;
        std::cout << "  Вид: " << sortedClubs[i].type << std::endl;
        std::cout << "  Название: " << sortedClubs[i].name << std::endl;
        std::cout << "  Руководитель: " << sortedClubs[i].director_name << std::endl;
        std::cout << "  График: " << sortedClubs[i].schedule << std::endl;
        std::cout << "  Время: " << sortedClubs[i].time << std::endl;
        std::cout << "  Стоимость: " << sortedClubs[i].price << std::endl;
        std::cout << "  Учеников: " << sortedClubs[i].number_of_students << std::endl;
    }
}

// Модуль работы с данными (пользователь)
void userDataModule(const std::vector<Club>& clubs) {
    int choice;
    do {
        std::cout << "\n=== Работа с данными ===" << std::endl;
        std::cout << "1. Просмотр всех данных" << std::endl;
        std::cout << "2. Выполнение индивидуального задания" << std::endl;
        std::cout << "3. Поиск данных" << std::endl;
        std::cout << "4. Сортировка" << std::endl;
        std::cout << "0. Назад" << std::endl;
        std::cout << "Выберите действие: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << MSG_INVALID_INPUT << std::endl;
            continue;
        }

        switch (choice) {
        case 1:
            showAllClubs(clubs);
            break;
        case 2: {
            std::cout << "\n=== Индивидуальное задание ===" << std::endl;
            std::cout << "1. Вывести все сведения о кружках определенного вида" << std::endl;
            std::cout << "2. Вывести все сведения о кружках выбранного руководителя" << std::endl;
            std::cout << "Выберите задание: ";
            int taskChoice;
            if (!(std::cin >> taskChoice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << MSG_INVALID_INPUT << std::endl;
                break;
            }
            if (taskChoice == 1) {
                showClubsByType(clubs);
            } else if (taskChoice == 2) {
                showClubsByDirector(clubs);
            } else {
                std::cout << MSG_INVALID_INPUT << std::endl;
            }
            break;
        }
        case 3:
            searchClubsUser(clubs);
            break;
        case 4:
            sortClubsUser(clubs);
            break;
        case 0:
            return;
        default:
            std::cout << MSG_INVALID_INPUT << std::endl;
        }
    } while (choice != 0);
}

// Валидация и функции ввода
bool isNumberNumeric(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isdigit(c) && c != '.' && c != '-') {
            return false;
        }
    }
    return true;
}

bool isValidPrice(double price) {
    return price >= 0;
}

bool isValidStudentCount(int count) {
    return count >= 0;
}

int getIntInput(const std::string& prompt) {
    std::string input;
    int value;
    
    while (true) {
        std::cout << prompt;
        std::cin >> input;
        
        try {
            value = std::stoi(input);
            return value;
        } catch (...) {
            std::cout << MSG_INVALID_NUMBER << std::endl;
            std::cin.clear();
        }
    }
}

double getDoubleInput(const std::string& prompt) {
    std::string input;
    double value;
    
    while (true) {
        std::cout << prompt;
        std::cin >> input;
        
        try {
            value = std::stod(input);
            return value;
        } catch (...) {
            std::cout << MSG_INVALID_NUMBER << std::endl;
            std::cin.clear();
        }
    }
}

std::string getStringInput(const std::string& prompt) {
    std::string input;
    if (!prompt.empty()) {
        std::cout << prompt;
    }
    std::getline(std::cin, input);
    return input;
}

bool confirmAction(const std::string& message) {
    char answer;
    std::cout << message;
    std::cin >> answer;
    return (answer == 'y' || answer == 'Y');
}

// Меню администратора
void adminMenu(std::vector<User>& users, std::vector<Club>& clubs) {
    int choice;
    do {
        std::cout << "\n=== Меню администратора ===" << std::endl;
        std::cout << "1. Управление учетными записями пользователей" << std::endl;
        std::cout << "2. Работа с данными (режим редактирования)" << std::endl;
        std::cout << "3. Работа с данными (режим обработки)" << std::endl;
        std::cout << "0. Выход" << std::endl;
        std::cout << "Выберите действие: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << MSG_INVALID_INPUT << std::endl;
            continue;
        }

        switch (choice) {
        case 1:
            manageUserAccounts(users);
            break;
        case 2:
            editClubData(clubs);
            break;
        case 3:
            processClubData(clubs);
            break;
        case 0:
            return;
        default:
            std::cout << MSG_INVALID_INPUT << std::endl;
        }
    } while (choice != 0);
}

// Меню пользователя
void userMenu(const std::vector<Club>& clubs) {
    int choice;
    do {
        std::cout << "\n=== Меню пользователя ===" << std::endl;
        std::cout << "1. Работа с данными" << std::endl;
        std::cout << "0. Выход" << std::endl;
        std::cout << "Выберите действие: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << MSG_INVALID_INPUT << std::endl;
            continue;
        }

        switch (choice) {
        case 1:
            userDataModule(clubs);
            break;
        case 0:
            return;
        default:
            std::cout << MSG_INVALID_INPUT << std::endl;
        }
    } while (choice != 0);
}

//ГЛАВНАЯ ФУНКЦИЯ

int main() {
#ifdef _WIN32
    
    SetConsoleOutputCP(CP_UTF8);
     SetConsoleCP(CP_UTF8);
    
#endif
        setlocale(LC_ALL, "ru_RU.UTF-8");

    std::cout << MSG_WELCOME << std::endl;

    // Загрузка данных из файлов
    std::vector<User> users;
    std::vector<Club> clubs;

    if (!loadUsersFromFile(users)) {
        std::cout << "Ошибка загрузки пользователей!" << std::endl;
        return 1;
    }

    if (!loadClubsFromFile(clubs)) {
        std::cout << "Ошибка загрузки кружков!" << std::endl;
        return 1;
    }

    // Авторизация
    std::string current_role;
    if (!authenticateUser(users, current_role)) {
        std::cout << "Не удалось войти в систему." << std::endl;
        return 1;
    }

    // Переход в соответствующее меню
    if (current_role == ROLE_ADMIN) {
        adminMenu(users, clubs);
    } else if (current_role == ROLE_USER) {
        userMenu(clubs);
    }

    // Сохранение данных перед выходом
    saveClubsToFile(clubs);
    saveUsersToFile(users);

    std::cout << MSG_EXIT << std::endl;
    return 0;
}
