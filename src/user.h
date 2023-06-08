//
// Created by liuzhengyangisme on 2023/5/24.
//

#ifndef WORD_GAME_USER_H
#define WORD_GAME_USER_H
#include "header.h"
#include <vector>

// 用户等级
enum level { low = 1,
             mid,
             high,
             super };

std::string enumToString(level lv);
//用户基类
class User {
    virtual int hello(){return 0;};
protected:

public:
    std::string pwd;
    std::string name;
    enum level lv;
    int is_admin;
    static void QueryUser();

};
//玩家类
class Player : public User {
private:

public:
    Player(std::string name, enum level lv,std::string pwd,int is_admin,int pointNum,int exp);
    void Menu();
    void Showinfo();
    int pointNum=0;//通过游戏关卡数
    int exp=0;     //经验值
    void PlayGames();
    void ViewRanking();
    static void displayWord(std::string word, int time);
};
//管理员
class Admin : public User {
private:

public:
    int probNum=0;//出题数目
    Admin(std::string name, enum level lv,std::string pwd,int is_admin,int probNum);
    void Menu();
    void Showinfo();
    void AddWord();
    void ViewRanking();
};


class UserManager {
public:
     unsigned int insert_user(std::string name, std::string pwd, int is_admin, enum level lv=low, int pointNum=0,int exp=0, int probNum=0);
     int find_user(std::string name, std::string pwd);
     User* find_user(int id);
     User* find_user(std::string name);
     unsigned int insert_word(std::string my_word, std::string my_level);
     std::string getWord(int g_level);
//    void update_user(User& me);
//    void delete_user(User& me);
//    std::vector<User>get_students(std::string condition="");
     ~UserManager();
     UserManager();

     unsigned int upd_player(Player *pPlayer);
     unsigned int upd_admin(Admin *pAdmin);
     void rank([[maybe_unused]] Player *pPlayer, int op);
     void rank([[maybe_unused]] Admin *pAdmin, int op);

 private:
    MYSQL *conn;
    const char *const host = "localhost";
    const char *const user = "lzy";
    const char *const password = "1234";
    const char *const db = "wordgame";
    const int port = 3306;
};
#endif//WORD_GAME_USER_H
