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

#include <exception>
#include <climits>
#include "clsASM.h"
#include "Private/clsASM_p.h"

/*************************************************************************************************************/
clsASM::clsASM(Configs _configs):
    pPrivate(new clsASMPrivate(_configs))
{
}

/*************************************************************************************************************/
const std::unordered_set<ColID_t>& clsASM::executeOnce(ColID_t _input, bool _isLearning)
{
    this->pPrivate->executeOnce(_input, _isLearning);
    return this->pPrivate->predictedCols();
}

/*************************************************************************************************************/
clsASMPrivate::clsASMPrivate(clsASM::Configs _configs)
{
    this->LastLearningCell = NULL;
    this->Configs = _configs;
}

/*************************************************************************************************************/
void clsASMPrivate::executeOnce(ColID_t _activeColIndex, bool _isLearning)
{
    this->PredictedCols.clear();
    //On NULL pattern clear all history
    if (_activeColIndex == 0)
    {
        this->LastLearningCell = NULL;
        this->PredictedCells.clear();
        this->FirstPattern = true;
        this->LastActiveColumn = _activeColIndex;
        return;
    }

    //Expand columns if necessary
    if(_activeColIndex > this->Cells.size())
        for(unsigned int i=0; i<= _activeColIndex - this->Cells.size(); i++)
            this->Cells.push_back(new std::vector<clsCell*>);

    //If this is the first pattern after NULL pattern
    if (this->FirstPattern)
    {
        if (this->Cells[_activeColIndex - 1]->empty())
            this->Cells[_activeColIndex - 1]->push_back(new clsCell(_activeColIndex));

        for (auto CellIter = this->Cells[_activeColIndex - 1]->begin();
             CellIter != this->Cells[_activeColIndex - 1]->end();
             CellIter++)
            this->setPredictionState(*CellIter);

        this->LastLearningCell = *this->Cells[_activeColIndex - 1]->begin();
        this->FirstPattern = false;
        this->LastActiveColumn = _activeColIndex;
        return;
    }

    clsCell* PredictiveCell = NULL;
    for(auto CellIter = this->Cells[_activeColIndex - 1]->begin();
        CellIter != this->Cells[_activeColIndex - 1]->end();
        CellIter++)
        if ((*CellIter)->wasPredicting())
        {
            PredictiveCell = (*CellIter);
            break;
        }

    if (PredictiveCell == NULL)
    {
        if (_isLearning)
        {
            //Learn new prediction
            clsCell* NewCell = new clsCell(_activeColIndex);
            NewCell->connection().Destination = LastLearningCell;
            NewCell->connection().Permanence = this->Configs.InitialConnectionPermanence;
            this->Cells[_activeColIndex - 1]->push_back(NewCell);
        }
        this->removeOldPredictions();
    }
    else
    {
        LastLearningCell = PredictiveCell;

        if (_isLearning)
        {
            //reinforce correct prediction
            PredictiveCell->connection().Permanence = (
                        SHRT_MAX - PredictiveCell->connection().Permanence < this->Configs.PermanenceIncVal ?
                            SHRT_MAX :
                            PredictiveCell->connection().Permanence + this->Configs.PermanenceIncVal);
            for(auto CellIter = this->PredictedCells.begin();
                CellIter != this->PredictedCells.end();
                CellIter ++)
            {
                //weaken incorrect prediction on all cells except the correct predicted one
                if (this->Configs.PermanenceDecVal && *CellIter != PredictiveCell)
                    (*CellIter)->connection().Permanence = (
                            PredictiveCell->connection().Permanence < this->Configs.PermanenceDecVal ?
                                0 :
                                PredictiveCell->connection().Permanence - this->Configs.PermanenceDecVal
                            );
             }
        }
        this->removeOldPredictions();
        this->setPredictionState(this->LastLearningCell);
    }
    this->LastActiveColumn = _activeColIndex;
}

/*************************************************************************************************************/
void clsASMPrivate::setPredictionState(clsCell* _activeCell)
{
    for (auto ColIter = this->Cells.begin();
         ColIter != this->Cells.end();
         ColIter++)
        for(auto CellIter = (*ColIter)->begin();
            CellIter != (*ColIter)->end();
            CellIter++)
            if ((*CellIter)->connection().Destination == _activeCell &&
                (*CellIter)->connection().Permanence >= this->Configs.MinPermanence2Connect)
            {
                (*CellIter)->setWasPredictingState(true);
                this->PredictedCells.push_back(*CellIter);
                this->PredictedCols.insert((*CellIter)->colID());
            }
}

/*************************************************************************************************************/
void clsASMPrivate::removeOldPredictions()
{
    for(auto CellIter = this->PredictedCells.begin();
        CellIter != this->PredictedCells.end();
        CellIter ++)
        (*CellIter)->setWasPredictingState(false);
    this->PredictedCells.clear();
}
