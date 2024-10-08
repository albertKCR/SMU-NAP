#include "SMUNAP.h"

void SMU::SerialInputInterpretation()
{
    int commaIndex, commaIndexAux, startIndex, numFloats, lastCommaIndex, index;
    String input = "";
    input = Serial.readString();
    String restOfString = "";
    int Technique;

    index = input.indexOf(',');

    if (index != -1)
    {
        Technique = input.substring(0, index).toInt();

        restOfString = input.substring(index + 1);
    }
    else
    {
        Technique = input.toInt();
        restOfString = "";
    }

    switch (Technique)
    {
    case 0:
        commaIndex = restOfString.indexOf(',');
        LSVStepTime = restOfString.substring(0, commaIndex).toInt();

        commaIndexAux = restOfString.indexOf(',', commaIndex + 1);
        LSVVoltageStep = (restOfString.substring(commaIndex + 1, commaIndexAux).toFloat() * 0.001);

        commaIndex = restOfString.indexOf(',', commaIndexAux + 1);
        LSVInitialVoltage = restOfString.substring(commaIndexAux + 1, commaIndex).toFloat();

        commaIndexAux = restOfString.indexOf(',', commaIndex + 1);
        LSVFinalVoltage = restOfString.substring(commaIndex + 1, commaIndexAux).toFloat();

        LSVStepTime = (LSVVoltageStep * 1000) / LSVStepTime;

        LinearSweepVoltammetry();
        break;
    case 1:
        commaIndex = restOfString.indexOf(',');
        CVStepTime = restOfString.substring(0, commaIndex).toInt();

        commaIndexAux = restOfString.indexOf(',', commaIndex + 1);
        CVVoltageStep = restOfString.substring(commaIndex + 1, commaIndexAux).toFloat() * 0.001;

        commaIndex = restOfString.indexOf(',', commaIndexAux + 1);
        CVInitialVoltage = restOfString.substring(commaIndexAux + 1, commaIndex).toFloat();

        commaIndexAux = restOfString.indexOf(',', commaIndex + 1);
        CVFinalVoltage = restOfString.substring(commaIndex + 1, commaIndexAux).toFloat();

        commaIndex = restOfString.indexOf(',', commaIndexAux + 1);
        CVPeakVoltage = restOfString.substring(commaIndexAux + 1, commaIndex).toFloat();

        commaIndexAux = restOfString.indexOf(',', commaIndex + 1);
        CVPeakVoltage2 = restOfString.substring(commaIndex + 1, commaIndexAux).toFloat();

        commaIndex = restOfString.indexOf(',', commaIndexAux + 1);
        CVCycles = restOfString.substring(commaIndexAux + 1, commaIndex).toInt();

        CVStepTime = (CVVoltageStep * 1000) / CVStepTime;

        CyclicSweepVoltammetry();
        break;
    case 2:
        commaIndex = restOfString.indexOf(',');
        SWVIntervalTime = restOfString.substring(0, commaIndex).toInt();
        SWVIntervalTime = 1000 / SWVIntervalTime;

        commaIndexAux = restOfString.indexOf(',', commaIndex + 1);
        SWVVoltageStep = restOfString.substring(commaIndex + 1, commaIndexAux).toFloat() * 0.001;

        commaIndex = restOfString.indexOf(',', commaIndexAux + 1);
        SWVInitialVoltage = restOfString.substring(commaIndexAux + 1, commaIndex).toFloat();

        commaIndexAux = restOfString.indexOf(',', commaIndex + 1);
        SWVFinalVoltage = restOfString.substring(commaIndex + 1, commaIndexAux).toFloat();

        commaIndex = restOfString.indexOf(',', commaIndexAux + 1);
        SWVPulseVoltage = restOfString.substring(commaIndexAux + 1, commaIndex).toFloat() * 0.001;

        SquareWaveVoltammetry();
        break;
    default:
        break;
    }
}

