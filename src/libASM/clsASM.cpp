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

#include <exception>
#include <iostream>
#include <fstream>
#include <climits>

#include "clsASM.h"
#include "Private/clsASM_p.h"

const char* FILE_SEGMENT_SEPARATOR = "**********";

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
bool clsASM::load(const char *_filePath, bool _throw)
{
    return this->pPrivate->load(_filePath, _throw);
}

/*************************************************************************************************************/
bool clsASM::save(const char *_filePath)
{
    return this->pPrivate->save(_filePath);
}

/*************************************************************************************************************/
clsASMPrivate::clsASMPrivate(clsASM::Configs _configs)
{
    this->LastLearningCell.clear();
    this->Configs = _configs;
}

/*************************************************************************************************************/
clsASMPrivate::~clsASMPrivate()
{
    this->reset();
}

/*************************************************************************************************************/
void clsASMPrivate::reset()
{
    for (auto ColIter = this->Columns.begin();
         ColIter != this->Columns.end();
         ColIter++){
        if (*ColIter == NULL)
            continue;

        for(auto CellIter = (*ColIter)->begin();
            CellIter != (*ColIter)->end();
            CellIter++)
            delete *CellIter;
        (*ColIter)->clear();
    }
    this->Columns.clear();
}

/*************************************************************************************************************/
void clsASMPrivate::executeOnce(ColID_t _activeColIndex, bool _isLearning)
{
    this->PredictedCols.clear();
    //On NULL pattern clear all history
    if (_activeColIndex == 0)
    {
        this->LastLearningCell.clear();
        this->PredictedCells.clear();
        this->FirstPattern = true;
        this->LastActiveColumn = _activeColIndex;
        return;
    }

    //if input column has not yet been seen do nothing as nothing
    //related has been learnt
    if (_isLearning == false &&
            (_activeColIndex > this->Columns.size() ||
             this->column(_activeColIndex) == NULL))
        return;

    //Expand columns if necessary
    if(_activeColIndex > this->Columns.size())
        this->Columns.resize(_activeColIndex, NULL);

    //Expansion creates Columns with no cell allocate when necessary
    if (this->column(_activeColIndex) == NULL)
        this->Columns[_activeColIndex - 1] = new clsColumn;

    //If this is the first pattern after NULL pattern
    if (this->FirstPattern)
    {
        if (this->column(_activeColIndex)->empty())
            this->column(_activeColIndex)->push_back(
                    new clsCell(_activeColIndex, this->column(_activeColIndex)->size()));

        for (auto CellIter = this->column(_activeColIndex)->begin();
             CellIter != this->column(_activeColIndex)->end();
             CellIter++)
            this->setPredictionState(*CellIter);

        this->LastLearningCell = this->column(_activeColIndex)->at(0)->loc();
        this->FirstPattern = false;
        this->LastActiveColumn = _activeColIndex;
        return;
    }

    clsCell* PredictiveCell = NULL;
    for(auto CellIter = this->column(_activeColIndex)->begin();
        CellIter != this->column(_activeColIndex)->end();
        CellIter++)
        if (this->cell((*CellIter)->loc())->wasPredicting())
        {
            PredictiveCell = (*CellIter);
            break;
        }

    if (PredictiveCell == NULL)
    {
        if (_isLearning)
        {
            //Learn new prediction
            clsCell* NewCell =
                    new clsCell(_activeColIndex, this->column(_activeColIndex)->size());
            NewCell->connection().Destination = LastLearningCell;
            NewCell->connection().Permanence = this->Configs.InitialConnectionPermanence;
            this->column(_activeColIndex)->push_back(NewCell);
        }
        this->removeOldPredictions();
    }
    else
    {
        LastLearningCell = PredictiveCell->loc();

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
                if (this->Configs.PermanenceDecVal && *CellIter != PredictiveCell->loc())
                    this->cell(*CellIter)->connection().Permanence = (
                            PredictiveCell->connection().Permanence < this->Configs.PermanenceDecVal ?
                                0 :
                                PredictiveCell->connection().Permanence - this->Configs.PermanenceDecVal
                            );
             }
        }
        this->removeOldPredictions();
        this->setPredictionState(this->cell(this->LastLearningCell));
    }
    this->LastActiveColumn = _activeColIndex;
}

