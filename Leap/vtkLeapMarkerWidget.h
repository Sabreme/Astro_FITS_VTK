/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkLeapMarkerWidget.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkLeapMarkerWidget - 2D widget for manipulating a marker prop
// .SECTION Description
// This class provides support for interactively manipulating the position,
// size, and apparent orientation of a prop that represents an orientation
// marker.  This class works by adding its internal renderer to an external
// "parent" renderer on a different layer.  The input orientation marker is
// rendered as an overlay on the parent renderer and, thus, appears superposed
// over all props in the parent's scene.  The camera view of the orientation
// the marker is made to match that of the parent's by means of an observer
// mechanism, giving the illusion that the orientation of the marker reflects
// that of the prop(s) in the parent's scene.
//
// The widget listens to left mouse button and mouse movement events. It will
// change the cursor shape based on its location. If the cursor is over the
// overlay renderer, it will change the cursor shape to a SIZEALL shape
// or to a resize corner shape (e.g., SIZENW) if the cursor is near a corner.
// If the left mouse button is pressed and held down while moving, the overlay
// renderer, and hence, the orientation marker, is resized or moved.  I the case
// of a resize operation, releasing the left mouse button causes the widget
// to enforce its renderer to be square.  The diagonally opposite corner to the
// one moved is repositioned such that all edges of the renderer have the same
// length: the minimum.
//
// To use this object, there are two key steps: 1) invoke SetInteractor() with
// the argument of the method a vtkRenderWindowInteractor, and 2) invoke
// SetOrientationMarker with an instance of vtkProp (see caveats below).
// Specifically, vtkAxesActor and vtkAnnotatedCubeActor are two classes
// designed to work with this class.  A composite orientation marker can be
// generated by adding instances of vtkAxesActor and vtkAnnotatedCubeActor to a
// vtkPropAssembly, which can then be set as the input orientation marker.
// The widget can be also be set up in a non-interactive fashion by setting 
// Ineractive to Off and sizing/placing the overlay renderer in its parent 
// renderer by calling the widget's SetViewport method.

// .SECTION Thanks
// This class was based originally on Paraview's vtkPVAxesWidget.

// .SECTION Caveats
// The input orientation marker prop should calculate its bounds as though they
// are symmetric about it's origin.  This must currently be done to correctly
// implement the camera synchronization between the ivar renderer and the
// renderer associated with the set interactor.  Importantly, the InteractorStyle
// associated with the interactor must be of the type vtkInteractorStyle*Camera.
// Where desirable, the parent renderer should be set by the SetDefaultRenderer
// method.  The parent renderer's number of layers is modified to 2 where
// required.

// .SECTION See Also
// vtkInteractorObserver vtkXYPlotWidget vtkScalarBarWidget vtkAxesActor
// vtkAnnotatedCubeActor

#ifndef __vtkLeapMarkerWidget_h
#define __vtkLeapMarkerWidget_h

#include "vtkInteractorObserver.h"

#include "vtkSmartPointer.h"
#include "vtkPlaneWidget.h"
#include "vtkPolyDataMapper.h"
#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkLeapMarkerWidget.h"
#include "vtkArrowSource.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkActor.h"
#include "vtkPlaneWidget.h"
#include "vtkCubeSource.h"
#include "vtkProbeFilter.h"
#include "vtkTransform.h"
#include "vtkGlyph3D.h"
#include "vtkProperty.h"
#include "vtkPointWidget.h"
#include "vtkSliderWidget.h"

class vtkActor2D;
class vtkPolyData;
class vtkProp;
class vtkLeapMarkerWidgetObserver;
class vtkRenderer;

class VTK_EXPORT vtkLeapMarkerWidget : public vtkInteractorObserver
{
public:
  static vtkLeapMarkerWidget* New();
  vtkTypeMacro(vtkLeapMarkerWidget, vtkInteractorObserver);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/get the Leap marker to be displayed in this widget.
  virtual void SetLeapMarker(vtkProp *prop);
  vtkGetObjectMacro(LeapMarker, vtkProp);

