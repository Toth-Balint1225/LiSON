/**
 *     Copyright (C) 2022  Tóth Bálint
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "LiSON_base.h"

namespace lison
{
	// consume symbol, but don't use it
	// can be used to check if a special symbol is present
    bool Parser::accept(Tokenizer::Symbol req)
    {
        if (iter->sym == req)
        {
            ++iter;
            return true;
        }
        return false;
    }

    char Parser::character()
    {
        if (iter->sym == Tokenizer::Sym_Character)
        {
            char c =  iter->character;
            ++iter;
            return c;
        }
        else
            return 0;
    }

    Object Parser::literal()
    {
        if (accept(Tokenizer::Sym_Quote))
        {
            std::stringstream ss;
            char c;
            bool whitespace;
            bool loop = true;
            while (loop)
            {
                whitespace = accept(Tokenizer::Sym_Whitespace);
                if (whitespace)
                {
                    ss << ' ';
                    continue;
                }
                c = character();
                if (c != 0)
                    ss << c;
                loop = c != 0 || whitespace;
            }
            if (accept(Tokenizer::Sym_Quote))
            {
                // Object o;
                // o.token = Object::Tkn_Literal;
                // o.str = ss.str();
				Object o(Token{Tkn_Literal{ss.str()}});
                return o;
            }
        }
        Object o(Token{Tkn_Error{}});
        // o.token = Object::Tkn_Error;
        return o;
    }

	/*
	  Object => [LeftParen] -> [String | [Whitespace -> Object] -> [RightParen]
	            |------------------------|
	 */

    Object Parser::object()
    {
		Object o(Token{Tkn_Object{}});
		auto& token = std::get<Tkn_Object>(o.token);
		// yank the whitespaces
        while (accept(Tokenizer::Sym_Whitespace));

		// outer if: looks for left paren, that means a new Object object
        if (accept(Tokenizer::Sym_LeftParen))
        {
            // yank the whitespaces
            while (accept(Tokenizer::Sym_Whitespace));

			// if it's an empty object, we return
            if (accept(Tokenizer::Sym_RightParen))
                return o;
			// the object has some contents
            else 
            {
				// get the first object in the object
				// at this point there must be at least one
                Object first_obj = object();

				// put them object in with a visitor
				if (std::holds_alternative<Tkn_Error>(first_obj.token))
				{
					o = Object(Token{Tkn_Error{}});
					return o;
				}

				token.value.push_back(first_obj);

                // optional objects
                bool list = true;
                while (list)
                {
                    if (!accept(Tokenizer::Sym_Whitespace))
                    {
                        if (accept(Tokenizer::Sym_RightParen))
						{
							list = false;
							break;
						}
                    }
                    if (accept(Tokenizer::Sym_RightParen))
                    {
                        list = false;
                        break;
                    }

					if (iter == endIter)
					{
						o = Object(Token{Tkn_Error{}});
						list = false;
						break;
					}
                    Object list_obj = object();
					token.value.push_back(list_obj);
                }
            }
        }
		// outer else:
		// happens if the object doesn't start with a left paren -> it's a literal (or later something else)
        else
        {
			// we try with the literal
            Object tmp = literal();
			if (!std::holds_alternative<Tkn_Error>(tmp.token))
			{
				auto v = std::get<Tkn_Literal>(tmp.token);
				o = tmp;
			}
			// else try with integer, float and other types
        }
		return o;
    }

    Object Parser::parse(std::list<Tokenizer::SymbolObject> symbolStream)
    {
        iter = symbolStream.begin();
		endIter = symbolStream.end();
        return object();
    }

}
