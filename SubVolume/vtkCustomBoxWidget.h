/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkCustomBoxWidget.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkCustomBoxWidget - orthogonal hexahedron 3D widget
// .SECTION Description
// This 3D widget defines a region of interest that is represented by an
// arbitrarily oriented hexahedron with interior face angles of 90 degrees
// (orthogonal faces). The object creates 7 handles that can be moused on and
// manipulated. The first six correspond to the six faces, the seventh is in
// the center of the hexahedron. In addition, a bounding box outline is shown,
// the "faces" of which can be selected for object rotation or scaling. A
// nice feature of the object is that the vtkCustomBoxWidget, like any 3D widget,
// will work with the current interactor style. That is, if vtkCustomBoxWidget does
// not handle an event, then all other registered observers (including the
// interactor style) have an opportunity to process the event. Otherwise, the
// vtkCustomBoxWidget will terminate the processing of the event that it handles.
//
// To use this object, just invoke SetInteractor() with the argument of the
// method a vtkRenderWindowInteractor.  You may also wish to invoke
// "PlaceWidget()" to initially position the widget. The interactor will act
// normally until the "i" key (for "interactor") is pressed, at which point the
// vtkCustomBoxWidget will appear. (See superclass documentation for information
// about changing this behavior.) By grabbing the six face handles (use the
// left mouse button), faces can be moved. By grabbing the center handle
// (with the left mouse button), the entire hexahedron can be
// translated. (Translation can also be employed by using the
// "shift-left-mouse-button" combination inside of the widget.) Scaling is
// achieved by using the right mouse button "up" the render window (makes the
// widget bigger) or "down" the render window (makes the widget smaller). To
// rotate vtkCustomBoxWidget, pick a face (but not a face handle) and move the left
// mouse. (Note: the mouse button must be held down during manipulation.)
// Events that occur outside of the widget (i.e., no part of the widget is
// picked) are propagated to any other registered obsevers (such as the
// interaction style).  Turn off the widget by pressing the "i" key again.
// (See the superclass documentation on key press activiation.)
//
// The vtkCustomBoxWidget is very flexible. It can be used to select, cut, clip, or
// perform any other operation that depends on an implicit function (use the
// GetPlanes() method); or it can be used to transform objects using a linear
// transformation (use the GetTransform() method). Typical usage of the
// widget is to make use of the StartInteractionEvent, InteractionEvent, and
// EndInteractionEvent events. The InteractionEvent is called on mouse
// motion; the other two events are called on button down and button up
// (either left or right button).
//
// Some additional features of this class include the ability to control the
// rendered properties of the widget. You can set the properties of the
// selected and unselected representations of the parts of the widget. For
// example, you can set the property for the handles, faces, and outline in
// their normal and selected states.

// .SECTION Caveats
// Note that handles can be picked even when they are "behind" other actors.
// This is an intended feature and not a bug.
//
// The box widget can be oriented by specifying a transformation matrix.
// This transformation is applied to the initial bounding box as defined by
// the PlaceWidget() method. DO NOT ASSUME that the transformation is applied
// to a unit box centered at the origin; this is wrong!

// .SECTION See Also
// vtk3DWidget vtkPointWidget vtkLineWidget vtkPlaneWidget
// vtkImplicitPlaneWidget vtkImagePlaneWidget

#ifndef __vtkCustomBoxWidget_h
#define __vtkCustomBoxWidget_h

//#include "vtk3DWidget.h"
#include "vtkBoxWidget.h"

///class VTK_WIDGETS_EXPORT vtkCustomBoxWidget : public vtkBoxWidget
class vtkCustomBoxWidget : public vtkBoxWidget
{
public:
  // Description:
  // Instantiate the object.
  static vtkCustomBoxWidget *New();

  vtkTypeMacro(vtkCustomBoxWidget,vtk3DWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void OnMiddleButtonDown();
  virtual void OnMiddleButtonUp();
  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();

protected:
  vtkCustomBoxWidget();
  ~vtkCustomBoxWidget();
};



#endif
