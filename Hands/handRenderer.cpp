#include "handRenderer.h"


#define HAND_SPHERE_RADIUS 7.0f
#define HAND_SPHERE_DETAIL 32
#define HAND_CYLINDER_RADIUS 4.0f

#include "vtkSphereSource.h"
#include "vtkSmartPointer.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"



HandRenderer::HandRenderer()
{
    this->handModel = new HandModeller();
}

void HandRenderer::drawJoints(visibleHand activeHand, vtkRenderer *renderer)
{
    vtkSmartPointer<vtkSphereSource> jointSource =
            vtkSmartPointer<vtkSphereSource>::New();
    jointSource->SetRadius(jointSize);

      vtkSmartPointer<vtkPolyDataMapper> jointMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    jointMapper->SetInputConnection(jointSource->GetOutputPort());

    /// Loop through each of the Fingers
    /// Then in Each Finger, loop through each of the Joints
    for (int f = 0; f < 5; f++)
    {
        fingerJoints  newJoints;                    /// Create a newJoints Object for each finger

           /// We get the defualt joints for the current finger from Active Hand
        switch (activeHand)
        {
            case rightHand: handModel->jointRightStartPos(newJoints,f); break;
            case leftHand:  handModel->jointLeftStartPos(newJoints,f); break;
        }


        for(int j = 0 ; j < 5; j++)                     /// We loop through each joint and create actor
        {
         global_Joints[activeHand][f][j] = vtkActor::New();
         global_Joints[activeHand][f][j]->SetMapper(jointMapper);

        global_Joints[activeHand][f][j]->GetProperty()->SetColor(fingerColourNormal);
        global_Joints[activeHand][f][j]->GetProperty()->SetOpacity(0.2);

                                                        /// We get the position from the newJoints [B][x,y,z]

        global_Joints[activeHand][f][j]->SetPosition(newJoints[j][0] * scale_,
                                                newJoints[j][1] * scale_,
                                                newJoints[j][2] * scale_ );

         renderer->AddActor(global_Joints[activeHand][f][j]);
        }
    }
}

void HandRenderer::drawBones(visibleHand activeHand, vtkRenderer *renderer)
{
    /// Loop through each of the Fingers
    /// Then in Each Finger, loop through each of the bones

    for (int f = 0; f < 5; f++)
    {

        for(int b = 0 ; b < 4; b++)                     /// We loop through each Bone and create actor
        {
            double* point1Pos = global_Joints[activeHand][f][b]->GetPosition();
            double* point2Pos = global_Joints[activeHand][f][b+1]->GetPosition();

            global_Bones[activeHand][f][b] =vtkLineSource::New();


             vtkSmartPointer<vtkPolyDataMapper>lineMapper =
                    vtkSmartPointer<vtkPolyDataMapper>::New();
            lineMapper->SetInputConnection(global_Bones[activeHand][f][b]->GetOutputPort());

            global_Bones[activeHand][f][b]->SetPoint1(point1Pos[0], point1Pos[1], point1Pos[2]);
            global_Bones[activeHand][f][b]->SetPoint2(point2Pos[0], point2Pos[1], point2Pos[2]);

            global_Bone_Actor[activeHand][f][b] = vtkActor::New();
            global_Bone_Actor[activeHand][f][b]->SetMapper(lineMapper);

            global_Bone_Actor[activeHand][f][b]->GetProperty()->SetColor(fingerColourNormal);

            global_Bone_Actor[activeHand][f][b]->GetProperty()->SetOpacity(0.5);
             global_Bone_Actor[activeHand][f][b]->GetProperty()->SetLineWidth(fingerSize);

         renderer->AddActor(global_Bone_Actor[activeHand][f][b]);
        }
    }
}

void HandRenderer::translateHand(visibleHand activeHand, Leap::Hand movingHand, bool outsideBounds)
{
    for (int f = 0; f < movingHand.fingers().count(); f++ )               /// For each finger, we get the joints
    {
        Leap::Finger finger = movingHand.fingers()[f];
        Leap::Bone mcp = finger.bone(Leap::Bone::TYPE_METACARPAL);

        Leap::Vector midpointMETA  = mcp.prevJoint() + mcp.prevJoint() / 2.0;

        /// We Get the location of the joint inside the hand
        double sensitivity = 0.01;
        double jointPosPoint[3] = {midpointMETA.x * sensitivity ,
                                   midpointMETA.y * sensitivity,
                                   midpointMETA.z * sensitivity
                                  };

        global_Joints[activeHand][f][0]->SetPosition(jointPosPoint);    ///joint Position.



        /// We interate through the bones and get the joints between each of them on each finger
        /// REMEMBER: We have 5 Joins for the 4 Bones for EACH of the 5 Fingers
        for (int b = 0; b < 4 ; b++)
        {
            Leap::Bone bone = finger.bone(static_cast<Leap::Bone::Type>(b));
            Leap::Vector bonePosition  = bone.nextJoint() + bone.nextJoint() / 2.0;

            double sensitivity = 0.01;
            double jointPosPoint[3] = { bonePosition.x * sensitivity ,
                                        bonePosition.y * sensitivity,
                                        bonePosition.z * sensitivity
                                      };

            global_Joints[activeHand][f][b+1]->SetPosition(jointPosPoint);    ///joint Position.
        }    /// for (int b = 0)
    }   ///  for (int f = 0; )

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////    Right Finger BONES  TRACKING  /////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////


    for (int f = 0; f < movingHand.fingers().count(); f++ )               /// For each finger, we get the joints
    {
        Leap::Finger finger = movingHand.fingers()[f];

        /// We interate through the bones and get the joints between each of them on each finger
        /// REMEMBER: We have 5 Joins for the 4 Bones for EACH of the 5 Fingers
        for (int b = 0; b < 4 ; b++)
        {

            double* point1Pos = global_Joints[activeHand][f][b]->GetPosition();
            double* point2Pos = global_Joints[activeHand][f][b+1]->GetPosition();


              global_Bones[activeHand][f][b]->SetPoint1(point1Pos);
              global_Bones[activeHand][f][b]->SetPoint2(point2Pos);

              /// IF OUT OF BOUNDS - Change ACTOR COLOUR
              if (outsideBounds)
                  global_Bone_Actor[activeHand][f][b]->GetProperty()->SetColor(fingerColourWarning);
              else
                  global_Bone_Actor[activeHand][f][b]->GetProperty()->SetColor(fingerColourNormal);

        }    /// for (int b = 0)
    }   ///  for (int f = 0; )
}

