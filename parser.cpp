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

    Object Parser::object()
    {
		Object o(Token{Tkn_Object{}});
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
				
				auto firstAppender = overload
				{
					[](const Tkn_Literal& l) {},
					[&first_obj](Tkn_Object& obj)
					{
						obj.value.push_back(first_obj);
					},
					[](const Tkn_Error& error) {}
				};
				std::visit(firstAppender,o.token);
                // o.token.value.push_back(first_obj);
                // optional objects
                bool list = true;
                while (list)
                {
					// actually need a whitespace
                    if (!accept(Tokenizer::Sym_Whitespace))
                    {
						// if end of list
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
                    Object list_obj = object();
					// errors just go to the top level
                    // if (list_obj.token == Object::Tkn_Error)
                    // {
                    //     o.token = Object::Tkn_Error;
                    //     return o;
                    // }
                    // o.token.value.push_back(list_obj);
					auto listAppender = overload
					{
						[](const Tkn_Literal& l) {},
						[&list_obj](Tkn_Object& obj)
						{
							obj.value.push_back(list_obj);
						},
						[](const Tkn_Error& error) {}
					};
					std::visit(listAppender,o.token);
                }
            }
        }
		// outer else:
		// happens if the object doesn't start with a left paren -> it's a literal (or later something else)
        else
        {
			// we try with the literal
            Object tmp = literal();
			bool finish = false;
			auto afterLiteral = overload
			{
				[&o,&finish](const Tkn_Literal& literal)
				{
					o = Object(Token{literal});
					finish = true;
				},
				[](const Tkn_Object& object)
				{},
				[&o](const Tkn_Error& err)
				{
					o = Object(Token{Tkn_Error{}});
				}
			};
			std::visit(afterLiteral, tmp.token);
			if (finish)
				return o;
        }
		return o;
    }

    Object Parser::parse(std::list<Tokenizer::SymbolObject> symbolStream)
    {
        iter = symbolStream.begin();
        return object();
    }

}
