#include "header.h"
#include "user.h"
#include "user_io.h"


int main() {
    std::cout << "welcome!" << std::endl;
    int id = in();
    UserManager userManager;
    User *user = userManager.find_user(id);
    if (user->is_admin == 0) {
        // 将基类指针转换为派生类指针
        auto* me = dynamic_cast<Player *>(user);
        me->Menu();
        userManager.upd_player(me);
    } else {
        auto* me = dynamic_cast<Admin *>(user);
        me->Menu();
        userManager.upd_admin(me);
    }

    return 0;
}
