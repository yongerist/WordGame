//
// Created by liuzhengyangisme on 2023/5/24.
//
#include "user.h"
#include "header.h"
#include <thread>
using namespace std;

string enumToString(level lv) {
    switch (lv) {
        case low:
            return "low";
        case mid:
            return "mid";
        case high:
            return "high";
        case super:
            return "super";
        default:
            return "UNKNOWN";
    }
}

enum level stringToEnum(string str) {
    if (str == "low") {
        return low;
    } else if (str == "mid") {
        return mid;
    } else if (str == "high") {
        return high;
    } else if (str == "super") {
        return super;
    }
    // 处理无效的字符串
    throw std::invalid_argument("Invalid string for conversion to enum");
}

UserManager::UserManager() {
    conn = mysql_init(nullptr);
    if (mysql_set_character_set(conn, "utf8")) {
        cout << "Error: " << mysql_error(conn) << endl;
    }
    if (!mysql_real_connect(conn, host, user, password, db, port, nullptr, 0)) {
        std::cout << "Error: " << mysql_error(conn) << std::endl;
        exit(1);
    }
    //    cout << "-------------------------------Connection successful!-------------------------------" << endl;
}

UserManager::~UserManager() {
    mysql_close(conn);
    //    cout << "-------------------------------Connection stop!-------------------------------" << endl;
}

unsigned int UserManager::insert_user(std::string name, std::string pwd, int is_admin, enum level lv, int pointNum, int exp, int probNum) {
    //        插入数据   user为表名 其他都是列名一一对应变量
    //    if (mysql_query(conn, "INSERT INTO User ( name, password, is_admin, level,pointNum,exp, probNum ) VALUES ('may', '12345','0', 'low', '0','0','0')")) {
    char str[200];
    string op = "INSERT INTO User ( name, password, is_admin, level,pointNum,exp, probNum ) VALUES ('" + name + "','" + pwd + "','" + to_string(is_admin) + "','" + enumToString(lv) + "', '" + to_string(pointNum) + "', '" + to_string(exp) + "', '" + to_string(probNum) + "')";
    strcpy_s(str, op.c_str());
    //    cout << str << endl;
    if (mysql_query(conn, str)) {
        //        cout << "Error type: " << mysql_errno(conn) << endl;
        //        cout << "Error: " << mysql_error(conn) << endl;
        cout << "Error " << endl;
        return mysql_errno(conn);
    }
    return 0;
}

