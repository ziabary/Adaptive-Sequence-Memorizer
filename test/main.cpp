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

#include <iostream>
#include "clsASM.h"
#include "DataGenerators/clsIncrementalSequenceGenerator.hpp"
#include "DataGenerators/clsFlashCardGenerator.hpp"

using namespace AdaptiveSequenceMemorizer;

clsASM ASM(clsASM::Configs(500,499,100,1));

void printPrediction(ColID_t _id, bool _learn = true)
{
    const clsASM::Prediction_t& Predictions = ASM.executeOnce(_id);
    /*if (_learn || _id == 0 || _id == 1)
        return;/**/
    std::cout<<"("<<_id<<"): "<<std::flush;
    for(auto Predicted : Predictions)
        std::cout<<Predicted.ColID<<"("<<Predicted.PathPermanence<<"),";
    std::cout<<std::endl;
}


int main()
{
    std::vector<ColID_t> Sequence;
    Sequence.push_back(0);
    Sequence.push_back(1);
    Sequence.push_back(2);
    Sequence.push_back(2);
    Sequence.push_back(3);
    Sequence.push_back(2);
    Sequence.push_back(4);

    ASM.execute(new clsIncrementalSequenceGenerator(Sequence));

    std::cout<<"************ LEARNING Finished **********"<<std::endl;
    ASM.save("asm.txt");
    ASM.load("asm.txt");
    ASM.save("asm2.txt");

    printPrediction(0, false);
//    printPrediction(1, false);
//    printPrediction(2, false);
    printPrediction(2, false);
//    printPrediction(3, false);
    printPrediction(3, false);
    printPrediction(2, false);
    printPrediction(4, false);
    printPrediction(2, false);
    printPrediction(5, false);

    return 0;
}
