/*************************************************************************
 * ASM : An Adaptive Sequence Memorizer
 * Copyright (C) 2013-2014  S.Mohammad M. Ziabary <mehran.m@aut.ac.ir>
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
 @author S.Mohammad M. Ziabary <mehran.m@aut.ac.ir>
 */

#include "clsASM.h"
#include <vector>

/**
 * @brief The clsIncrementalSequenceGenerator class
 */
class clsIncrementalSequenceGenerator : public intfInputIterator
{
public:

    clsIncrementalSequenceGenerator(const std::vector<ColID_t>& _inputs){
        this->InputList = _inputs;
        this->CurrShown = -1;
        this->MaxShown = 0;
    }

    ColID_t next(){
        CurrShown++;
        if (this->CurrShown == this->MaxShown){
            this->MaxShown++;
            if (MaxShown > this->InputList.size())
                return NOT_ASSIGNED;
            CurrShown = 0;
        }

        return this->InputList[CurrShown];
    }

private:
    int CurrShown;
    int MaxShown;
    std::vector<ColID_t> InputList;
};

