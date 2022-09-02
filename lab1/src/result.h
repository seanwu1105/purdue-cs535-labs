#pragma once
#include <string>

template <typename T>
struct SuccessResult { const T value; };

struct ErrorResult { const std::string message; };