int UserManager::find_user(std::string name, std::string pwd) {
    MYSQL_RES res;
    MYSQL_ROW row;
    //    char column[30][40];
    std::string query = "SELECT * FROM User WHERE name = '" + name + "' AND password = '" + pwd + "'";
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error executing query: " << mysql_error(conn) << endl;
        return 0;
    }
    //取出选择的内容
    res = *mysql_store_result(conn);
    //    //统计 表 的字段
    //    unsigned short numFields = mysql_num_fields(&res);
    //    cout << "numFields:" << numFields << endl;
    //    //保存字段
    //    for (int i = 0; i < numFields; ++i) {
    //        strcpy(column[i], mysql_fetch_field(&res)->name);
    //        //      cout << "column[i]" << column[i] << endl;
    //    }
    //    7.打印每一行的内容
    //    while ((row = mysql_fetch_row(&res)) != nullptr) {
    //        for (int j = 0; j < numFields; j++) {
    //            cout << column[j] << "=" << row[j] << endl;
    //        }
    //    }
    if ((row = mysql_fetch_row(&res)) != nullptr) {
        //        cout << "row[7]:" << row[7] << endl;
        cout << "Login successful!" << endl;
        return std::stoi(row[7]);
    } else {
        cout << "Login failed! User name or password is incorrect" << std::endl;
        return 0;
    }
}
User *UserManager::find_user(int id) {
    MYSQL_RES res;
    MYSQL_ROW row;
    std::string query = "SELECT * FROM User WHERE id = '" + to_string(id) + "'";
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error executing query: " << mysql_error(conn) << endl;
        return nullptr;
    }
    //取出选择的内容
    res = *mysql_store_result(conn);
    if ((row = mysql_fetch_row(&res)) != nullptr) {
        int is_admin = stoi(row[3]);
        if (is_admin) {
            cout << "Find the admin!" << endl;
            auto *new_user = new Admin(row[0], stringToEnum(row[1]), row[2], is_admin, stoi(row[6]));
            return new_user;
        } else {
            cout << "Find the player!" << endl;
            auto *new_user = new Player(row[0], stringToEnum(row[1]), row[2], is_admin, stoi(row[4]), stoi(row[5]));
            return new_user;
        }
    } else {
        cout << "Didn't find the person!" << std::endl;
        return nullptr;
    }
}
unsigned int UserManager::insert_word(std::string my_word, std::string my_level) {
    char str[200];
    string op = "INSERT INTO word ( word, level ) VALUES ('" + my_word + "','" + my_level + "')";
    strcpy_s(str, op.c_str());
    //    cout << str << endl;
    if (mysql_query(conn, str)) {
        cout << "Error type: " << mysql_errno(conn) << endl;
        cout << "Error: " << mysql_error(conn) << endl;
        return mysql_errno(conn);
    }
    return 0;
}
string UserManager::getWord(int g_level) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    string word;
    int my_lv = (g_level > 3) ? 3 : g_level;
    string query = "SELECT word FROM word WHERE level = " + to_string(my_lv) + " ORDER BY RAND() LIMIT 1";
    if (mysql_query(conn, query.c_str())) {
        cout << "Query error: " << mysql_error(conn) << endl;
        return word;
    }
    // 获取查询结果
    res = mysql_use_result(conn);
    if ((row = mysql_fetch_row(res)) != nullptr) {
        word = row[0];
        mysql_free_result(res);
        return word;
    } else {
        cout << "no word!" << endl;
        mysql_free_result(res);
        return word;
    }
}
unsigned int UserManager::upd_player(Player *pPlayer) {
    char str[200];
    string op = "UPDATE  user SET  level = '" + enumToString(pPlayer->lv) + "' ,pointNum = " + to_string(pPlayer->pointNum) + ",exp =  " + to_string(pPlayer->exp) + " WHERE name ='" + pPlayer->name + "'";
    strcpy_s(str, op.c_str());
    //    cout << str << endl;
    if (mysql_query(conn, str)) {
        cout << "Error type: " << mysql_errno(conn) << endl;
        cout << "Error: " << mysql_error(conn) << endl;
        return mysql_errno(conn);
    }
    //    cout << "更新成功" << endl;
    return 0;
}
unsigned int UserManager::upd_admin(Admin *pAdmin) {
    char str[200];
    string op = "UPDATE  user SET  level = '" + enumToString(pAdmin->lv) + "' ,probNum = " + to_string(pAdmin->probNum) + " WHERE name ='" + pAdmin->name + "'";
    strcpy_s(str, op.c_str());
    //    cout << str << endl;
    if (mysql_query(conn, str)) {
        cout << "Error type: " << mysql_errno(conn) << endl;
        cout << "Error: " << mysql_error(conn) << endl;
        return mysql_errno(conn);
    }
    //    cout << "更新成功" << endl;
    return 0;
}
User *UserManager::find_user(std::string name) {
    MYSQL_RES res;
    MYSQL_ROW row;
    std::string query = "SELECT * FROM User WHERE name = '" + name + "'";
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error executing query: " << mysql_error(conn) << endl;
        return nullptr;
    }
    //取出选择的内容
    res = *mysql_store_result(conn);
    if ((row = mysql_fetch_row(&res)) != nullptr) {
        int is_admin = stoi(row[3]);
        if (is_admin) {
            cout << "Find the admin!" << endl;
            auto *new_user = new Admin(row[0], stringToEnum(row[1]), row[2], is_admin, stoi(row[6]));
            return new_user;
        } else {
            cout << "Find the player!" << endl;
            auto *new_user = new Player(row[0], stringToEnum(row[1]), row[2], is_admin, stoi(row[4]), stoi(row[5]));
            return new_user;
        }
    } else {
        cout << "Didn't find the person!" << std::endl;
        return nullptr;
    }
}

