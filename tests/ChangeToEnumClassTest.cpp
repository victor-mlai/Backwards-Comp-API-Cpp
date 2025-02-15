#include <string>
#include "ChangeToEnumClass.hpp"
#include "gtest/gtest.h"

void Print(std::string_view /*text*/, Text::Style /*style*/) {}

TEST(ChangeToEnumClass, Basic) {
    Print("test", {});
    Print("test", Text::STYLE_BOLD);
    Print("test", Text::STYLE_BOLD | Text::Style::STYLE_ITALLIC);
    Print("test", Text::Style::STYLE_BOLD | Text::STYLE_ITALLIC);
    Print("test", Text::Style::STYLE_BOLD | Text::Style::STYLE_ITALLIC);
}
