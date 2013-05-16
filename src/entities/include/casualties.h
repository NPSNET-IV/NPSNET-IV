/*
 *  casualties.h
 *    Authors: Bryan C. Stewart
 *    Thesis Advisors:	Prof. John Falby
 *        		Dr. Dave Pratt
 *			Dr. Mike Zyda
 *    Date:  22 January 1996
 *
 *    Continuing Thesis work by Bryan Stewart
 *    Modified 29 January 96
 *
 */

#ifndef __CASUALTIES_H
#define __CASUALTIES_H

#define MAXPOINTS               2000
//#define XSTEAM                  176.6f
//#define YSTEAM                  -9.9f
//#define ZSTEAM                  7.45f
#define STEAMPOINTS             1000
#define STEAMLENGTH             0.75f
#define STEAMRADIUS             0.25f
#define STEAMDIR                90.0f
#define STEAMPITCH              0.0f
#define STEAMSIZE               5.0f

class CASUALTY_STEAM {

protected:
    pfLightPoint    *particles;
    pfVec3           velocity[STEAMPOINTS];
    pfVec3           dest[MAXPOINTS];
    pfVec4           color;
    pfVec3           origin;
    pfNode          *parent;

public:

    CASUALTY_STEAM(pfNode *,pfVec3);
    ~CASUALTY_STEAM();

    void move(float percent);
    void start();
    void stop();
    int isStarted();
};



//#define XOIL                    184.7f
//#define YOIL                    -7.9f
//#define ZOIL                    6.3f
#define OILPOINTS               1000
#define OILLENGTH               0.25f
#define OILRADIUS               0.15f
#define OILDIR                  0.0f
#define OILPITCH                45.0f
#define OILSIZE                 4.0f


class CASUALTY_OIL_LEAK {

protected:
    pfLightPoint    *particles;
    pfVec3           velocity[OILPOINTS];
    pfVec3           dest[MAXPOINTS];
    pfVec4           color;
    pfVec3           origin;
    pfNode          *parent;

public:

    CASUALTY_OIL_LEAK(pfNode *,pfVec3);
    ~CASUALTY_OIL_LEAK();

    void move(float percent);
    void start();
    void stop();
    int isStarted();
};


class CASUALTY_FIRE {

protected:

    int              flame_index;
    int              puffs_index;
    int              smoke_index;
    float            size, smoke_size;
    pfVec3           relposvec;
    pfVec3           loc;

public:

    CASUALTY_FIRE(pfVec3);
    ~CASUALTY_FIRE();

    void start(pfCoord &);
    void stop();
    int isStarted();
    int isSmoke();
    void update_size(float);
    void update_position(pfCoord &);
    void update_smoke_size(float, int);
    float *getposition() { return loc; }
};

#endif
