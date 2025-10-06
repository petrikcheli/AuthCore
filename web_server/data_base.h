#pragma once
#include <sqlite_modern_cpp.h>
#include <string>
#include <vector>
#include <optional>
#include <iostream>

struct User {
    int id;
    std::string username;
    std::string full_name;
    std::string role;
    std::string created_at;
    std::string updated_at;
};

struct Controller {
    int id;
    std::string name;
    std::string location;
};

class data_base {
public:
    data_base(const std::string& db_path) : db(db_path) {
        init();
    }

    // --- Создание таблиц ---
    void init() {
        db <<
            "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "username TEXT NOT NULL UNIQUE,"
            "password_hash TEXT NOT NULL,"
            "full_name TEXT NOT NULL,"
            "role TEXT NOT NULL CHECK(role IN ('admin','user','viewer')),"
            "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
            "updated_at DATETIME DEFAULT CURRENT_TIMESTAMP"
            ");";

        db <<
            "CREATE TABLE IF NOT EXISTS controllers ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL,"
            "location TEXT"
            ");";

        db <<
            "CREATE TABLE IF NOT EXISTS user_access ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "user_id INTEGER NOT NULL,"
            "controller_id INTEGER NOT NULL,"
            "access_level TEXT NOT NULL CHECK(access_level IN ('read','write','admin')),"
            "FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE,"
            "FOREIGN KEY(controller_id) REFERENCES controllers(id) ON DELETE CASCADE,"
            "UNIQUE(user_id, controller_id)"
            ");";

        db <<
            "CREATE TRIGGER IF NOT EXISTS update_users_timestamp "
            "AFTER UPDATE ON users "
            "FOR EACH ROW "
            "BEGIN "
            "UPDATE users SET updated_at = CURRENT_TIMESTAMP WHERE id = OLD.id;"
            "END;";
    }

    // --- Добавление пользователя ---
    void addUser(const std::string& username, const std::string& password_hash,
                 const std::string& full_name, const std::string& role) {
        db << "INSERT INTO users (username, password_hash, full_name, role) VALUES (?, ?, ?, ?);"
           << username
           << password_hash
           << full_name
           << role;
    }

    // --- Получение пользователя по username ---
    std::optional<User> getUserByUsername(const std::string& username) {
        std::optional<User> result;
        db << "SELECT id, username, full_name, role, created_at, updated_at FROM users WHERE username = ?;"
           << username
            >> [&](int id, std::string uname, std::string fname, std::string role, std::string created, std::string updated) {
                  result = User{id, uname, fname, role, created, updated};
              };
        return result;
    }

    // --- Обновление пароля ---
    void updatePassword(int user_id, const std::string& new_hash) {
        db << "UPDATE users SET password_hash = ? WHERE id = ?;"
           << new_hash
           << user_id;
    }

    // --- Добавление контроллера ---
    void addController(const std::string& name, const std::string& location = "") {
        db << "INSERT INTO controllers (name, location) VALUES (?, ?);"
           << name
           << location;
    }

    // --- Получение всех контроллеров ---
    std::vector<Controller> getControllers() {
        std::vector<Controller> controllers;
        db << "SELECT id, name, location FROM controllers;"
            >> [&](int id, std::string name, std::string location) {
                  controllers.push_back({id, name, location});
              };
        return controllers;
    }

    // --- Установка прав доступа ---
    void setAccess(int user_id, int controller_id, const std::string& access_level) {
        db << "INSERT OR REPLACE INTO user_access (user_id, controller_id, access_level) VALUES (?, ?, ?);"
           << user_id
           << controller_id
           << access_level;
    }

    // --- Проверка доступа ---
    bool checkAccess(int user_id, int controller_id, const std::string& required) {
        std::string level;
        bool found = false;

        db << "SELECT access_level FROM user_access WHERE user_id = ? AND controller_id = ?;"
           << user_id
           << controller_id
            >> [&](std::string access_level) {
                  level = access_level;
                  found = true;
              };

        if (!found) return false;
        if (level == "admin") return true;
        if (level == required) return true;
        if (level == "write" && required == "read") return true;
        return false;
    }

private:
    sqlite::database db;
};
