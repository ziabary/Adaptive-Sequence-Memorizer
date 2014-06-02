/*************************************************************************
 * ASM : An Adaptive Sequence Memorizer
 * Copyright (C) 2013-2014  S.M.Mohammadzadeh <mehran.m@aut.ac.ir>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *************************************************************************/
/**
 @author S.M.Mohammadzadeh <mehran.m@aut.ac.ir>
 */

#ifndef CLSASM_H
#define CLSASM_H

#include <unordered_set>

class clsASMPrivate;

typedef unsigned int ColID_t;

class clsASM
{
public:
    struct stuConfigs
    {
        unsigned short  MinPermanence2Connect;
        unsigned short  InitialConnectionPermanence;
        unsigned short  PermanenceIncVal;
        unsigned short  PermanenceDecVal;
        stuConfigs(
                unsigned short  _initialConnectionPermanence = 500,
                unsigned short  _minPermanence2Connect = 499,
                unsigned short  _permanenceIncVal= 100,
                unsigned short  _permanenceDecVal = 1
                )
        {
            this->InitialConnectionPermanence = _initialConnectionPermanence;
            this->MinPermanence2Connect = _minPermanence2Connect;
            this->PermanenceDecVal = _permanenceDecVal;
            this->PermanenceIncVal = _permanenceIncVal;
        }
    };

public:
    clsASM(stuConfigs _configs = stuConfigs());

    const std::unordered_set<ColID_t>& executeOnce(ColID_t _input, bool _isLearning = true);
    const std::unordered_set<ColID_t>& execute();

protected:
    clsASMPrivate* pPrivate;
};

#endif // CLSASM_H
