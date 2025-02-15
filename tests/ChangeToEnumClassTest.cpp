#include "ChangeToEnumClass.hpp"
#include "gtest/gtest.h"

TEST(ChangeToEnumClass, Basic) {
    Print("test", 0);
    Print("test", Text::STYLE_BOLD);
    Print("test", Text::STYLE_BOLD | Text::Style::STYLE_ITALLIC);
    Print("test", Text::Style::STYLE_BOLD | Text::STYLE_ITALLIC);
    Print("test", Text::STYLE_BOLD | Text::STYLE_ITALLIC | Text::STYLE_STRIKE_THROUGH);
    Print("test",
          Text::Style::STYLE_BOLD | Text::Style::STYLE_ITALLIC | Text::Style::STYLE_STRIKE_THROUGH);
}
