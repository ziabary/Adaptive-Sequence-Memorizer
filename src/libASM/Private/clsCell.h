/*************************************************************************
 * ASM : An Adaptive Sequence Memorizer
 * Copyright (C) 2013-2014 S.M.Mohammadzadeh <mehran.m@aut.ac.ir>
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

#ifndef CLSCELL_H
#define CLSCELL_H

#include "clsASM_p.h"

#ifndef NULL
#define NULL 0
#endif

namespace AdaptiveSequenceMemorizer{

static inline void setBit(uint8_t& _var, bool _set, const uint8_t& _bitenum) {
    _var = (_set ? _var | _bitenum : _var & ~_bitenum);
}

typedef uint16_t ZIndex_t;

class clsCell
{
public:
    struct stuLocation{
        ColID_t ColID;
        ZIndex_t ZIndex;

        stuLocation(ColID_t _colID = NOT_ASSIGNED, ZIndex_t _zIndex = 0){
            this->ColID = _colID;
            this->ZIndex = _zIndex;
        }

        inline bool operator==(const stuLocation& _loc) const {
            return (this->ColID == _loc.ColID && this->ZIndex == _loc.ZIndex);
        }

        inline bool operator!=(const stuLocation& _loc) const {
            return ! (*this == _loc);
        }

        stuLocation& operator = (const stuLocation& _loc){
            this->ColID = _loc.ColID;
            this->ZIndex = _loc.ZIndex;
            return *this;
        }

        inline void clear(){
            this->ColID = NOT_ASSIGNED;
        }

        inline bool isEmpty(){
            return this->ColID == NOT_ASSIGNED;
        }
    };

    struct stuConnection
    {
        stuLocation       Destination;
        Permanence_t      Permanence;

        stuConnection(ColID_t _destCol = NOT_ASSIGNED, ZIndex_t _destZIndex = 0, Permanence_t  _perm = 0){
            Destination.ColID = _destCol;
            Destination.ZIndex = _destZIndex;
            this->Permanence = _perm;
        }

        stuConnection& operator = (const stuConnection& _con){
            this->Destination = _con.Destination;
            this->Permanence = _con.Permanence;
            return *this;
        }
    };

private:
    enum enuState
    {
        STATE_Active        = 0x01,
        STATE_Predicting    = 0x02,
        STATE_Learning      = 0x04,
        STATE_WasActive     = 0x10,
        STATE_WasPredicting = 0x20,
        STATE_WasLearning   = 0x40
    };

public:
    clsCell(ColID_t _colID,
            ZIndex_t _zIndex,
            char _states = 0,
            const stuConnection& _connection = stuConnection()){
        this->Loc.ColID = _colID;
        this->Loc.ZIndex = _zIndex;
        this->States = _states;
        this->Connection = _connection;
    }

    inline bool isActive() {return this->States & STATE_Active;}
    inline void setActiveState(bool _value){setBit(this->States, _value, STATE_Active);}

    inline bool isPredicting() {return this->States & STATE_Active;}
    inline void setPredictingState(bool _value){setBit(this->States, _value, STATE_Predicting);}

    inline bool isLearning() {return this->States & STATE_Learning;}
    inline void setLearningState(bool _value){setBit(this->States, _value, STATE_Learning);}

    inline bool wasActive()     {return this->States & STATE_WasActive;}
    inline void setWasActiveState(bool _value){setBit(this->States, _value, STATE_WasActive);}
    inline bool wasPredicting() {return this->States & STATE_WasPredicting;}
    inline void setWasPredictingState(bool _value){setBit(this->States, _value, STATE_WasPredicting);}
    inline bool wasLearning()   {return this->States & STATE_WasLearning;}
    inline void setWasLearningState(bool _value){setBit(this->States, _value, STATE_WasLearning);}

    inline uint16_t states(){
        return this->States;
    }

    inline bool hasConnection(){return this->Connection.Destination.ColID != NOT_ASSIGNED;}

    inline stuConnection& connection(){return this->Connection; }
    inline const stuLocation& loc(){return this->Loc;}

private:
    uint8_t States;
    stuConnection Connection;
    stuLocation   Loc;
};

}
#endif // CLSCELL_H
