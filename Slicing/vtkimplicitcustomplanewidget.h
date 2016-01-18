/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkImplicitCustomPlane.h,v $

  Copyright (c)
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME v



#ifndef VTKIMPLICITCUSTOMPLANEWIDGET_H
#define VTKIMPLICITCUSTOMPLANEWIDGET_H

#include "vtkPolyDataSourceWidget.h"
#include "vtkSystemIncludes.h"
//#include "vtkIntera"

class vtkActor;
class vtkPolyDataMapper;
class vtkCellPicker;
class vtkConeSource;
class vtkLineSource;
class vtkSphereSource;
class vtkTubeFilter;
class vtkPlane;
class vtkCutter;
class vtkProperty;
class vtkImageData;
class vtkOutlineFilter;
class vtkFeatureEdges;
class vtkPolyData;
class vtkTransform;

//class VTK_WIDGETS_EXPORT vtkImplicitCustomPlaneWidget : public vtkPolyDataSourceWidget
class vtkImplicitCustomPlaneWidget : public vtkPolyDataSourceWidget
{
public:
    static vtkImplicitCustomPlaneWidget *New();

    vtkTypeMacro(vtkImplicitCustomPlaneWidget,vtkPolyDataSourceWidget);
    void PrintSelf(ostream &os, vtkIndent indent);

    virtual void SetEnabled(int);
    virtual void PlaceWidget(double bounds[6]);
    void PlaceWidget()
    {this->Superclass::PlaceWidget();}
    void PlaceWidget(double xmin, double xmax, double ymin, double ymax,
                     double zmin, double zmax)
    {this->Superclass::PlaceWidget(xmin,xmax,ymin,ymax,zmin,zmax);}
    //

    // Description:
      // Get the origin of the plane.
      virtual void SetOrigin(double x, double y, double z);
      virtual void SetOrigin(double x[3]);
      double* GetOrigin();
      void GetOrigin(double xyz[3]);

      // Description:
      // Get the normal to the plane.
      void SetNormal(double x, double y, double z);
      void SetNormal(double x[3]);
      double* GetNormal();
      void GetNormal(double xyz[3]);

      // Description:
      // Force the plane widget to be aligned with one of the x-y-z axes.
      // If one axis is set on, the other two will be set off.
      // Remember that when the state changes, a ModifiedEvent is invoked.
      // This can be used to snap the plane to the axes if it is orginally
      // not aligned.
      void SetNormalToXAxis(int);
      vtkGetMacro(NormalToXAxis,int);
      vtkBooleanMacro(NormalToXAxis,int);
      void SetNormalToYAxis(int);
      vtkGetMacro(NormalToYAxis,int);
      vtkBooleanMacro(NormalToYAxis,int);
      void SetNormalToZAxis(int);
      vtkGetMacro(NormalToZAxis,int);
      vtkBooleanMacro(NormalToZAxis,int);

      // Description:
      // Turn on/off tubing of the wire outline of the plane. The tube thickens
      // the line by wrapping with a vtkTubeFilter.
      vtkSetMacro(Tubing,int);
      vtkGetMacro(Tubing,int);
      vtkBooleanMacro(Tubing,int);

      // Description:
      // Enable/disable the drawing of the plane. In some cases the plane
      // interferes with the object that it is operating on (i.e., the
      // plane interferes with the cut surface it produces producing
      // z-buffer artifacts.)
      void SetDrawPlane(int plane);
      vtkGetMacro(DrawPlane,int);
      vtkBooleanMacro(DrawPlane,int);

      // Description:
      // Turn on/off the ability to translate the bounding box by grabbing it
      // with the left mouse button.
      vtkSetMacro(OutlineTranslation,int);
      vtkGetMacro(OutlineTranslation,int);
      vtkBooleanMacro(OutlineTranslation,int);

      // Description:
      // Turn on/off the ability to move the widget outside of the input's bound
      vtkSetMacro(OutsideBounds,int);
      vtkGetMacro(OutsideBounds,int);
      vtkBooleanMacro(OutsideBounds,int);

      // Description:
      // Turn on/off the ability to scale with the mouse
      vtkSetMacro(ScaleEnabled,int);
      vtkGetMacro(ScaleEnabled,int);
      vtkBooleanMacro(ScaleEnabled,int);

      // Description:
      // Turn on/off the ability to translate the origin (sphere)
      // with the left mouse button.
      vtkSetMacro(OriginTranslation,int);
      vtkGetMacro(OriginTranslation,int);
      vtkBooleanMacro(OriginTranslation,int);

      // Description:
      // By default the arrow is 30% of the diagonal length. DiagonalRatio control
      // this ratio in the interval [0-2]
      vtkSetClampMacro(DiagonalRatio,double,0,2);
      vtkGetMacro(DiagonalRatio,double);

      // Description:
      // Grab the polydata that defines the plane. The polydata contains a single
      // polygon that is clipped by the bounding box.
      void GetPolyData(vtkPolyData *pd);

      // Description:
      // Satisfies superclass API.  This returns a pointer to the underlying
      // PolyData (which represents the plane).
      vtkPolyDataAlgorithm* GetPolyDataAlgorithm();

      // Description:
      // Get the implicit function for the plane. The user must provide the
      // instance of the class vtkPlane. Note that vtkPlane is a subclass of
      // vtkImplicitFunction, meaning that it can be used by a variety of filters
      // to perform clipping, cutting, and selection of data.
      void GetPlane(vtkPlane *plane);

      // Description:
      // Satisfies the superclass API.  This will change the state of the widget
      // to match changes that have been made to the underlying PolyDataSource
      void UpdatePlacement();

      // Description:
      // Control widget appearance
      virtual void SizeHandles();

