#ifndef HANDRENDERER_H
#define HANDRENDERER_H

#define scale_ 0.01
#define jointSize 0.15
#define fingerSize 20
#define fingerColourNormal 2,2,2
#define fingerColourWarning 250,0,0

#include "Leap/Leap.h"
#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkLineSource.h"

#include "HandModel.h"

typedef double fingerJoints[5][3];          /// Each finger has 5 joints and 3 pt Vector
typedef double fingerBones[4][4];        /// Each Finger has 4 Bones and  4 pt Vector (Which includes  1 Length)

enum visibleHand  {rightHand, leftHand};

class HandRenderer
{
public:
    HandRenderer() ;

    void drawJoints(visibleHand activeHand, vtkRenderer *renderer);
    void drawBones(visibleHand activeHand, vtkRenderer *renderer);

     void translateHand(visibleHand activeHand, Leap::Hand movingHand, bool outsideBounds );

    HandModeller            *handModel;

    vtkActor                    *global_Joints[2][5][5];        /// 2 Hands,  5 FINGERS / 5 Joints
    vtkLineSource        *global_Bones[2][5][4];            /// 2 Hands, 5 FINGERS / 4 Bones
    vtkActor                    *global_Bone_Actor[2][5][4];            /// 5 Fingers / 4 Bones

    //double scale_ = 0.01;
//    double jointSize = 0.15;
//    double fingerSize = 20;
//    double fingerColourNormal [3] =  {2, 2, 2};
//    double fingerColourWarning [3] =  {255, 0, 0};

};

#endif // HANDRENDERER_H
