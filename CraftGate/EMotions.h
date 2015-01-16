
#ifndef _CG_ENUM_MOTIONS_H_
#define _CG_ENUM_MOTIONS_H_

namespace cg
{

enum E_CG_MOTION_POSES
{
    ECMP_STAND=0,
    ECMP_WALK,
    ECMP_UNKNOWN1,
    ECMP_RUNNING,
    ECMP_UNKNOWN2,
    ECMP_ATTACK,
    ECMP_CAST,
    ECMP_THROW,
    ECMP_UNDERATTACK,
    ECMP_GUARDING,
    ECMP_LYINGDOWN,
    ECMP_SEAT,
    ECMP_GLADHAND,
    ECMP_HAPPY,
    ECMP_ANGRY,
    ECMP_DISMAY,
    ECMP_NODDING,
    ECMP_ROCK,
    ECMP_SCISSORS,
    ECMP_PAPER,
    ECMP_FISHING,
};

enum E_CG_MOTION_DIRECTIONS
{
    ECMD_NORTH=0,
    ECMD_NORTHEAST,
    ECMD_EAST,
    ECMD_SOUTHEAST,
    ECMD_SOUTH,
    ECMD_SOUTHWEST,
    ECMD_WEST,
    ECMD_NORTHWEST,
};

static wchar_t const * MotionPoseNames[] =
{
    L"Stand",
    L"Walk",
    L"Unknown1",
    L"Running",
    L"Unknown2",
    L"Attack",
    L"Cast",
    L"Throw",
    L"Under Attack",
    L"Guarding",
    L"Lying Down",
    L"Seat",
    L"Glad Hand",
    L"Happy",
    L"Angry",
    L"Dismay",
    L"Nodding",
    L"Rock",
    L"Scissors",
    L"Paper",
    L"Fishing",
    0x0
};

static wchar_t const * MotionDirectionNames[] =
{
    L"North",
    L"Northeast",
    L"East",
    L"Southeast",
    L"South",
    L"Southwest",
    L"West",
    L"Northwest",
    0x0
};

}

#endif //_CG_ENUM_MOTIONS_H_