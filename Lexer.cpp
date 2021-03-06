#include "Lexer.h"

namespace aec
{
    Scanner::Scanner(const InputStream &text) : _buf({text.begin(), text.end()}), _cursor(0U)
    {
    }

    Scanner::Output Scanner::peek() const noexcept
    {
        return _buf.at(_cursor);
    }

    Scanner::Output Scanner::next() noexcept
    {
        if (_cursor < _buf.size())
            return _buf[_cursor++];
        else
            return Scanner::eof;
    }

    bool Scanner::is_eof() const noexcept
    {
        return _cursor >= _buf.size();
    }

} // end namespace aec

namespace aec
{
    Lexer::Lexer(string &&text) : _zin(std::forward<string>(text)), _buf(), _now(std::nullopt)
    {
    }

    Token Lexer::peek()
    {
        // return _now.value_or([this]() -> Token { _now = _iter_next(); return _now.value(); }());
        /// ^^^ this stmt can not work normally as followings, it's a trap and do not use it.

        if (not _now.has_value())
            _now = _iter_next();
        return _now.value_or(Token::eof);
    }

    Token Lexer::next()
    {
        if (_now.has_value())
        {
            auto old = _now.value();
            _now = _iter_next();
            return old;
        }
        else
        {
            auto next = _iter_next();
            _now = _iter_next();
            return next.value_or(Token::eof);
        }
    }

    bool Lexer::is_eof() const noexcept
    {
        return _zin.is_eof() && not _now.has_value();
    }

    Token Lexer::_eat_number()
    {
        do
        {
            _buf.push_back(_zin.next());
        } while (not _zin.is_eof() && std::isdigit(_zin.peek()));
        auto number = std::stoull(_buf);
        return _buf.clear(), number;
    }

    optional<Token> Lexer::_iter_next()
    {
        while (not _zin.is_eof())
        {
            auto ch = _zin.peek();
            if (ch == ' ' || ch == '\t')
            {
                _zin.next(); /// skip whitespace
                continue;
            }
            else if (std::isdigit(ch))
                return _eat_number();
            else
            {
                switch (_zin.next())
                {
                case '(':
                    return Delim::LeftParent;
                case ')':
                    return Delim::RightParent;
                case '+':
                    return Operator::Add;
                case '-':
                    return Operator::Sub;
                case '*':
                    return Operator::Mul;
                case '/':
                    return Operator::Div;

                default: /// error case
                    unexpected<void>("unexpected characher in source: {}", ch);
                }
            }
        }
        return std::nullopt;
    }
} // end namespace aec