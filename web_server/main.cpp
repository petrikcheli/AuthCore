#include "crow.h"

#include "password_hasher.h"

#include <fstream>
#include <sstream>

std::string load_html(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string render_template(const std::string& path, const std::string& result){
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string html = buffer.str();

    // Простая замена {{result}} на значение
    size_t pos = html.find("{{result}}");
    if(pos != std::string::npos){
        html.replace(pos, 10, result);
    }

    return html;
}

int main() {
    password_hasher::init();
    crow::SimpleApp app;

    // HTML форма логина
    CROW_ROUTE(app, "/").methods("GET"_method)([](const crow::request& req) {
        auto page = crow::mustache::load("login.html");
        return page.render();
    });

    CROW_ROUTE(app, "/login").methods("POST"_method)([](const crow::request& req){
        CROW_LOG_INFO << "Raw body: " << req.body;  // отладка

        auto data = crow::json::load(req.body);
        if (!data) {
            return crow::response(400, "Некорректный JSON. Получено: " + req.body);
        }

        std::string username = data["username"].s();
        std::string password = data["password"].s();

        if (username == "admin" && password == "1234") {
            return crow::response("<h1>Добро пожаловать, " + username + "!</h1>");
        } else {
            return crow::response(403, "<h1>Неверный логин или пароль</h1>");
        }
    });

    app.port(18080).multithreaded().run();
}
