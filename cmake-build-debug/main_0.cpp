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

// Helper function to find the index of a value in a vector, returns -1 if not found
template<typename T>
int indexOf(const std::vector<T>& vec, const T& value) {
    auto it = std::ranges::find(vec, value);
    if (it != vec.end()) {
        return std::distance(vec.begin(), it);
    }
    return -1;
}

// Function to get the precedence of operators
int precedence(char op) {
    if (op == '+' || op == '-') {
        return 1;
    }
    if (op == '*' || op == '/') {
        return 2;
    }
    return 0;
}

// Function to perform arithmetic operations
int applyOp(int a, int b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b == 0) {
                throw std::runtime_error("Division by zero");
            }
            return a / b;
    }
    return 0;
}

// Function to convert infix to postfix expression

std::string infixToPostfix(const std::string &expression) {
    std::stack<char> operators;
    std::stringstream postfix;
    int parenthesesBalance = 0;
    bool expectOperand = true;

    for (char c : expression) {
        if (std::isspace(c)) {
            continue;
        }

        if (std::isdigit(c)) {
            if (!expectOperand) {
                throw std::invalid_argument("Invalid expression: Unexpected operand");
            }
            postfix << c;
            expectOperand = false;
        } else if (c == '(') {
            operators.push(c);
            parenthesesBalance++;
            expectOperand = true;
        } else if (c == ')') {
            if (parenthesesBalance == 0) {
                throw std::invalid_argument("Invalid expression: Mismatched parentheses");
            }
            while (!operators.empty() && operators.top() != '(') {
                postfix << ' ' << operators.top();
                operators.pop();
            }
            operators.pop(); // Remove '('
            parenthesesBalance--;
            expectOperand = false;
        } else {
            if (expectOperand) {
                throw std::invalid_argument("Invalid expression: Unexpected operator");
            }
            while (!operators.empty() && precedence(operators.top()) >= precedence(c)) {
                postfix << ' ' << operators.top();
                operators.pop();
            }
            operators.push(c);
            postfix << ' ';
            expectOperand = true;
        }
    }

    if (parenthesesBalance != 0) {
        throw std::invalid_argument("Invalid expression: Mismatched parentheses");
    }

    while (!operators.empty()) {
        postfix << ' ' << operators.top();
        operators.pop();
    }

    return postfix.str();
}

// Function to evaluate a postfix expression
int evaluatePostfix(const std::string &expression) {
    std::stack<int> stack;
    std::istringstream tokens(expression);
    std::string token;
    while (tokens >> token) {
        if (std::isdigit(token[0])) {
            stack.push(std::stoi(token));
        } else {
            int operand2 = stack.top(); stack.pop();
            int operand1 = stack.top(); stack.pop();
            int result = applyOp(operand1, operand2, token[0]);
            stack.push(result);
        }
    }
    if (stack.size() != 1) {
        throw std::runtime_error("Invalid expression: too many operands");
    }
    return stack.top();
};

// Test function
void runTests() {

    std::cout << "running tests\n";
    // Basic Arithmetic Operations
    assert(evaluatePostfix(infixToPostfix("3 + 4")) == 7);
    assert(evaluatePostfix(infixToPostfix("10 - 5")) == 5);
    assert(evaluatePostfix(infixToPostfix("2 * 3")) == 6);
    assert(evaluatePostfix(infixToPostfix("8 / 2")) == 4);

    // Operations with Parentheses
    assert(evaluatePostfix(infixToPostfix("( 3 + 4 ) * 2")) == 14);
    assert(evaluatePostfix(infixToPostfix("2 * ( 3 + 4 )")) == 14);
    assert(evaluatePostfix(infixToPostfix("10 / ( 5 - 3 )")) == 5);
    assert(evaluatePostfix(infixToPostfix("( 2 + 3 ) * ( 4 - 1 )")) == 15);

    // Complex Expressions
    assert(evaluatePostfix(infixToPostfix("3 + 4 * 2")) == 11);
    assert(evaluatePostfix(infixToPostfix("3 + 4 * 2 / ( 1 - 5 )")) == 1);
    assert(evaluatePostfix(infixToPostfix("3 + 4 * 2 / ( 1 - 5 ) * 2 * 3")) == -21);

    std::cout << "running catch tests\n";

    // Division by Zero Handling
    try {
        evaluatePostfix(infixToPostfix("10 / 0"));
        assert(false); // Should not reach here
    } catch (const std::exception& e) {
        assert(std::string(e.what()) == "Division by zero");
    }

    // Invalid Expressions
    try {
        evaluatePostfix(infixToPostfix("3 + + 4"));
        assert(false); // Should not reach here
    } catch (const std::exception& e) {
        assert(std::string(e.what()) == "Invalid expression: not enough operands for the operator");
    }

    try {
        evaluatePostfix(infixToPostfix("3 4 +"));
        assert(false); // Should not reach here
    } catch (const std::exception& e) {
        assert(std::string(e.what()) == "Invalid expression: too many operands");
    }

    std::cout << "All tests passed!" << std::endl;
}

// Function to evaluate an infix expression
bool evaluate(const char *expression, int &result) {
    try {
        std::string postfixExpression = infixToPostfix(expression);
        result = evaluatePostfix(postfixExpression);
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

// Main function
int main() {
    const char *infixExpression = "3 + 4 * 2 / ( 1 - 5 ) * 2 * 3";
    int result;

    // Run tests
    runTests();

    if (evaluate(infixExpression, result)) {
        std::cout << "Result: " << result << std::endl;
        return 0; // Success
    } else {
        return 1; // Failure
    }
}