      // Description:
      // Get the properties on the normal (line and cone).
      vtkGetObjectMacro(NormalProperty,vtkProperty);
      vtkGetObjectMacro(SelectedNormalProperty,vtkProperty);

      // Description:
      // Get the plane properties. The properties of the plane when selected
      // and unselected can be manipulated.
      vtkGetObjectMacro(PlaneProperty,vtkProperty);
      vtkGetObjectMacro(SelectedPlaneProperty,vtkProperty);

      // Description:
      // Get the property of the outline.
      vtkGetObjectMacro(OutlineProperty,vtkProperty);
      vtkGetObjectMacro(SelectedOutlineProperty,vtkProperty);

      // Description:
      // Get the property of the intersection edges. (This property also
      // applies to the edges when tubed.)
      vtkGetObjectMacro(EdgesProperty,vtkProperty);

      // Description:
      // Restablish Default Observers to the Current Widget
      void RestoreObservers();   

      // Description
      // Check if the current Position is a Cone Selection for Rotation
      void finger1Pressed(int X, int Y);

      // Description
      // Custom Rotation using Single Finger after ValidRotation Action
      void finger1Moving(int X, int Y, int X_last, int Y_last);

      void finger1Released();

      // Description
      // Check if the current Position is a Cone Selection for Rotation
      void finger2Pressed(int X, int Y);

      // Description
      // Custom Rotation using Single Finger after ValidRotation Action
      void finger2Moving(int X, int Y, int X_last, int Y_last);

      void finger2Released();

      void leapMovingNormal(int normalMoving);

      void leapMovingPlane(int planeMoving);




protected:
      vtkImplicitCustomPlaneWidget();
      ~vtkImplicitCustomPlaneWidget();

    //BTX - manage the state of the widget
      int State;
      enum WidgetState
      {
        Start=0,
        MovingPlane,
        MovingOutline,
        MovingOrigin,
        Scaling,
        Pushing,
        Rotating,
        Outside
      };
    //ETX

      //handles the events
      static void ProcessEvents(vtkObject* object, unsigned long event,
                                void* clientdata, void* calldata);

      // ProcessEvents() dispatches to these methods.
      void OnLeftButtonDown();
      void OnLeftButtonUp();
      void OnMiddleButtonDown();
      void OnMiddleButtonUp();
      void OnRightButtonDown();
      void OnRightButtonUp();
      void OnMouseMove();

      // Controlling ivars
      int NormalToXAxis;
      int NormalToYAxis;
      int NormalToZAxis;
      void UpdateRepresentation();

      // The actual plane which is being manipulated
      vtkPlane *Plane;

      // The bounding box is represented by a single voxel image data
      vtkImageData      *Box;
      vtkOutlineFilter  *Outline;
      vtkPolyDataMapper *OutlineMapper;
      vtkActor          *OutlineActor;
      void HighlightOutline(int highlight);
      int OutlineTranslation; //whether the outline can be moved
      int ScaleEnabled; //whether the widget can be scaled
      int OutsideBounds; //whether the widget can be moved outside input's bounds

      // The cut plane is produced with a vtkCutter
      vtkCutter         *Cutter;
      vtkPolyDataMapper *CutMapper;
      vtkActor          *CutActor;
      int               DrawPlane;
      virtual void HighlightPlane(int highlight);      

      // Optional tubes are represented by extracting boundary edges and tubing
      vtkFeatureEdges   *Edges;
      vtkTubeFilter     *EdgesTuber;
      vtkPolyDataMapper *EdgesMapper;
      vtkActor          *EdgesActor;
      int               Tubing; //control whether tubing is on

      // Control final length of the arrow:
      double DiagonalRatio;

      // The + normal cone
      vtkConeSource     *ConeSource;
      vtkPolyDataMapper *ConeMapper;
      vtkActor          *ConeActor;
      void HighlightNormal(int highlight);

      // The + normal line
      vtkLineSource     *LineSource;
      vtkPolyDataMapper *LineMapper;
      vtkActor          *LineActor;

      // The - normal cone
      vtkConeSource     *ConeSource2;
      vtkPolyDataMapper *ConeMapper2;
      vtkActor          *ConeActor2;

      // The - normal line
      vtkLineSource     *LineSource2;
      vtkPolyDataMapper *LineMapper2;
      vtkActor          *LineActor2;

      // The origin positioning handle
      vtkSphereSource   *Sphere;
      vtkPolyDataMapper *SphereMapper;
      vtkActor          *SphereActor;
      int OriginTranslation; //whether the origin (sphere) can be moved

      // Do the picking
      vtkCellPicker *Picker;

      // Transform the normal (used for rotation)
      vtkTransform *Transform;

      // Methods to manipulate the plane
      void ConstrainOrigin(double x[3]);
      void Rotate(int X, int Y, double *p1, double *p2, double *vpn);
      void TranslatePlane(double *p1, double *p2);
      void TranslateOutline(double *p1, double *p2);
      void TranslateOrigin(double *p1, double *p2);
      void Push(double *p1, double *p2);
      void Scale(double *p1, double *p2, int X, int Y);

      // Properties used to control the appearance of selected objects and
      // the manipulator in general.
      vtkProperty *NormalProperty;
      vtkProperty *SelectedNormalProperty;
      vtkProperty *PlaneProperty;
      vtkProperty *SelectedPlaneProperty;
      vtkProperty *OutlineProperty;
      vtkProperty *SelectedOutlineProperty;
      vtkProperty *EdgesProperty;
      void CreateDefaultProperties();

      void GeneratePlane();

    private:
      vtkImplicitCustomPlaneWidget(const vtkImplicitCustomPlaneWidget&);  //Not implemented
      void operator=(const vtkImplicitCustomPlaneWidget&);  //Not implemented
    };

    #endif
