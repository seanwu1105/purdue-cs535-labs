#pragma once
template <typename T>
struct SuccessResult { const T value; };

struct ErrorResult { const char* const message; };
