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

#ifndef CLSASM_H
#define CLSASM_H

#include <list>

namespace AdaptiveSequenceMemorizer{

class clsASMPrivate;

typedef uint32_t ColID_t;
typedef uint16_t Permanence_t;
static ColID_t NOT_ASSIGNED = UINT32_MAX;

/**
 * @brief The intfInputIterator class is used to derive classes in order to be used in execute command
 */
class intfInputIterator
{
public:
    intfInputIterator(){}

    virtual ColID_t next() = 0;
};


class clsASM
{
public:

    /**
     * @brief The Configs struct contains network configuration
     * @see executeOnce for more info
     */
    struct Configs
    {
        Permanence_t  MinPermanence2Connect;
        Permanence_t  InitialConnectionPermanence;
        Permanence_t  PermanenceIncVal;
        Permanence_t  PermanenceDecVal;

        /**
         * @brief Configs constructor
         * @param _initialConnectionPermanence Initial permanence value on new connections.
         * @param _minPermanence2Connect Minimum permanence value required in order to suppose cells connected
         * @param _permanenceIncVal Value to be increased to connection permanence value on correct predictions.
         * @param _permanenceDecVal Value to be decreased from connection permanence value on incorrect predictions
         */
        Configs(
                Permanence_t  _initialConnectionPermanence = 500,
                Permanence_t  _minPermanence2Connect = 300,
                Permanence_t  _permanenceIncVal= 50,
                Permanence_t  _permanenceDecVal = 1
                )
        {
            this->InitialConnectionPermanence = _initialConnectionPermanence;
            this->MinPermanence2Connect = _minPermanence2Connect;
            this->PermanenceDecVal = _permanenceDecVal;
            this->PermanenceIncVal = _permanenceIncVal;
        }
    };

    enum enuLearningLevel{
        LearningFrozen,
        AwardAndPunishment,
        LearningFull
    };

    struct stuPrediction{
        ColID_t       ColID;
        Permanence_t  PathPermanence;
        stuPrediction(ColID_t _colID = NOT_ASSIGNED,
                  Permanence_t _perm = 0){
            this->ColID = _colID;
            this->PathPermanence = _perm;
        }
    };

    typedef std::list<clsASM::stuPrediction>  Prediction_t;

public:
    /**
     * @brief clsASM Base class implementing Adaptive Sequence Memorizer
     * @param _configs Configurations to be used in the memorizer.
     */
    clsASM(clsASM::Configs _configs = clsASM::Configs());

    /**
     * @brief executeOnce Main method used to both learn sequences and retrieve them.
     * Based on learning level there will be different actions:
     * 1- Learning Full:
     *  If @see _learningLevel is set to full then each input id will be learnt as a sequence of old inputs.
     *  @see _input = 0 is reserved to reset sequences and clear all predictions. If @see _input > 0 and
     *  it was not seen before (greatest ID on old inputs was less than _input) then it will be added to
     *  valid inputs and buffer sizes will be increased. Take note that non consecutive inputs will cause
     *  to reserve space for those input IDs expected between the las seen and the new @see _input ID.
     *  Based on the last seen input sequences next step will be predicted and returned as an unordered
     *  list of input IDs. Also new sequences will be memorized creating new cells and cell connections between
     *  last learning cell and the new input column cell. Each time with a new input if it was predicted in
     *  last step it will be awarded and it's connection permanence will be increased (@see Configs::PermanenceIncVal)
     *  at the same time incorrect predicted cells will be punished decreasing their permanence value (@see
     *  Configs::PermanenceDecVal).This way new sequences can be memorized and less used sequences can be
     *  forgotten because their connection permanence will be decreased to less than Configs::MinPermanence2Connect
     *  so they will be disconnected. Connections that has been decreased to zero will be removed.
     * 2- Award and Punishment:
     *   When @see _learningLevel is set AwardAndPunishment no new cells nor connections will be made but next step
     *   prediction, awarding correct predictions and punishment of incorrect ones will be made as described
     *   above.
     * 3- Frozen
     *   When @see _isLearning = Frozen all the permanence values will be frozen and no new connection
     *   will be made. This is usefull for systems that will learn in a time period and then will
     *   be used to just predict sequences without any feedback.
     *
     * @param _input ID of the current step sequence to be memorized or retrieved.
     * @param _learningLevel indicates how to change connection permanence valuse. See description
     * @return return A set of predicted PatternIDs based on input patterID sequences.
     */
    const Prediction_t& executeOnce(ColID_t _input,
                                                   enuLearningLevel _learningLevel = LearningFull);

    /**
     * @brief execute this method will show a sequence of patterns to the network using input generator
     * @param _inputGenerator a derived class from intfInputIterator which will generate inputs
     * @param _ticks how many times to iterate using Input Generator -1 means until end of data
     * @param _isLearning indicates wheter learn input or just predict next patternID
     * @return A set of predicted PatternIDs based on input patterID sequences
     */
    const Prediction_t& execute(intfInputIterator* _inputGenerator,
                                               int32_t _ticks = -1,
                                               enuLearningLevel _learningLevel = LearningFull);
    /**
     * @brief feedback external feedback to award or punishment of last prediction.
     * Awarding and Punishment score is controlled using @see _score.
     * @param _colID columnID to be awarded on positive feedback. and exceptional column on
     * negative feedbacks. if _colID=0 and _score is positive then nothing will happen but if _score
     * is negative then all predicted columns will be punished.
     * @param _score a real number between -2.0 and 2.0 which will be used as a multiplier to
     * Configs::PermanenceIncVal and Configs::PermanenceDecVal to award or punish. Positive _score will be
     * used to award and negative _score is used to punish. if _score = 0 then cell connection in the
     * specified column will be awarded by Configs::PermanenceIncVal and all other predicted cells will be
     * punished by Configs::PermanenceDecVal
     */
    void feedback(ColID_t _colID, double _score = 0);

    bool load(const char* _filePath, bool _throw = false);
    bool save(const char* _filePath);
protected:
    clsASMPrivate* pPrivate;
};
}
#endif // CLSASM_H