void SMU::LinearSweepVoltammetry()
{
    int timer = 0;
    int direction;

    float i = LSVInitialVoltage;

    if (LSVFinalVoltage > LSVInitialVoltage)
        direction = 1;
    else
        direction = -1;

    while ((direction == 1 && i < LSVFinalVoltage) || (direction == -1 && i > LSVFinalVoltage))
    {
        timer = millis();
        ApplyVoltage(i);
        ReadCurrent();

        while ((millis() - timer) < LSVStepTime)
        {
        }
        i += direction * LSVVoltageStep;
    }
    DAC.setVoltage(0, false);
}

void SMU::CyclicSweepVoltammetry()
{
    int contador = 0;
    int timer = 0;

    int direction;
    float i = CVInitialVoltage;

    if (CVPeakVoltage > CVInitialVoltage)
        direction = 1;
    else
        direction = -1;

    for (int cycle = 0; cycle < CVCycles; cycle++)
    {

        while ((direction == 1 && i < CVPeakVoltage) || (direction == -1 && i > CVPeakVoltage))
        {
            timer = millis();
            ApplyVoltage(i);
            ReadCurrent();

            while ((millis() - timer) < LSVStepTime)
            {
            }
            i += direction * CVVoltageStep;
        }

        if (CVPeakVoltage2 > CVPeakVoltage)
            direction = 1;
        else
            direction = -1;

        while ((direction == 1 && i < CVPeakVoltage2) || (direction == -1 && i > CVPeakVoltage2))
        {
            timer = millis();
            ApplyVoltage(i);
            ReadCurrent();

            count++;
            while ((millis() - timer) < LSVStepTime)
            {
            }
            i += direction * CVVoltageStep;
        }

        if (CVFinalVoltage > CVPeakVoltage2)
            direction = 1;
        else
            direction = -1;

        while ((direction == 1 && i < CVFinalVoltage) || (direction == -1 && i > CVFinalVoltage))
        {
            timer = millis();
            ApplyVoltage(i);
            ReadCurrent();

            while ((millis() - timer) < LSVStepTime)
            {
            }
            i += direction * CVVoltageStep;
        }
    }
    DAC.setVoltage(0, false);
}

void SMU::SquareWaveVoltammetry()
{
    int timer = 0;
    int direction;

    if (SWVFinalVoltage > SWVInitialVoltage)
        direction = 1;
    else
        direction = -1;

    float lastVoltage = SWVInitialVoltage;
    ApplyVoltage(lastVoltage);
    ReadCurrent();

    while ((direction == 1 && lastVoltage < SWVFinalVoltage) || (direction == -1 && lastVoltage > SWVFinalVoltage))
    {
        timer = millis();
        lastVoltage += direction * (SWVPulseVoltage + SWVVoltageStep);

        ApplyVoltage(lastVoltage);
        ReadCurrent();

        lastVoltage -= direction * SWVPulseVoltage;

        ApplyVoltage(lastVoltage);
        ReadCurrent();

        while ((millis() - timer) < DPVLowTime)
        {
        }
    }
    DAC.setVoltage(0, false);
}

void SMU::ApplyVoltage(float voltage)
{
    float DACVoltageBit;
    DACVoltageBit = voltage * (10 / 3.3F) + 1.723 - 0.02;
    DACVoltageBit = DACVoltageBit * (4095 / 5);
    DACVoltageBit = constrain(DACVoltageBit, 0, 4095);

    DAC.setVoltage(DACVoltageBit, false);
    float currentVoltage = (ads.readADC_Differential_0_1() * multiplier) - 0.5 + 0.05;

    dataString = String(voltage, 5);
}

void SMU::ReadCurrent()
{
    multiplier = 0.0001875F;
    float corrente = 1000000 *((ads.readADC_Differential_2_3() * multiplier) - 2.5) / 50000;
    
    dataString = dataString + "," + String(corrente, 4);
    Serial.println(dataString);
}