bool compareByExp(const std::vector<std::string> &user1, const std::vector<std::string> &user2) {
    int exp1 = std::stoi(user1[5]);
    int exp2 = std::stoi(user2[5]);
    return exp1 > exp2;
}
bool compareByLv(const std::vector<std::string> &user1, const std::vector<std::string> &user2) {
    enum level exp1 = stringToEnum(user1[1]);
    enum level exp2 = stringToEnum(user2[1]);
    return exp1 > exp2;
}
bool compareByNum(const std::vector<std::string> &user1, const std::vector<std::string> &user2) {
    int exp1 = std::stoi(user1[4]);
    int exp2 = std::stoi(user2[4]);
    return exp1 > exp2;
}
bool compareByPro(const std::vector<std::string> &user1, const std::vector<std::string> &user2) {
    int exp1 = std::stoi(user1[6]);
    int exp2 = std::stoi(user2[6]);
    return exp1 > exp2;
}
void UserManager::rank([[maybe_unused]] Player *pPlayer, int op) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    vector<vector<string>> data;// 二维列表，用于存储字段值
    char str[200];
    std::string query = "SELECT * FROM user";
    strcpy_s(str, query.c_str());
    //    cout << str << endl;
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error executing query: " << mysql_error(conn) << endl;
    }
    //取出选择的内容
    res = mysql_store_result(conn);
    while ((row = mysql_fetch_row(res)) != nullptr) {
        vector<string> rowValues;// 一维列表，用于存储每行记录的字段值
        for (unsigned int i = 0; i < mysql_num_fields(res); i++) {
            rowValues.emplace_back(row[i]);// 将每个字段的值添加到一维列表中
        }
        data.push_back(rowValues);// 将一维列表添加到二维列表中
    }
    //    for (const auto &arow: data) {
    //        for (const auto &value: arow) {
    //            std::cout << value << " ";
    //        }
    //        std::cout << std::endl;
    //    }
    if (op == 2) std::sort(data.begin(), data.end(), compareByExp);
    if (op == 1) std::sort(data.begin(), data.end(), compareByLv);
    if (op == 3) std::sort(data.begin(), data.end(), compareByNum);
    // 打印排序后的结果
    for (const auto &userInfo: data) {
        if (userInfo[3] == "0") {
            std::cout << "Name: " << userInfo[0] << ", level: " << userInfo[1] << ", game number: " << userInfo[4] << ", exp: " << userInfo[5] << std::endl;
        }
    }
    data.clear();
    mysql_free_result(res);
}

void UserManager::rank([[maybe_unused]] Admin *pAdmin, int op) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    vector<vector<string>> data;// 二维列表，用于存储字段值
    char str[200];
    std::string query = "SELECT * FROM user";
    strcpy_s(str, query.c_str());
    //    cout << str << endl;
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error executing query: " << mysql_error(conn) << endl;
    }
    //取出选择的内容
    res = mysql_store_result(conn);
    while ((row = mysql_fetch_row(res)) != nullptr) {
        vector<string> rowValues;// 一维列表，用于存储每行记录的字段值
        for (unsigned int i = 0; i < mysql_num_fields(res); i++) {
            rowValues.emplace_back(row[i]);// 将每个字段的值添加到一维列表中
        }
        data.push_back(rowValues);// 将一维列表添加到二维列表中
    }
    //    for (const auto &arow: data) {
    //        for (const auto &value: arow) {
    //            std::cout << value << " ";
    //        }
    //        std::cout << std::endl;
    //    }
    if (op == 2) std::sort(data.begin(), data.end(), compareByPro);
    if (op == 1) std::sort(data.begin(), data.end(), compareByLv);
    // 打印排序后的结果
    for (const auto &userInfo: data) {
        if (userInfo[3] == "1") {
            std::cout << "Name: " << userInfo[0] << ", level: " << userInfo[1] << ", problem number: " << userInfo[6] << std::endl;
        }
    }
    data.clear();
    mysql_free_result(res);
}

Player::Player(string my_name, enum level my_lv, string my_pwd, int my_is_admin, int my_pointNum, int my_exp) {
    name = my_name;
    lv = my_lv;
    pwd = my_pwd;
    is_admin = my_is_admin;
    pointNum = my_pointNum;
    exp = my_exp;
}

