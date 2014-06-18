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

typedef std::vector<clsCell*> clsColumn;

class clsASMPrivate
{
public:
    clsASMPrivate(clsASM::Configs _configs);
    ~clsASMPrivate();

    void executeOnce(ColID_t _activeColIndex, clsASM::enuLearningLevel _learningLevel);
    inline const std::unordered_set<ColID_t>& predictedCols() const{
        return this->PredictedCols;
    }
    bool load(const char* _filePath, bool _throw = false);
    bool save(const char* _filePath);
    void feedback(ColID_t _colID, double _score);

private:
    void award(ColID_t _colID, Permanence_t _pVal);
    void punish(ColID_t _colID, Permanence_t _pVal);

    void reset();
    void setPredictionState(clsCell *_activeCell);
    void removeOldPredictions();
    void removeCell(clsCell::stuLocation& _loc);
    inline clsCell* cell(const clsCell::stuLocation& _loc) const{
        return this->column(_loc.ColID)->at(_loc.ZIndex);
    }

    inline clsColumn* column(ColID_t _col) const{
        return this->Columns[_col - 1];
    }

private:
    clsCell::stuLocation               LastLearningCell;
    clsCell::stuLocation               LastPredictiveCell;
    ColID_t                            LastActiveColumn;
    bool                               FirstPattern;
    std::list<clsCell::stuLocation>    PredictedCells;
    std::unordered_set<ColID_t>        PredictedCols;
    std::vector<clsColumn*>            Columns;
    clsASM::Configs Configs;

    unsigned int ActiveCol;
};

#endif // CLSASM_P_H
