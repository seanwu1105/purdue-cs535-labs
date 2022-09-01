#pragma once
template <typename T>
struct SuccessResult { const T value; };

struct FailureResult { const char* const message; };