Admin::Admin(std::string my_name, enum level my_lv, std::string my_pwd, int my_is_admin, int my_probNum) {
    name = my_name;
    lv = my_lv;
    pwd = my_pwd;
    is_admin = my_is_admin;
    probNum = my_probNum;
}

void Player::Menu() {
    char operation = '1';
    getchar();
    while (operation != '0') {
        int menu = 1;
        while (menu) {
            system("cls");
            cout << "*****************************************" << endl;
            cout << "*        Word elimination game             *" << endl;
            cout << "* 1、Show   info                            *" << endl;
            cout << "* 2、play    game                           *" << endl;
            cout << "* 3、Query  user                            *" << endl;
            cout << "* 4、View   ranking                         *" << endl;
            cout << "* 0、Exit                                   *" << endl;
            cout << "*****************************************" << endl;
            cout << "Please select: ";
            string str;
            getline(cin, str);// 为了防止非法输入，这里使用了getline函数，先读入一行，再对字符串进行处理
            if (str.size() != 1) {
                cout << "Invalid operation!" << endl;
                system("pause");
                menu = 1;
                continue;
            }
            char s = str.at(0);
            if (s >= '0' && s <= '4') {
                menu = 0;
                operation = s;
            } else {
                cout << "Invalid operation!" << endl;
                system("pause");
            }
        }
        switch (operation) {
            case '1':
                this->Showinfo();//显示所有信息
                break;
            case '2':
                this->PlayGames();//添加单词
                break;
            case '3':
                QueryUser();//查询用户
                break;
            case '4':
                this->ViewRanking();//查看排行
                break;
            case '0':
                cout << "Thank you for your use！" << endl;//退出程序
                system("pause");
                break;
            default:
                cout << "Invalid operation!" << endl;
                break;
        }
    }
    cout << "**************************************************The End**************************************************" << endl;//退出程序
}
void Player::PlayGames() {
    UserManager userManager;
    int g_level = 1;
    int display_time = 5;
    int out = 0;
    while (!out) {
        out = 0;
        system("cls");
        cout << "level " << to_string(g_level) << " game begin!" << endl;
        // 获取一个单词
        string word = userManager.getWord(g_level);
        //展示单词
        displayWord(word, display_time);
        string userInput;
        // 用户输入
        cout << "Enter the word that just appeared：";
        //获取开始时间点
        auto startTime = std::chrono::high_resolution_clock::now();
        cin >> userInput;
        // 获取结束时间点
        auto endTime = std::chrono::high_resolution_clock::now();
        // 计算耗时时间
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        // 输出耗时时间
        long long count = duration.count();
        std::cout << "Elapsed time: " << count << " milliseconds" << std::endl;
        // 检查输入是否正确
        if (userInput == word) {
            cout << "Congratulations on passing the level " << g_level << " game！" << endl;
            this->pointNum++;
            this->exp += g_level + ((count > 2500) ? 0 : 1);
            if (this->exp >= (this->lv * 5)) {
                int intValue = static_cast<int>(this->lv);
                intValue++;
                // 将整数类型转换回枚举类型
                this->lv = static_cast<level>(intValue);
            }
            //            cout << "你花了 " << count << " ms！" << endl;
            //            cout << "你的exp增加了 " << g_level + ((count > 2000) ? 0 : 1) << endl;
            //            cout << "你的level为 " << this->lv << endl;
            g_level++;     // 进入下一关
            display_time--;// 单词显示时间减少，增加难度
        } else {
            cout << "Wrong, game over！" << endl;
            system("pause");
            out = 1;
        }
    }
}


void Player::ViewRanking() {
    system("cls");

    //    cout << "ViewRanking is ready " << endl;
    cout << "What do you want to sort by?" << endl;
    cout << "1. level  2. exp  3. game" << endl;
    UserManager userManager;
    int op;
    cin >> op;
    userManager.rank(this, op);

    system("pause");
}

void Player::Showinfo() {
    system("cls");

    //    cout << "Showinfo is ready" << endl;
    cout << "your name: " << this->name << endl;
    cout << "your level: " << this->lv << endl;
    cout << "your problem pointNum: " << this->pointNum << endl;
    cout << "your problem exp: " << this->exp << endl;
    system("pause");
}

