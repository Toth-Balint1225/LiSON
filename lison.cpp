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

    void LiSON::deserialize(const std::string& src)
    {
        Tokenizer tokenizer;
        Parser parser;
        std::list<Tokenizer::SymbolObject> symbolStream = tokenizer.tokenize(src);
        Object o = parser.parse(symbolStream);
        interpret(o);
    }

    std::string LiSON::serialize() const
    {
        return revert().to_string();
    }

    // wstring <-> lison
    LiSON& operator >>(const std::string& src, LiSON& lison)
    {
        lison.deserialize(src);
        return lison;
    }

    std::string& operator <<(std::string& res, const LiSON& lison)
    {
        res =  lison.serialize();
        return res;
    }

    // serializer <-> lison
    LiSON& operator >>(const Serializer& serializer, LiSON& lison)
    {
        lison.deserialize(serializer.read());
        return lison;
    }

    const Serializer& operator <<(const Serializer& serializer, const LiSON& lison)
    {
        serializer.write(lison.serialize());
        return serializer;
    }

    // wstring -> serializer
    Serializer& operator >>(const std::string& filename, Serializer& serializer)
    {
        serializer.set_file(filename);
        return serializer;
    }

}