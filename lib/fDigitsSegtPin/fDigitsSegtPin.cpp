/* fDigitsSegtPin
 * Copyright (C) fDigitsSegtPin contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ntcn
 */

#ifndef ARDUINO                           // This part is used to make a fool of Syntastic.
#ifndef Syntastic_h                       // Syntastic is a vim syntax checker.
#include "/home/klsz/Arduino/Syntastic.h" // It can act as a C++ checker,
#endif                                    // but not an Arduino checker.
#endif                                    // It wont affect compiling using Arduino IDE.

#ifdef ARDUINO
#include "Arduino.h"
#endif

#include "fDigitsSegtPin.h"

// Storage the pins
fDigitsSegtPin::fDigitsSegtPin(u8 vPf1, u8 vPf2, u8 vPf3, u8 vPf4, u8 vPf5, u8 vPf6, u8 vPf7, u8 vPf8, u8 vPf9, u8 vPf10, u8 vPf11, u8 vPf12)
{
    vPcD1 = vPf12;
    vPcD2 = vPf9;
    vPcD3 = vPf8;
    vPcD4 = vPf6;
    vPcA = vPf11;
    vPcB = vPf7;
    vPcC = vPf4;
    vPcD = vPf2;
    vPcE = vPf1;
    vPcF = vPf10;
    vPcG = vPf5;
    vPcH = vPf3;
}

// Init the pins
void fDigitsSegtPin::Begin(Type display_type, uint8_t number_of_digits)
{
    digits = number_of_digits;
    if (display_type == Type::COMMON_CATHODE)
    {
        segmentOnState = LOW;
        digitOnState = HIGH;
    }
    else
    {
        segmentOnState = HIGH;
        digitOnState = LOW;
    }
    segmentOffState = segmentOnState == LOW ? HIGH : LOW;
    digitOffState = digitOnState == LOW ? HIGH : LOW;
    pinMode(vPcD1, OUTPUT);
    pinMode(vPcD2, OUTPUT);
    pinMode(vPcD3, OUTPUT);
    pinMode(vPcD4, OUTPUT);
    pinMode(vPcA, OUTPUT);
    pinMode(vPcB, OUTPUT);
    pinMode(vPcC, OUTPUT);
    pinMode(vPcD, OUTPUT);
    pinMode(vPcE, OUTPUT);
    pinMode(vPcF, OUTPUT);
    pinMode(vPcG, OUTPUT);
    pinMode(vPcH, OUTPUT);

    digitalWrite(vPcD1, digitOffState);
    digitalWrite(vPcD2, digitOffState);
    digitalWrite(vPcD3, digitOffState);
    digitalWrite(vPcD4, digitOffState);
    digitalWrite(vPcA, segmentOffState);
    digitalWrite(vPcB, segmentOffState);
    digitalWrite(vPcC, segmentOffState);
    digitalWrite(vPcD, segmentOffState);
    digitalWrite(vPcE, segmentOffState);
    digitalWrite(vPcF, segmentOffState);
    digitalWrite(vPcG, segmentOffState);
    digitalWrite(vPcH, segmentOffState);
}