/*************************************************************************************************************/
bool clsASMPrivate::load(const char *_filePath, bool _throw)
{
    try{
        this->reset();

        std::ifstream File;
        File.open(_filePath);
        if (File.is_open()){
            std::string Buff,Part1, Part2;
            size_t SepLoc, Line=0;
            bool ReadingConfigs = true;
            while(std::getline(File, Buff)){
                Line++;
                if (ReadingConfigs){
                    if (Buff == FILE_SEGMENT_SEPARATOR){
                        ReadingConfigs = false;
                        continue;
                    }
                    SepLoc = Buff.find(':');
                    if (SepLoc == std::string::npos)
                        throw std::logic_error("Colon missing on line: " + std::to_string(Line));

                    Part1 = Buff.substr(0,SepLoc);
                    Part2 = Buff.substr(SepLoc+1);
                    if (Part1.empty() || Part2.empty())
                        throw std::logic_error("Data missing on line: " + std::to_string(Line));

                    if (Part1 == "ICP"){
                        this->Configs.InitialConnectionPermanence = std::stoul(Part2);
                    }else if (Part1 == "MPC"){
                        this->Configs.MinPermanence2Connect = std::stoul(Part2);
                    }else if (Part1 == "PDV"){
                        this->Configs.PermanenceDecVal = std::stoul(Part2);
                    }else if (Part1 == "PIV"){
                        this->Configs.PermanenceIncVal = std::stoul(Part2);
                    }else if (Part1 == "MCS"){
                        this->Columns.resize(std::stoul(Part2), NULL);
                    }else
                        throw std::logic_error("Invalid identifier <" + Part1 + "> on line: " + std::to_string(Line));
                }else{
                    SepLoc = Buff.find(':');
                    if (SepLoc == std::string::npos)
                        throw std::logic_error("Colon missing at first on line: " + std::to_string(Line));

                    Part1 = Buff.substr(0,SepLoc);
                    Part2 = Buff.substr(SepLoc+1);
                    if (Part1.empty() || Part2.empty() || Part2.at(0) != '[' || Part2.at(Part2.size() - 1)!= ']')
                        throw std::logic_error("Data missing on line: " + std::to_string(Line));
                    ColID_t ColID = std::stoull(Part1);

                    this->Columns[ColID - 1] = new clsColumn;

                    clsCell::stuConnection Connection;
                    char States;
                    size_t NextInfoPos = Part2.find(']');
                    size_t StartPos = 0;
                    ZIndex_t ZIndex=0;
                    std::string InnerPart1, InnerPart2;
                    do{
                        Part1 = Part2.substr(StartPos + 1, NextInfoPos - StartPos - 1);
                        SepLoc = Part1.find(':');
                        if (SepLoc == std::string::npos)
                            throw std::logic_error("Colon missing on line: " +
                                                   std::to_string(Line) +
                                                   " for cell: " + std::to_string(ZIndex));
                        InnerPart1 = Part1.substr(0,SepLoc);
                        InnerPart2 = Part1.substr(SepLoc+1);
                        if (InnerPart1.empty())
                            throw std::logic_error("States missing on line: " +
                                                   std::to_string(Line) +
                                                   " for cell: " + std::to_string(ZIndex));

                        States = std::stoul(InnerPart1);
                        SepLoc = InnerPart2.find(':');
                        if (SepLoc == std::string::npos)
                            throw std::logic_error("Second Colon missing on line: " +
                                                   std::to_string(Line) +
                                                   " for cell: " + std::to_string(ZIndex));

                        InnerPart1 = InnerPart2.substr(0,SepLoc);
                        InnerPart2 = InnerPart2.substr(SepLoc+1);

                        if (InnerPart1.empty())
                            Connection.Destination.ColID=NOT_ASSIGNED;
                        else
                            Connection.Destination.ColID = std::stoull(InnerPart1);

                        SepLoc = InnerPart2.find(':');
                        if (SepLoc == std::string::npos)
                            throw std::logic_error("Third Colon missing on line: " +
                                                   std::to_string(Line) +
                                                   " for cell: " + std::to_string(ZIndex));

                        InnerPart1 = InnerPart2.substr(0,SepLoc);
                        InnerPart2 = InnerPart2.substr(SepLoc+1);
                        if (InnerPart1.empty())
                            Connection.Destination.ZIndex = 0;
                        else
                            Connection.Destination.ZIndex = std::stoull(InnerPart1);
                        if (InnerPart2.empty())
                            throw std::logic_error("Permanence missing on line: " +
                                                   std::to_string(Line) +
                                                   " for cell: " + std::to_string(ZIndex));
                        Connection.Permanence = std::stoul(InnerPart2);

                        this->column(ColID)->push_back(new clsCell(ColID,ZIndex++, States, Connection));

                        StartPos = NextInfoPos + 1;
                        NextInfoPos = Part2.find(']',StartPos);
                    }while(NextInfoPos != std::string::npos);
                }
            }
        }
    }catch(std::exception &e){
        if (_throw)
            throw;
        else{
            std::cerr<<e.what()<<std::endl;
            return false;
        }
    }

    this->executeOnce(0, false); // to reset anything.
    return true;
}

