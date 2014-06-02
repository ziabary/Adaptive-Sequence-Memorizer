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

#ifndef CLSASM_P_H
#define CLSASM_P_H

#include <list>
#include <vector>
#include "clsASM.h"
#include "clsCell.h"


class clsASMPrivate
{
public:
    clsASMPrivate(clsASM::stuConfigs _configs);

    void executeOnce(ColID_t _activeColIndex, bool _isLearning);
    inline const std::unordered_set<ColID_t>& predictedCols(){
        return this->PredictedCols;
    }

private:
    void setPredictionState(clsCell *_activeCell);
    void removeOldPredictions();

private:
    clsCell*                           LastLearningCell;
    ColID_t                            LastActiveColumn;
    bool                               FirstPattern;
    std::list<clsCell*>                PredictedCells;
    std::unordered_set<ColID_t>        PredictedCols;
    std::vector<std::vector<clsCell*>* > Cells;
    clsASM::stuConfigs Configs;

    unsigned int ActiveCol;
};

#endif // CLSASM_P_H
