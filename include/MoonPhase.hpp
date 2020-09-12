/*
  MoonPhase.h - A Arduino library to get the moon's current phase
  Copyright (c) 2020 Ashley Sheaff.  All right reserved.
*/

// ensure this library description is only included once
#ifndef MoonPhase_hpp
#define MoonPhase_hpp


// library interface description
class MoonPhase
{
    // user-accessible "public" interface
public:
    MoonPhase();

    static float GetPhase(int nMonth, int nDay, int nYear);
    static double MyNormalize(double v);
    static int GetType(float phaseval);
    static int GetPercentage(float phaseval);

};

#endif

