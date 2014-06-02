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
    /**
     * @brief The Configs struct contains network configuration
     * @see executeOnce for more info
     */
    struct Configs
    {
        unsigned short  MinPermanence2Connect;
        unsigned short  InitialConnectionPermanence;
        unsigned short  PermanenceIncVal;
        unsigned short  PermanenceDecVal;

        /**
         * @brief Configs constructor
         * @param _initialConnectionPermanence Initial permanence value on new connections.
         * @param _minPermanence2Connect Minimum permanence value required in order to suppose cells connected
         * @param _permanenceIncVal Value to be increased to connection permanence value on correct predictions.
         * @param _permanenceDecVal Value to be decreased from connection permanence value on incorrect predictions
         */
        Configs(
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
    /**
     * @brief clsASM Base class implementing Adaptive Sequence Memorizer
     * @param _configs Configurations to be used in the memorizer.
     */
    clsASM(clsASM::Configs _configs = clsASM::Configs());

    /**
     * @brief executeOnce Main method used to both learn sequences and retrieve them.
     * On each _input if it is se to 0 sequence memorization and retrieval will be restarted.
     * So _input=0 can be used to separate different sequences. If _input > 0 and it was not
     * seen before (greatest ID on old inputs was less than _input) then it will be added to
     * valid inputs and consumed memory will be increased. Based on Old inputs (those after
     * the last _input=0) it will predict IDs  (if any) which can be seen after. If
     * _islearning=true then new patterns will make new connections between inputs with default
     * connection permanence set to Configs::InitialConnectionPermanence, correct predctions
     * (when next _input is one of those predicted) will be awarded by increasing connections
     * permanence on the prediction path using Configs::PermanenceIncVal, and those predctions
     * that were incorrect will be punished by Configs::PermanenceDecVal.
     * This way new sequences can be memorized and less used sequences can be forgotten because
     * their connection permanence will be decreased to less than Configs::MinPermanence2Connect
     * so they will be disconnected. Connections that has been decreased to zero will be removed.
     * When _isLearning = false all the permanence values will be frozen and no new connection
     * will be made. This is usefull for systems that will learn in a time period and then will
     * be used to just predict sequences.
     *
     *
     * @param _input ID of the sequence to be memorized.
     * @param _isLearning indicates wheter learn input or just predict next patternID
     * @return return A set of predicted PatternIDs based on input patterID sequences.
     */
    const std::unordered_set<ColID_t>& executeOnce(ColID_t _input, bool _isLearning = true);

protected:
    clsASMPrivate* pPrivate;
};

#endif // CLSASM_H
