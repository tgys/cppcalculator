#pragma once

#include <string>
#include <vector>
#include <expected>

// Function declarations

// Helper function to split a string by spaces
std::vector<std::string> split(const std::string& str);

// Function to get the precedence of operators
constexpr int precedence(char op);

// Function to perform arithmetic operations
constexpr int applyOp(int a, int b, char op);

// Function to convert infix to postfix expression
std::expected<std::string, std::string> infixToPostfix(const std::string_view& expression);

// Function to evaluate a postfix expression
std::expected<int, std::string> evaluatePostfix(const std::string& expression);

// Function to print errors from std::expected
template<typename T>
bool printErrors(std::expected<T, std::string>& exp);

// Test function
void runTests();

// Main evaluate function
bool evaluate(const char *expression, int &result);