void Player::displayWord(string word, int time) {
    cout << "Word appearance：" << word << endl;
    this_thread::sleep_for(chrono::seconds(time));
    system("cls");
}


void Admin::Menu() {
    char operation = '1';
    getchar();
    while (operation != '0') {
        int menu = 1;
        while (menu) {
            system("cls");
            cout << "*****************************************" << endl;
            cout << "*        Word elimination game             *" << endl;
            cout << "* 1、Show   info                            *" << endl;
            cout << "* 2、Add    words                           *" << endl;
            cout << "* 3、Query  user                            *" << endl;
            cout << "* 4、View   ranking                         *" << endl;
            cout << "* 0、Exit                                   *" << endl;
            cout << "*****************************************" << endl;
            cout << "Please select: ";
            string str;
            getline(cin, str);// 为了防止非法输入，这里使用了getline函数，先读入一行，再对字符串进行处理
            if (str.size() != 1) {
                cout << "Invalid operation!" << endl;
                system("pause");
                menu = 1;
                continue;
            }
            char s = str.at(0);
            if (s >= '0' && s <= '4') {
                menu = 0;
                operation = s;
            } else {
                cout << "Invalid operation!" << endl;
                system("pause");
            }
        }
        switch (operation) {
            case '1':
                this->Showinfo();//显示所有信息
                break;
            case '2':
                this->AddWord();//添加单词
                break;
            case '3':
                QueryUser();//查询用户
                break;
            case '4':
                this->ViewRanking();//查看排行
                break;
            case '0':
                cout << "Thank you for your use！" << endl;//退出程序
                system("pause");
                break;
            default:
                cout << "Invalid operation!" << endl;
                break;
        }
    }
    cout << "**************************************************The End**************************************************" << endl;//退出程序
}
void Admin::Showinfo() {
    //    cout << "Showinfo is ready" << endl;
    system("cls");

    cout << "your name: " << this->name << endl;
    cout << "your level: " << this->lv << endl;
    cout << "your problem number: " << this->probNum << endl;
    system("pause");
}
void Admin::AddWord() {
    system("cls");

    string new_word;
    string my_level = "1";
    //    cout << "AddWord is ready " << endl;
    cout << "Please type the word you want to add:" << endl;
    cin >> new_word;
    if (new_word.size() > 5 and new_word.size() <= 10) {
        my_level = '2';
    } else if (new_word.size() > 10) {
        my_level = '3';
    }
    UserManager userManager;
    unsigned int is_err = userManager.insert_word(new_word, my_level);
    if (is_err == 1062) {
        cout << "ERROR: It's already in the thesaurus!" << endl;
    } else if (is_err == 0) {
        cout << "Add successfully!" << endl;
        this->probNum++;
        if (this->probNum > ((this->lv) * 5)) {
            int intValue = static_cast<int>(this->lv);
            intValue++;
            // 将整数类型转换回枚举类型
            this->lv = static_cast<level>(intValue);
        }
    }
    system("pause");
}


void Admin::ViewRanking() {
    //    cout << "ViewRanking is ready " << endl;
    system("cls");

    cout << "What do you want to sort by?" << endl;
    cout << "1. level  2. problem number " << endl;
    UserManager userManager;
    int op;
    cin >> op;
    userManager.rank(this, op);
    system("pause");
}

void User::QueryUser() {
    system("cls");

    string name;
    cout << "Please enter the name of the user you want to query:" << endl;
    cin >> name;
    UserManager userManager;
    User *user = userManager.find_user(name);
    if (user != nullptr) {
        if (user->is_admin == 0) {
            // 将基类指针转换为派生类指针
            auto *me = dynamic_cast<Player *>(user);
            //            cout << "Find the player!" << endl;
            cout << "name:  " << me->name << endl;
            cout << "level:  " << me->lv << endl;
            cout << "pointNum:  " << me->pointNum << endl;
            cout << "exp:  " << me->exp << endl;
        } else {
            auto *me = dynamic_cast<Admin *>(user);
            //            cout << "Find the Admin!" << endl;
            cout << "name:  " << me->name << endl;
            cout << "level:  " << me->lv << endl;
            cout << "probNum:  " << me->probNum << endl;
        }
    }
    system("pause");
}
