/******************************************************************************\
* Copyright (C) 2012-2013 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#include <iostream>
#include <Leap/Sample.h>
#include <vtkCamera.h>
#include <vtkTransform.h>
#include "src/mainwindow.h"

#include  "vtkRenderWindow.h"
#include "vtkRendererCollection.h"


SampleListener::SampleListener(vtkCamera* camera)  {
     camera_ = camera;
    // mw_ = mw;

}

void SampleListener::onInit(const Controller& controller) {
  std::cout << "Initialized" << std::endl;
}

void SampleListener::onConnect(const Controller& controller) {
  std::cout << "Connected" << std::endl;
  controller.enableGesture(Gesture::TYPE_CIRCLE);
  controller.enableGesture(Gesture::TYPE_KEY_TAP);
  controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Gesture::TYPE_SWIPE);
}

void SampleListener::onDisconnect(const Controller& controller) {
  //Note: not dispatched when running in a debugger.
  std::cout << "Disconnected" << std::endl;
}

void SampleListener::onExit(const Controller& controller) {
  std::cout << "Exited" << std::endl;
}

void SampleListener::onFrame(const Controller& controller) {
  // Get the most recent frame and report some basic information
//  const Frame frame = controller.frame();
//  //if frame.timestamp()
//  std::cout << "Frame id: " << frame.id()
//            << ", timestamp: " << frame.timestamp() << std::endl;
////            << ", hands: " << frame.hands().count()
////            << ", fingers: " << frame.fingers().count()
////            << ", tools: " << frame.tools().count()
////            << ", gestures: " << frame.gestures().count() << std::endl;

//  //if (!frame.hands().empty() && frame.) {
//    // Get the first hand
//    const Hand hand = frame.hands()[0];

////    if (hand.fingers().count() > 3)
////    {
//        double  * focalPoint;
//        double  * position ;
//        double  dist ;

//        focalPoint = camera_->GetFocalPoint();
//        position = camera_->GetPosition();
//        dist =  sqrt(pow((position[0]-focalPoint[0]),2) +
//                pow((position[1]-focalPoint[1]),2) +
//                pow((position[2]-focalPoint[2]),2));

//        Vector newposition = hand.translation(controller.frame(1));
//        float current_angle = 0.0;

//        current_angle = hand.rotationAngle(controller.frame(1));
//        //std::cout << "Hand Angle: " << current_angle << std::endl;

//        newposition = 0.1 * newposition;

//        vtkTransform* trans = camera_->GetModelViewTransformObject();

//        trans->Identity();

//        trans->Translate(newposition.x, newposition.y, newposition.z);

//        camera_->ApplyTransform(trans);

//        camera_->SetPosition(focalPoint[0], focalPoint[1], focalPoint[2] + dist);
////    }

//    // Check if the hand has any fingers
//    const FingerList fingers = hand.fingers();
//    if (!fingers.empty()) {
//      // Calculate the hand's average finger tip position
//      Vector avgPos;
//      for (int i = 0; i < fingers.count(); ++i) {
//        avgPos += fingers[i].tipPosition();
//      }
//      avgPos /= (float)fingers.count();
//    //  std::cout << "Hand has " << fingers.count()
//   //             << " fingers, average finger tip position" << avgPos << std::endl;
//    }

//    // Get the hand's sphere radius and palm position
//  //  std::cout << "Hand sphere radius: " << hand.sphereRadius()
// //             << " mm, palm position: " << hand.palmPosition() << std::endl;

//    // Get the hand's normal vector and direction
//    const Vector normal = hand.palmNormal();
//    const Vector direction = hand.direction();

//    // Calculate the hand's pitch, roll, and yaw angles
////    std::cout << "Hand pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
////              << "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
////              << "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << std::endl;
////  }

//  // Get gestures
//  const GestureList gestures = frame.gestures();
//  for (int g = 0; g < gestures.count(); ++g) {
//    Gesture gesture = gestures[g];

//    switch (gesture.type()) {
//      case Gesture::TYPE_CIRCLE:
//      {
//        CircleGesture circle = gesture;
//        std::string clockwiseness;

//        if (circle.pointable().direction().angleTo(circle.normal()) <= PI/4) {
//          clockwiseness = "clockwise";

////          Vector newposition = frame.hands()[0].translation(controller.frame(1));

////          double * position ;
////          double  dist ;
////          double * focalPoint;

////          position = camera_->GetPosition();
////          focalPoint = camera_->GetFocalPoint();
////          dist =  sqrt(pow((position[0]-focalPoint[0]),2) +
////                       pow((position[1]-focalPoint[1]),2) +
////                       pow((position[2]-focalPoint[2]),2));

////          camera_->SetPosition(focalPoint[0], focalPoint[1], focalPoint[2] + dist + 1);

////          camera_->Modified();

//          //newposition = 0.0000001 * newposition;

//          //vtkTransform* trans = camera_->GetModelViewTransformObject();
//          //camera_->SetPosition(focalPoint[0], focalPoint[1], focalPoint[2] + dist);

//          //trans.Identity();
//          //trans->Translate(newposition.x, newposition.y, newposition.z);

//         // camera_->ApplyTransform(trans);
//          //camera_->Render();


//        } else {
//          clockwiseness = "counterclockwise";

////          Vector newposition = frame.hands()[0].translation(controller.frame(1));

////          double * position ;
////          double  dist ;
////          double * focalPoint;

////          position = camera_->GetPosition();
////          focalPoint = camera_->GetFocalPoint();
////          dist =  sqrt(pow((position[0]-focalPoint[0]),2) +
////                       pow((position[1]-focalPoint[1]),2) +
////                       pow((position[2]-focalPoint[2]),2));

////          camera_->SetPosition(focalPoint[0], focalPoint[1], focalPoint[2] + dist - 1);

////          camera_->Modified();

//        }

//        // Calculate angle swept since last frame
//        float sweptAngle = 0;
//        if (circle.state() != Gesture::STATE_START) {
//          CircleGesture previousUpdate = CircleGesture(controller.frame(1).gesture(circle.id()));
//          sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * PI;
//        }
//        std::cout << "Circle id: " << gesture.id()
//                  << ", state: " << gesture.state()
//                  << ", progress: " << circle.progress()
//                 << ", radius: " << circle.radius()
//                  << ", angle " << sweptAngle * RAD_TO_DEG
//                  <<  ", " << clockwiseness << std::endl;
//        break;
//      }
//      case Gesture::TYPE_SWIPE:
//      {
//        SwipeGesture swipe = gesture;
//        std::cout << "Swipe id: " << gesture.id()
//          << ", state: " << gesture.state()
//          << ", direction: " << swipe.direction()
//          << ", speed: " << swipe.speed() << std::endl;
//        break;
//      }
//      case Gesture::TYPE_KEY_TAP:
//      {
//        KeyTapGesture tap = gesture;
//        std::cout << "Key Tap id: " << gesture.id()
//          << ", state: " << gesture.state()
//          << ", position: " << tap.position()
//          << ", direction: " << tap.direction()<< std::endl;
//        break;
//      }
//      case Gesture::TYPE_SCREEN_TAP:
//      {
//        ScreenTapGesture screentap = gesture;
//        std::cout << "Screen Tap id: " << gesture.id()
//        << ", state: " << gesture.state()
//        << ", position: " << screentap.position()
//        << ", direction: " << screentap.direction()<< std::endl;
//        break;
//      }
//      default:
//        std::cout << "Unknown gesture type." << std::endl;
//        break;
//    }
//  }

//  if (!frame.hands().empty() || !gestures.empty()) {
//    std::cout << std::endl;
//  }
}

void SampleListener::onFocusGained(const Controller& controller) {
  std::cout << "Focus Gained" << std::endl;
}

void SampleListener::onFocusLost(const Controller& controller) {
  std::cout << "Focus Lost" << std::endl;
}