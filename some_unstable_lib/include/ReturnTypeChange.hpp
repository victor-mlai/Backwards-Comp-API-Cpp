#pragma once
#include <optional>
#include <string>
#include <utility>

// Changing the return type from primitive `T` to `NewUserDefT`
#ifndef BC_API_CHANGED
// This method returns true if it succeeds, otherwise false.
// Change: Make this method return some error message as well
//  so the user knows why it failed (returned false)
bool CheckPassword(const std::string& pass) {
    if (pass.empty())
        return false;

    if (pass.starts_with("1234"))
        return false;

    return true;
}
#else
// Solution: return a new type that can be implicitly converted to bool.
struct CheckPasswordResult {
#ifdef BC_ALLOW_IMPLICIT_CONV
    /// If you need to allow any implicit conversions, e.g. `int x = CheckPassword();`
    operator bool() const { return !errMsg.has_value(); }
#else
    /// If you need to allow only implicit conversions to bool, e.g.: `bool x = CheckPassword();`
    template <typename T>
    explicit(!std::is_same_v<T, bool>) operator T() const {
        return !errMsg.has_value();
    }
#endif
	
	static CheckPasswordResult Err(std::string errMsg) { return {std::move(errMsg)}; }
	static CheckPasswordResult Ok() { return {std::nullopt}; }

    std::optional<std::string> errMsg;
};

/// This method now returns an error message when it fails.
CheckPasswordResult CheckPassword(const std::string& pass) {
    if (pass.empty())
        return CheckPasswordResult::Err("Password must not be empty.");

    if (pass == "1234")
        return CheckPasswordResult::Err("Password not strong enough.");

    return CheckPasswordResult::Ok();
}
#endif
