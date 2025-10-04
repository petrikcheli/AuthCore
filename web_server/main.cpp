#include "crow.h"
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
    crow::SimpleApp app;

    // HTML форма логина
    CROW_ROUTE(app, "/").methods("GET"_method)([](const crow::request& req) {
        // std::ifstream file("templates//index.html");
        // if (!file.is_open()) {
        //     return crow::response(500, "Не найден index.html");
        // }
        // std::stringstream buffer;
        // buffer << file.rdbuf();
        // return crow::response(buffer.str());

        auto a_str = req.url_params.get("a");
        auto b_str = req.url_params.get("b");
        auto op    = req.url_params.get("op");

        std::string result_text = ""; // пустой результат по умолчанию

        if(a_str && b_str && op){
            double a = std::stod(a_str);
            double b = std::stod(b_str);
            double result;

            std::string operation(op);
            if(operation == "add") result = a + b;
            else if(operation == "sub") result = a - b;
            else if(operation == "mul") result = a * b;
            else if(operation == "div"){
                if(b == 0){ result_text = "Ошибка: деление на ноль"; }
                else result = a / b;
            } else result_text = "Ошибка: неизвестная операция";

            if(result_text.empty()) result_text = std::to_string(result);
        }
        //std::cout << result_text << std::endl;

        auto page = crow::mustache::load("index.html");
        crow::mustache::context ctx;
        ctx["result"] = result_text;
        // crow::response res;

        // res.set_header("Content-Type", "text/html; charset=UTF-8");
        // res.write(render_template("templates//index.html", result_text));

        return page.render(ctx);
        //return res;

    });

    app.port(18080).multithreaded().run();
}
