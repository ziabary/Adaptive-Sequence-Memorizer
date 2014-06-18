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

namespace AdaptiveSequenceMemorizer {
/**
 *  @brief The clsFlashCardGenerator class
 *
 *  This is a state machine that takes a series inputs on initialization and a number of steps to display
 *  each input. After the pause (number of steps of dataGenerator calls), FlashCards will update the matrix argument
 *  with the next matrix in the series from the initializer.  At the end of the list of matrices, FlashCards rotates
 *  back to the beginning.
 *
 *  In the context of HTM Networks, this can be used to rotate through a series of static data inputs when training
 *  an HTM Network.
 **/
class clsFlashCardGenerator : public intfInputIterator
{
public:

    clsFlashCardGenerator(const std::vector<ColID_t>& _inputs, int _repetition){
        this->MaxRepetition = _repetition;
        this->InputList = _inputs;
        this->reset();
    }

    void reset(){
        this->CurrRepetition = 0;
        this->LastNotShown = 0;
    }

    ColID_t next(){
        if (this->CurrRepetition >= this->MaxRepetition)
            if (!(this->CurrRepetition = 0) && ++this->LastNotShown > this->InputList.size() - 1)
                this->reset();
        this->CurrRepetition++;
        return this->InputList[this->LastNotShown];
    }

private:
    int MaxRepetition;
    int CurrRepetition;
    std::vector<ColID_t> InputList;
    int LastNotShown;

};

}
