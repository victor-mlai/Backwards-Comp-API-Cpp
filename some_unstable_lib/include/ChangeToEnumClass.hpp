#pragma once

struct Logger {
#ifndef BC_API_CHANGED
    enum Handler {
        StdOut,
        StdErr,
        File,
    };
#else
    enum class Handler {
        StdOut,
        StdErr,
        File,
    };

    static constexpr Handler StdOut = Handler::StdOut;
    static constexpr Handler StdErr = Handler::StdErr;
    static constexpr Handler File = Handler::File;

    // If the enum was used as bit flags, define bitwise operators as well.
    //
    // If you expect Log(StdOut | StdErr) to still call `Log(int)`, then
    // the return type should be `int`, otherwise change it to `Handler`
    friend constexpr int operator|(Handler lhs, Handler rhs) {
        return static_cast<int>(lhs) | static_cast<int>(rhs);
    }
#endif

    // track which Log overload was called
    enum Tag { LogInt, LogEnum };

    constexpr Tag Log(int) const { return LogInt; }
    constexpr Tag Log(Handler) const { return LogEnum; }
};