  // Description:
  // Enable/disable the widget. Default is 0 (disabled).
  virtual void SetEnabled(int);

  // Description:
  // Callback to keep the camera for the Leap marker up to date with the
  // camera in the parent renderer.
  void ExecuteCameraUpdateEvent(vtkObject *o, unsigned long event, void *calldata);

  // Description:
  // Set/get whether to allow this widget to be interactively moved/scaled.
  // Default is On.
  void SetInteractive(int state);
  vtkGetMacro(Interactive, int);
  vtkBooleanMacro(Interactive, int);

  // Description:
  // Set/get the color of the outline of this widget.  The outline is visible
  // when (in interactive mode) the cursor is over this widget.
  // Default is white (1,1,1).
  void SetOutlineColor(double r, double g, double b);
  double *GetOutlineColor();

  // Description:
  // Set/get the viewport to position/size this widget.
  // Default is bottom left corner (0,0,0.2,0.2).
  void SetViewport(double minX, double minY, double maxX, double maxY);
  double* GetViewport();

  // Description:
  // The tolerance representing the distance to the widget (in pixels)
  // in which the cursor is considered to be on the widget, or on a
  // widget feature (e.g., a corner point or edge).
  vtkSetClampMacro(Tolerance,int,1,10);
  vtkGetMacro(Tolerance,int);


  void GeneratActors();

  vtkActor        * leapDbgSphereActor;
  vtkActor        * leapDbgArrowActor;
  vtkArrowSource  * leapDbgArrow;
  vtkPlaneSource  * leapDbgPlane;
  vtkSphereSource * leapDbgSphere;
  vtkPlaneWidget * leapDbgPlaneWidget;
  vtkPointWidget * leapDbgPointWidget;
  vtkSliderWidget* leapDbgSliderWidget;

  double global_CameraPosition;

  int64_t global_ScaleFactorID ;  // Last  Frames Counted.
  double scaling_Max = 40;
  double scaling_Start = 10;
  double scaling_Min = 5;

protected:
  vtkLeapMarkerWidget();
  ~vtkLeapMarkerWidget();

  vtkRenderer *Renderer;
  vtkProp     *LeapMarker;
  vtkPolyData *Outline;
  vtkActor2D  *OutlineActor;

  ///////////////////////////////////////////////////////////
  /// \brief leapDbgSphereActor
  ///


  unsigned long StartEventObserverId;
  
  static void ProcessEvents(vtkObject *object, unsigned long event,
                            void *clientdata, void *calldata);

  // ProcessEvents() dispatches to these methods.
  void OnLeftButtonDown();
  void OnLeftButtonUp();
  void OnMouseMove();

  // observer to update the renderer's camera
  vtkLeapMarkerWidgetObserver *Observer;

  int Interactive;
  int Tolerance;
  int Moving;

  // used to compute relative movements
  int StartPosition[2];

//BTX - manage the state of the widget
  int State;
  enum WidgetState
  {
    Outside = 0,
    Inside,
    Translating,
    AdjustingP1,
    AdjustingP2,
    AdjustingP3,
    AdjustingP4
  };
//ETX


  // use to determine what state the mouse is over, edge1 p1, etc.
  // returns a state from the WidgetState enum above
  int ComputeStateBasedOnPosition(int X, int Y, int *pos1, int *pos2);

  // set the cursor to the correct shape based on State argument
  void SetCursor(int state);

  // adjust the viewport depending on state
  void MoveWidget(int X, int Y);
  void ResizeTopLeft(int X, int Y);
  void ResizeTopRight(int X, int Y);
  void ResizeBottomLeft(int X, int Y);
  void ResizeBottomRight(int X, int Y);

  void SquareRenderer();
  void UpdateOutline();


private:
  vtkLeapMarkerWidget(const vtkLeapMarkerWidget&);  // Not implemented
  void operator=(const vtkLeapMarkerWidget&);  // Not implemented
};

#endif