/*************************************************************************************************************/
bool clsASMPrivate::save(const char *_filePath)
{
    std::ofstream File;
    File.open(_filePath);
    if (File.is_open()){
        File<<"ICP:"<<this->Configs.InitialConnectionPermanence<<std::endl;
        File<<"MPC:"<<this->Configs.MinPermanence2Connect<<std::endl;
        File<<"PDV:"<<this->Configs.PermanenceDecVal<<std::endl;
        File<<"PIV:"<<this->Configs.PermanenceIncVal<<std::endl;
        File<<"MCS:"<<this->Columns.size()<<std::endl;
        File<<FILE_SEGMENT_SEPARATOR<<std::endl;
        int ColID = 0;
        for(auto ColIter : this->Columns){
            ColID++;
            if (ColIter && ColIter->size()){
                File<<ColID<<":";
                for(auto CellIter : *ColIter){
                    if (CellIter->hasConnection())
                        File<<"["<<
                              CellIter->states()<<":"<<
                              CellIter->connection().Destination.ColID<<":"<<
                              CellIter->connection().Destination.ZIndex<<":"<<
                              CellIter->connection().Permanence<<"]";
                    else
                        File<<"["<<
                              CellIter->states()<<":::"<<
                              CellIter->connection().Permanence<<"]";
                }
                File<<std::endl;
            }
        }
    }
}

/*************************************************************************************************************/
void clsASMPrivate::setPredictionState(clsCell* _activeCell)
{
    for (auto ColIter = this->Columns.begin();
         ColIter != this->Columns.end();
         ColIter++)
        if (*ColIter)
            for(auto CellIter = (*ColIter)->begin();
                CellIter != (*ColIter)->end();
                CellIter++)
                if (this->cell((*CellIter)->loc())->connection().Destination == _activeCell->loc() &&
                    this->cell((*CellIter)->loc())->connection().Permanence >= this->Configs.MinPermanence2Connect)
                {
                    this->cell((*CellIter)->loc())->setWasPredictingState(true);
                    this->PredictedCells.push_back((*CellIter)->loc());
                    this->PredictedCols.insert((*CellIter)->loc().ColID);
                }
}

/*************************************************************************************************************/
void clsASMPrivate::removeOldPredictions()
{
    for(auto CellIter = this->PredictedCells.begin();
        CellIter != this->PredictedCells.end();
        CellIter ++)
        this->cell(*CellIter)->setWasPredictingState(false);
    this->PredictedCells.clear();
}
