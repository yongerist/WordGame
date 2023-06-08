//
// Created by liuzhengyangisme on 2023/5/24.
//
#include "user_io.h"
#include "header.h"
using namespace std;

int log_in() {

    UserManager userManager;
    string my_name;
    string my_pwd;
    int id = 0;
    while (id == 0) {
        cout << "Please enter your name: " << endl;
        cin >> my_name;
        cout << "Please enter your pwd:  " << endl;
        cin >> my_pwd;
        id = userManager.find_user(my_name, my_pwd);
    }
    return id;
}

int my_register() {
    string my_name;
    string my_pwd;
    int my_type;
    UserManager userManager;
    while (TRUE) {
        cout << "Please create your name: " << endl;
        cin >> my_name;
        cout << "Please create your pwd:  " << endl;
        cin >> my_pwd;
        cout << "Which type of account do you want to create(enter 0/1):" << endl;
        cout << "0.Player   1.Questioner" << endl;
        cin >> my_type;
        unsigned int error = userManager.insert_user(my_name, my_pwd, my_type);
        if (!error) {
            cout << "Congratulations on your successful registration!" << endl;
            break;
        } else if (error == 1062) {
            cout << "The name is occupied. Please enter another one!" << endl;
        }
    }
    return 0;
}


int in() {
    string have_account = "y";
    int id;
    while (true) {
        cout << "Do you have an account?[y/n]" << endl;
        cin >> have_account;
        if (have_account == "y") {
            id = log_in();
            break;
        } else if (have_account == "n") {
            my_register();
//            break;
        } else {
            cout << "ERROR: Please enter y or n!" << endl;
        }
    }
//        cout << "you're user " << id << endl;
    return id;
}
