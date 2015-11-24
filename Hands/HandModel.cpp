#include "HandModel.h"
#include <sstream>
#include <stdio.h>
#include <string.h>



void HandModeller::jointRightStartPos(fingerJoints& joints, int finger)
{
    /// FINGER 5, JOINTS 5, 3 Point Vector
    switch (finger)
    {
    case 0:
    {

        fingerJoints finger1 =
        {
            {174.7,140.9,169.3},
            {174.7,140.9,169.3},
            {145.5,173.9,121.4},
            {129.4,194.6,85.6},
            {121.0,207.4,59.2},
        };
        memcpy(joints,finger1,sizeof(joints));
        break;
    }

    case 1:
    {

        fingerJoints finger2 =
        {
            {195.4,162.1,178.2},
            {202.8,222.5,104.2},
            {207.2,265.6,68.7},
            {206.0,281.0,41.3},
            {202.7,283.8,19.4},
        };
        memcpy(joints,finger2,sizeof(joints));
        break;
    }
    case 2:
    {
        fingerJoints finger3 =
        {
            {211.2,159.8,176.9},
            {230.0,213.0,105.7},
            {245.1,262.7,70.4},
            {248.4,284.6,40.7},
            {246.7,292.5,17.6},
        };
        memcpy(joints,finger3,sizeof(joints));
        break;
    }

    case 3:
    {
        fingerJoints finger4 =
        {
            {225.8,153.5,174.7},
            {253.8,196.1,111.0},
            {279.8,234.0,75.3},
            {289.5,250.0,44.4},
            {290.9,254.2,20.5},
        };
        memcpy(joints,finger4,sizeof(joints));
        break;
    }

    case 4:
    {
        fingerJoints finger5 =
        {
            {236.4,139.3,169.5},
            {272.4,175.5,113.7},
            {304.7,196.7,88.7},
            {318.2,205.1,68.8},
            {325.0,208.7,47.7},
        };
        memcpy(joints,finger5,sizeof(joints));
        break;
    }

    }   /// switch()
}

void HandModeller::jointLeftStartPos(fingerJoints& joints, int finger)
{
    /// FINGER 5, JOINTS 5, 3 Point Vector
    switch (finger)
    {
    case 0:
    {

        fingerJoints finger1 =
        {
            {-174.7,140.9,169.3},
            {-174.7,140.9,169.3},
            {-145.5,173.9,121.4},
            {-129.4,194.6,85.6},
            {-121.0,207.4,59.2},
        };
        memcpy(joints,finger1,sizeof(joints));
        break;
    }

    case 1:
    {

        fingerJoints finger2 =
        {
            {-195.4,162.1,178.2},
            {-202.8,222.5,104.2},
            {-207.2,265.6,68.7},
            {-206.0,281.0,41.3},
            {-202.7,283.8,19.4},
        };
        memcpy(joints,finger2,sizeof(joints));
        break;
    }
    case 2:
    {
        fingerJoints finger3 =
        {
            {-211.2,159.8,176.9},
            {-230.0,213.0,105.7},
            {-245.1,262.7,70.4},
            {-248.4,284.6,40.7},
            {-246.7,292.5,17.6},
        };
        memcpy(joints,finger3,sizeof(joints));
        break;
    }

    case 3:
    {
        fingerJoints finger4 =
        {
            {-225.8,153.5,174.7},
            {-253.8,196.1,111.0},
            {-279.8,234.0,75.3},
            {-289.5,250.0,44.4},
            {-290.9,254.2,20.5},
        };
        memcpy(joints,finger4,sizeof(joints));
        break;
    }

    case 4:
    {
        fingerJoints finger5 =
        {
            {-236.4,139.3,169.5},
            {-272.4,175.5,113.7},
            {-304.7,196.7,88.7},
            {-318.2,205.1,68.8},
            {-325.0,208.7,47.7},
        };
        memcpy(joints,finger5,sizeof(joints));
        break;
    }

    }   /// switch()
}

void HandModeller::boneRightStartPos(fingerBones& bones, int finger)
{
    /// FINGER 5, BONES 4, 3 Point Vector
    switch (finger)
    {
    case 0:
    {

        fingerBones finger1 =
        {
            {174.7,140.9,169.3,0.0},
            {174.7,140.9,169.3,43.4},
            {145.5,173.9,121.4,29.6},
            {129.4,194.6,85.6,20.3},
        };
        memcpy(bones,finger1,sizeof(bones));
        break;
    }

    case 1:
    {

        fingerBones finger2 =
        {
            {195.4,162.1,178.2,63.9},
            {202.8,222.5,104.2,37.3},
            {207.2,265.6,68.7,21.0},
            {206.0,281.0,41.3,14.8},
        };
        memcpy(bones,finger2,sizeof(bones));
        break;
    }
    case 2:
    {
        fingerBones finger3 =
        {
            {211.2,159.8,176.9,60.6},
            {230.0,213.0,105.7,41.9},
            {245.1,262.7,70.4,24.7},
            {248.4,284.6,40.7,16.3},
        };
        memcpy(bones,finger3,sizeof(bones));
        break;
    }

    case 3:
    {
        fingerBones finger4 =
        {
            {225.8,153.5,174.7,54.4},
            {253.8,196.1,111.0,38.8},
            {279.8,234.0,75.3,24.1},
            {289.5,250.0,44.4,16.2},
        };
        memcpy(bones,finger4,sizeof(bones));
        break;
    }

    case 4:
    {
        fingerBones finger5 =
        {
            {236.4,139.3,169.5,50.4},
            {272.4,175.5,113.7,30.7},
            {304.7,196.7,88.7,17.0},
            {318.2,205.1,68.8,15.0},
        };
        memcpy(bones,finger5,sizeof(bones));
        break;
    }

    }   /// switch()
}

void HandModeller::boneLeftStartPos(fingerBones& bones, int finger)
{
    /// FINGER 5, BONES 4, 3 Point Vector
    switch (finger)
    {
    case 0:
    {

        fingerBones finger1 =
        {
            {-174.7,140.9,169.3,0.0},
            {-174.7,140.9,169.3,43.4},
            {-145.5,173.9,121.4,29.6},
            {-129.4,194.6,85.6,20.3},
        };
        memcpy(bones,finger1,sizeof(bones));
        break;
    }

    case 1:
    {

        fingerBones finger2 =
        {
            {-195.4,162.1,178.2,63.9},
            {-202.8,222.5,104.2,37.3},
            {-207.2,265.6,68.7,21.0},
            {-206.0,281.0,41.3,14.8},
        };
        memcpy(bones,finger2,sizeof(bones));
        break;
    }
    case 2:
    {
        fingerBones finger3 =
        {
            {-211.2,159.8,176.9,60.6},
            {-230.0,213.0,105.7,41.9},
            {-245.1,262.7,70.4,24.7},
            {-248.4,284.6,40.7,16.3},
        };
        memcpy(bones,finger3,sizeof(bones));
        break;
    }

    case 3:
    {
        fingerBones finger4 =
        {
            {-225.8,153.5,174.7,54.4},
            {-253.8,196.1,111.0,38.8},
            {-279.8,234.0,75.3,24.1},
            {-289.5,250.0,44.4,16.2},
        };
        memcpy(bones,finger4,sizeof(bones));
        break;
    }

    case 4:
    {
        fingerBones finger5 =
        {
            {-236.4,139.3,169.5,50.4},
            {-272.4,175.5,113.7,30.7},
            {-304.7,196.7,88.7,17.0},
            {-318.2,205.1,68.8,15.0},
        };
        memcpy(bones,finger5,sizeof(bones));
        break;
    }

    }   /// switch()
}
