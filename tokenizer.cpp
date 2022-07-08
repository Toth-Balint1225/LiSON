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
    std::list<Tokenizer::SymbolObject> Tokenizer::tokenize(const std::string& src)
    {
        std::list<SymbolObject> symbolStream;
		std::string numerics = "0123456789";
        for (unsigned i=0;i<src.length();i++)
        {
            char c = src[i];
            SymbolObject sym;
            if (c == '\'')
                sym.sym = Sym_Quote;
            else if (c == '(')
                sym.sym = Sym_LeftParen;
            else if (c == ')')
                sym.sym = Sym_RightParen;
            else if (c == '\t' || c == '\n' || c == ' ')
            {
                sym.sym = Sym_Whitespace;
            }
			else if (std::find(numerics.begin(),numerics.end(),c)
					 != numerics.end())
			{
				sym.sym = Sym_Numeric;
				sym.character = c;
			}
            else
            {
                sym.sym = Sym_Character;
                sym.character = c;
            }
            symbolStream.push_back(sym);
        }
        return symbolStream;
    }
}
