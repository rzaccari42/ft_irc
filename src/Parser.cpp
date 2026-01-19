/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 06:13:42 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/18 06:32:00 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include <cctype>

bool Parser::readToken(const std::string& s, size_t& i, std::string& token)
{
    size_t start = i;
    while (i < s.size() && s[i] != ' ')
        ++i;
    if (i == start) return false;
    token.assign(s, start, i - start);
    return true;
}

void Parser::skipSpaces(const std::string& s, size_t& i)
{
    while (i < s.size() && s[i] == ' ') ++i;
}

std::string Parser::toUpper(const std::string& s)
{
    std::string ret = s;
    for (size_t i = 0; i < ret.size(); ++i)
        ret[i] = (char)std::toupper((unsigned char)ret[i]);
    return ret;
}

bool Parser::parseMsg(const std::string& line, Message& out, std::string* err)
{
    out = Message();

    size_t i = 0;
    skipSpaces(line, i);
    if (i >= line.size()) {
        if (err) *err = "empty line";
        return false;
    }

    if (line[i] == ':') {
        ++i;
        std::string prefix;
        if (!readToken(line, i, prefix)) {
            if (err) *err = "prefix without token";
            return false;
        }
        out.hasPrefix = true;
        out.prefix = prefix;
        skipSpaces(line, i);
        if (i >= line.size()) {
            if (err) *err = "prefix without command";
            return false;
        }
    }

    std::string cmd;
    if (!readToken(line, i, cmd)) {
        if (err) *err = "missing command";
        return false;
    }
    out.command = toUpper(cmd);

    while (i < line.size()) {
        skipSpaces(line, i);
        if (i >= line.size()) break;
        if (line[i] == ':') {
            out.params.push_back(line.substr(i + 1));
            return true;
        }
        std::string p;
        if (!readToken(line, i, p)) break;
        out.params.push_back(p);
    }

    return true;
}
