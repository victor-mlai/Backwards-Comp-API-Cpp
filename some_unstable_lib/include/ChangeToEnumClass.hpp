#pragma once

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
        return Style{static_cast<ut>(lhs) | static_cast<ut>(rhs)};
    }
#endif
};
