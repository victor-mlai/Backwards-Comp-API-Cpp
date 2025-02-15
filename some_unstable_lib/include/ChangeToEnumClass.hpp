#pragma once
#include <string>       // string_view
#include <type_traits>  // underlying_type_t

struct Text {
#ifndef BC_API_CHANGED
    enum Style {
        STYLE_BOLD,
        STYLE_ITALLIC,
        STYLE_STRIKE_THROUGH,
    };
#else
    enum class Style {
        Bold,
        STYLE_BOLD = Bold,
        Itallic,
        STYLE_ITALLIC = Itallic,
        StrikeThrough,
        STYLE_STRIKE_THROUGH = StrikeThrough,
    };

    static inline Style STYLE_BOLD = Style::Bold;
    static inline Style STYLE_ITALLIC = Style::Itallic;
    static inline Style STYLE_STRIKE_THROUGH = Style::StrikeThrough;

    // If the enum was used as bit flags, define bitwise operators as well.
    friend inline Style operator|(Style lhs, Style rhs) {
        using ut = std::underlying_type_t<Style>;
        return static_cast<Style>(static_cast<ut>(lhs) | static_cast<ut>(rhs));
    }
#endif
};

// If you have methods that received Style as int, overload them to receive Style
void Print(std::string_view /*text*/, int /*style*/) {}
#ifdef BC_API_CHANGED
void Print(std::string_view /*text*/, Text::Style /*style*/) {}
#endif
