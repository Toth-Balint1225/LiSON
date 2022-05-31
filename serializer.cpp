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

    Serializer::Serializer(const std::string& _filename)
        :filename(_filename)
    {}

    void Serializer::set_file(const std::string& _filename)
    {
        this->filename = _filename;
    }

    std::string Serializer::read() const
    {
        if (filename.empty())
            return "";
        std::ifstream file;
        file.open(filename,std::ios_base::in);
        if (!file.is_open())
            return "";
        std::stringstream ss;
        ss << file.rdbuf();
        file.close();
        return ss.str();
    }

    void Serializer::write(const std::string& source) const
    {
        if (filename.empty())
            return;
        std::ofstream file;
        file.open(filename,std::ios_base::out);
        if (!file.is_open())
            return;
        file << source;
        file.close();
    }
}