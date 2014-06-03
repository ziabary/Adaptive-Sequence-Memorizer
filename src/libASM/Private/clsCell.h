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

#include "clsASM.h"

#ifndef NULL
#define NULL 0
#endif

static inline void setBit(char& _var, bool _set, const char& _bitenum) {
    _var = (_set ? _var | _bitenum : _var & ~_bitenum);
}

class clsCell
{
    enum enuState
    {
        STATE_Active        = 0x01,
        STATE_Predicting    = 0x02,
        STATE_Learning      = 0x04,
        STATE_WasActive     = 0x10,
        STATE_WasPredicting = 0x20,
        STATE_WasLearning   = 0x40
    };

    struct stuConnection
    {
        clsCell*   Destination;
        unsigned short      Permanence;

        stuConnection(clsCell* _dest = NULL, unsigned short  _perm = 0){
            this->Destination = _dest;
            this->Permanence = _perm;
        }
    };

public:
    clsCell(ColID_t _colID){
        this->ColID = _colID;
        this->States = 0;
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

    inline bool hasConnection(){return this->Connection.Destination != NULL;}

    inline stuConnection& connection(){return this->Connection; }
    inline ColID_t colID(){return this->ColID;}

private:
    char States;
    stuConnection Connection;
    ColID_t       ColID;
};

#endif // CLSCELL_H
