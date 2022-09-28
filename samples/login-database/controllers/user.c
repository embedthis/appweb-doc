/*
    user.c - User login
 */
#include "esp.h"

/*
    Action to login a user. Redirects to /public/login.esp if login fails
 */
static void loginUser() {
    if (httpLogin(getConn(), param("username"), param("password"))) {
        redirect("/index.esp");
    } else {
        feedback("error", "Invalid Login");
        redirect("/public/login.esp");
    }
}

/*
    Logout the user and redirect to the login page
 */
static void logoutUser() {
    httpLogout(getConn());
    redirect("/public/login.esp");
}

/*
    Dynamic module initialization
 */
ESP_EXPORT int esp_controller_login_database_user(HttpRoute *route)
{
    /*
        Define the login / logout actions
     */
    espDefineAction(route, "user-login", loginUser);
    espDefineAction(route, "user-logout", logoutUser);
    return 0;
}
