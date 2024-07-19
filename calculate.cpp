#include "calculate.h"
#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <cassert>

// Helper function to split a string by spaces
std::vector<std::string> split(const std::string& str) {
    std::istringstream iss(str);
    std::vector<std::string> result;
    std::string token;
    while (iss >> token) {
        result.push_back(token);
    }
    return result;
}

// Function to get the precedence of operators
constexpr int precedence(char op) {
    switch (op) {
        case '+': case '-': return 1;
        case '*': case '/': return 2;
        default: return 0;
    }
}

// Function to perform arithmetic operations
constexpr int applyOp(int a, int b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b == 0) {
                throw std::runtime_error("Division by zero");
            }
            return a / b;
        default: throw std::invalid_argument("Invalid operator");
    }
}

// Function to convert infix to postfix expression
std::expected<std::string, std::string> infixToPostfix(const std::string_view& expression) {
    std::stack<char> operators;
    std::stringstream postfix;
    int parenthesesBalance = 0;
    bool expectOperand = true; //either a digit or an opening parenthesis

    try {
        for (char c : expression) {
            if (std::isspace(c)) continue;

            if (std::isdigit(c)) {
                if (!expectOperand) return std::unexpected("Invalid expression: Unexpected operand");
                postfix << c;
                expectOperand = false;
            } else if (c == '(') {
                operators.push(c);
                ++parenthesesBalance;
                expectOperand = true;
            } else if (c == ')') {
                if (parenthesesBalance == 0) return std::unexpected("Invalid expression: Mismatched parentheses");
                while (!operators.empty() && operators.top() != '(') {
                    postfix << ' ' << operators.top();
                    operators.pop();
                }
                operators.pop(); // Remove '('
                --parenthesesBalance;
                expectOperand = false;  // after a subexpression, the next expected token should be an operator/another parenthesis
            } else {
                if (expectOperand) return std::unexpected("Invalid expression: Unexpected operator");
                while (!operators.empty() && precedence(operators.top()) >= precedence(c)) { //check if op at the top of the stack has greater precedence than the current
                    postfix << ' ' << operators.top();
                    operators.pop();
                }
                operators.push(c);
                postfix << ' ';
                expectOperand = true;
            }
        }

        if (parenthesesBalance != 0) return std::unexpected("Invalid expression: Mismatched parentheses");

        while (!operators.empty()) {
            postfix << ' ' << operators.top();
            operators.pop();
        }

        return postfix.str();
    } catch (const std::exception& e) {
        return std::unexpected(e.what());
    }
}

// Function to evaluate a postfix expression
std::expected<int, std::string> evaluatePostfix(const std::string& expression) {
    std::stack<int> stack;

    std::istringstream tokens(expression);
    std::string token;

    try {
        while (tokens >> token) {
            if (std::isdigit(token[0])) {
                stack.push(std::stoi(token)); //checks if the token is an operand
            } else {
                if (stack.size() < 2) return std::unexpected("Invalid expression: not enough operands");
                auto operand2 = stack.top(); stack.pop();
                auto operand1 = stack.top(); stack.pop();
                auto result = applyOp(operand1, operand2, token[0]);
                stack.push(result);
            }
        }

        if (stack.size() != 1) return std::unexpected("Invalid expression: too many operands");
        return stack.top();
    } catch (const std::exception& e) {
        return std::unexpected(e.what());
    }
}

template<typename T>
bool printErrors(std::expected<T, std::string>& exp){
    if(!exp.has_value()){
        std::cout << exp.error() << '\n';
        return false;
    } else {
        return true;
    }
}

// Test function
void runTests() {
    std::cout << "Running tests\n";
    // Basic Arithmetic Operations
    assert(evaluatePostfix(infixToPostfix("3 + 4").value()) == 7);
    assert(evaluatePostfix(infixToPostfix("10 - 5").value()) == 5);
    assert(evaluatePostfix(infixToPostfix("2 * 3").value()) == 6);
    assert(evaluatePostfix(infixToPostfix("8 / 2").value()) == 4);

    // Operations with Parentheses
    assert(evaluatePostfix(infixToPostfix("( 3 + 4 ) * 2").value()) == 14);
    assert(evaluatePostfix(infixToPostfix("2 * ( 3 + 4 )").value()) == 14);
    assert(evaluatePostfix(infixToPostfix("10 / ( 5 - 3 )").value()) == 5);
    assert(evaluatePostfix(infixToPostfix("( 2 + 3 ) * ( 4 - 1 )").value()) == 15);

    // Complex Expressions
    assert(evaluatePostfix(infixToPostfix("3 + 4 * 2").value()) == 11);
    assert(evaluatePostfix(infixToPostfix("3 + 4 * 2 / ( 1 - 5 )").value()) == 1);
    assert(evaluatePostfix(infixToPostfix("3 + 4 * 2 / ( 1 - 5 ) * 2 * 3").value()) == -21);

    std::cout << "Running catch tests\n";

    // Division by Zero Handling
    try {
        evaluatePostfix(infixToPostfix("10 / 0").value());
        assert(false); // Should not reach here
    } catch (const std::exception& e) {
        assert(std::string(e.what()) == "Division by zero");
    }

    // Invalid Expressions
    try {
        evaluatePostfix(infixToPostfix("3 + + 4").value());
        assert(false); // Should not reach here
    } catch (const std::exception& e) {
        assert(std::string(e.what()) == "Invalid expression: Unexpected operand");
    }

    try {
        evaluatePostfix(infixToPostfix("3 4 +").value());
        assert(false); // Should not reach here
    } catch (const std::exception& e) {
        assert(std::string(e.what()) == "Invalid expression: too many operands");
    }

    std::cout << "All tests passed!" << std::endl;
}

bool evaluate(const char *expression, int &result) {
    auto postfix = infixToPostfix(expression);
    auto ret_value = printErrors(postfix);

    if(ret_value){
        auto evalResult = evaluatePostfix(postfix.value());
        ret_value = printErrors(postfix);
        result = evalResult.value_or(0);
    }
    return ret_value;
}

int main() {
    const char *infixExpression = "3 + 4 * 2 / ( 1 - 5 ) * 2 * 3";
    int result;

    // Run tests
    runTests();
    bool bool_result = evaluate(infixExpression, result);

    if (bool_result) {
        std::cout << "Result: " << result << std::endl;

        return 0; // Success
    } else {
        return 1; // Failure
    }
}