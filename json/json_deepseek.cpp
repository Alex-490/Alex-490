#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <variant>
#include <sstream>
#include <cctype>

class JsonValue {
public:
    using Object = std::map<std::string, JsonValue>;
    using Array = std::vector<JsonValue>;
    using Value = std::variant<std::nullptr_t, bool, int, double, std::string, Object, Array>;

    JsonValue() : value(nullptr) {}
    JsonValue(Value v) : value(v) {}

    Value value;

    std::string toString() const {
        if (std::holds_alternative<std::nullptr_t>(value)) {
            return "null";
        } else if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value) ? "true" : "false";
        } else if (std::holds_alternative<int>(value)) {
            return std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::to_string(std::get<double>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            return "\"" + std::get<std::string>(value) + "\"";
        } else if (std::holds_alternative<Object>(value)) {
            std::string result = "{";
            for (const auto& [key, val] : std::get<Object>(value)) {
                result += "\"" + key + "\":" + val.toString() + ",";
            }
            if (result.size() > 1) result.pop_back(); // Remove trailing comma
            result += "}";
            return result;
        } else if (std::holds_alternative<Array>(value)) {
            std::string result = "[";
            for (const auto& val : std::get<Array>(value)) {
                result += val.toString() + ",";
            }
            if (result.size() > 1) result.pop_back(); // Remove trailing comma
            result += "]";
            return result;
        }
        return "";
    }
};

class JsonParser {
public:
    JsonParser(const std::string& json) : json(json), pos(0) {}

    JsonValue parse() {
        skipWhitespace();
        char ch = peek();
        if (ch == '{') {
            return parseObject();
        } else if (ch == '[') {
            return parseArray();
        } else if (ch == '"') {
            return parseString();
        } else if (ch == 't' || ch == 'f') {
            return parseBoolean();
        } else if (ch == 'n') {
            return parseNull();
        } else if (isdigit(ch) || ch == '-') {
            return parseNumber();
        } else {
            throw std::runtime_error("Unexpected character: " + std::string(1, ch));
        }
    }

private:
    std::string json;
    size_t pos;

    char peek() const {
        if (pos >= json.size()) throw std::runtime_error("Unexpected end of input");
        return json[pos];
    }

    void advance() {
        pos++;
    }

    void skipWhitespace() {
        while (pos < json.size() && isspace(json[pos])) {
            advance();
        }
    }

    JsonValue parseObject() {
        JsonValue::Object result;
        advance(); // Skip '{'
        skipWhitespace();
        while (peek() != '}') {
            std::string key = parseString().toString();
            skipWhitespace();
            if (peek() != ':') throw std::runtime_error("Expected ':'");
            advance(); // Skip ':'
            skipWhitespace();
            JsonValue value = parse();
            result[key] = value;
            skipWhitespace();
            if (peek() == ',') {
                advance(); // Skip ','
                skipWhitespace();
            } else if (peek() != '}') {
                throw std::runtime_error("Expected ',' or '}'");
            }
        }
        advance(); // Skip '}'
        return JsonValue(result);
    }

    JsonValue parseArray() {
        JsonValue::Array result;
        advance(); // Skip '['
        skipWhitespace();
        while (peek() != ']') {
            result.push_back(parse());
            skipWhitespace();
            if (peek() == ',') {
                advance(); // Skip ','
                skipWhitespace();
            } else if (peek() != ']') {
                throw std::runtime_error("Expected ',' or ']'");
            }
        }
        advance(); // Skip ']'
        return JsonValue(result);
    }

    JsonValue parseString() {
        advance(); // Skip '"'
        std::string result;
        while (peek() != '"') {
            result += peek();
            advance();
        }
        advance(); // Skip '"'
        return JsonValue(result);
    }

    JsonValue parseBoolean() {
        if (json.substr(pos, 4) == "true") {
            pos += 4;
            return JsonValue(true);
        } else if (json.substr(pos, 5) == "false") {
            pos += 5;
            return JsonValue(false);
        } else {
            throw std::runtime_error("Expected 'true' or 'false'");
        }
    }

    JsonValue parseNull() {
        if (json.substr(pos, 4) == "null") {
            pos += 4;
            return JsonValue(nullptr);
        } else {
            throw std::runtime_error("Expected 'null'");
        }
    }

    JsonValue parseNumber() {
        std::stringstream ss;
        if (peek() == '-') {
            ss << peek();
            advance();
        }
        while (isdigit(peek())) {
            ss << peek();
            advance();
        }
        if (peek() == '.') {
            ss << peek();
            advance();
            while (isdigit(peek())) {
                ss << peek();
                advance();
            }
        }
        double num;
        ss >> num;
        return JsonValue(num);
    }
};

int main() {
    std::string json = R"(
        {
            "name": "John Doe",
            "age": 30,
            "isStudent": false,
            "address": {
                "street": "123 Main St",
                "city": "Anytown",
                "state": "CA"
            },
            "phoneNumbers": [
                {
                    "type": "home",
                    "number": "212 555-1234"
                },
                {
                    "type": "office",
                    "number": "646 555-4567"
                }
            ]
        }
    )";

    JsonParser parser(json);
    JsonValue value = parser.parse();
    std::cout << value.toString() << std::endl;

    return 0;
}