// Clean the afterglow
void fDigitsSegtPin::fvAfterGlow()
{
    digitalWrite(vPcA, segmentOffState);
    digitalWrite(vPcB, segmentOffState);
    digitalWrite(vPcC, segmentOffState);
    digitalWrite(vPcD, segmentOffState);
    digitalWrite(vPcE, segmentOffState);
    digitalWrite(vPcF, segmentOffState);
    digitalWrite(vPcG, segmentOffState);
    digitalWrite(vPcH, segmentOffState);
}
// Set the digit to write
void fDigitsSegtPin::fvSet(u8 vifDigit)
{
    switch (vifDigit)
    {
    case 0:
        digitalWrite(vPcD1, digitOnState);
        digitalWrite(vPcD2, digitOffState);
        digitalWrite(vPcD3, digitOffState);
        digitalWrite(vPcD4, digitOffState);
        break;
    case 1:
        digitalWrite(vPcD1, digitOffState);
        digitalWrite(vPcD2, digitOnState);
        digitalWrite(vPcD3, digitOffState);
        digitalWrite(vPcD4, digitOffState);
        break;
    case 2:
        digitalWrite(vPcD1, digitOffState);
        digitalWrite(vPcD2, digitOffState);
        digitalWrite(vPcD3, digitOnState);
        digitalWrite(vPcD4, digitOffState);
        break;
    case 3:
        digitalWrite(vPcD1, digitOffState);
        digitalWrite(vPcD2, digitOffState);
        digitalWrite(vPcD3, digitOffState);
        digitalWrite(vPcD4, digitOnState);
        break;
    default:
        break;
    }
}
// Write number to the digit
void fDigitsSegtPin::fvWrite(u8 vifNumber, u8 vifDot)
{
    digitalWrite(vPcA, villDigitTable[vifNumber][0] == 1 ? segmentOnState : segmentOffState);
    digitalWrite(vPcB, villDigitTable[vifNumber][1] == 1 ? segmentOnState : segmentOffState);
    digitalWrite(vPcC, villDigitTable[vifNumber][2] == 1 ? segmentOnState : segmentOffState);
    digitalWrite(vPcD, villDigitTable[vifNumber][3] == 1 ? segmentOnState : segmentOffState);
    digitalWrite(vPcE, villDigitTable[vifNumber][4] == 1 ? segmentOnState : segmentOffState);
    digitalWrite(vPcF, villDigitTable[vifNumber][5] == 1 ? segmentOnState : segmentOffState);
    digitalWrite(vPcG, villDigitTable[vifNumber][6] == 1 ? segmentOnState : segmentOffState);
    digitalWrite(vPcH, vifDot == 1 ? segmentOnState : segmentOffState);
}

// Print the digit with the number
void fDigitsSegtPin::fvPrint(u8 vifDigit, u8 vifNumber, u8 vifDot)
{
    fvSet(vifDigit);
    fvWrite(vifNumber, vifDot);
    delay(1);
    if (antiglow)
    {
        fvAfterGlow();
    }
}

void fDigitsSegtPin::SetAntiGlowEnabled(bool parameter)
{
    antiglow = parameter;
}

// Print user's input
void fDigitsSegtPin::Print(float vff)
{
    // Make sure these value can be printed
    if (vff >= 10000)
    {
        if (Serial && doReport_overRange)
            Serial.println("[Error](4Digit7Seg12Pin): Input value larger than 10000.");
        return;
    }
    if (vff < 0)
    {
        if (Serial && doReport_overRange)
            Serial.println("[Error](4Digit7Seg12Pin): Input value smaller than 0.");
        return;
    }

    // Print the values
    if (vff >= 1000)
    {
        uint8_t digitValue = vff / 1000;
        fvPrint(0, digitValue, 0);
        vff -= digitValue * 1000;
        digitValue = vff / 100;
        fvPrint(1, digitValue, 0);
        vff -= digitValue * 100;
        digitValue = vff / 10;
        fvPrint(2, digitValue, 0);
        vff -= digitValue * 10;
        fvPrint(3, (int)(vff), doPrint_lastDot);
    }
    else if (vff >= 100)
    {
        fvPrint(0, vff / 100, 0);
        vff -= (int)(vff / 100) * 100;
        fvPrint(1, vff / 10, 0);
        vff -= (int)(vff / 10) * 10;
        fvPrint(2, (int)(vff), 1);
        vff -= (int)(vff);
        vff *= 10;
        fvPrint(3, ((vff - (int)(vff)) > 0.5) ? ((int)(vff) + 1) : (int)(vff), 0);
    }
    else if (vff >= 10)
    {
        fvPrint(0, vff / 10, 0);
        vff -= (int)(vff / 10) * 10;
        fvPrint(1, (int)(vff), 1);
        vff -= (int)(vff);
        vff *= 10;
        fvPrint(2, (int)(vff), 0);
        vff -= (int)(vff);
        vff *= 10;
        fvPrint(3, ((vff - (int)(vff)) > 0.5) ? ((int)(vff) + 1) : (int)(vff), 0);
    }
    else
    {
        if (doPrint_firstZero == 1)
            fvPrint(0, 0, 0);
        fvPrint(1, (int)(vff), 1);
        vff -= (int)(vff);
        vff *= 10;
        fvPrint(2, (int)(vff), 0);
        vff -= (int)(vff);
        vff *= 10;
        fvPrint(3, ((vff - (int)(vff)) > 0.5) ? ((int)(vff) + 1) : (int)(vff), 0);
    }
}
