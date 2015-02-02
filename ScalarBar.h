#ifndef SCALARBAR_H
#define SCALARBAR_H

#include <cassert>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include "QVTKWidget.h"
#include "vtkScalarBarActor.h"
#include "vtkLookupTable.h"
#include "vtkPolyData.h"
#include "vtkfitsreader.h"
#include "vtkPolyDataMapper.h"
#include "vtkSmartPointer.h"
#include "vtkStructuredPoints.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindowInteractor.h"

#endif // SCALARBAR_H

void AddScalarBar(QVTKWidget *qvtkWidget, vtkFitsReader *fitsSource )
{
    assert (qvtkWidget != 0);

    // Create scalar data to associate with the vertices of the sphere
      int numPts = fitsSource->GetOutput()->GetNumberOfPoints();
      vtkSmartPointer<vtkFloatArray> scalars =
        vtkSmartPointer<vtkFloatArray>::New();
      scalars->SetNumberOfValues( numPts );
      for( int i = 0; i < numPts; ++i )
        {
        scalars->SetValue(i,static_cast<float>(i)/numPts);
        }
      vtkSmartPointer<vtkPolyData> poly =
        vtkSmartPointer<vtkPolyData>::New();
      poly->DeepCopy(fitsSource->GetOutput());
      poly->GetPointData()->SetScalars(scalars);

      vtkSmartPointer<vtkPolyDataMapper> mapper =
          vtkSmartPointer<vtkPolyDataMapper>::New();
      #if VTK_MAJOR_VERSION <= 5
        mapper->SetInput(poly);
      #else
        mapper->SetInputData(poly);
      #endif
        mapper->ScalarVisibilityOn();
        mapper->SetScalarModeToUsePointData();
        mapper->SetColorModeToMapScalars();

        vtkSmartPointer<vtkActor> actor =
          vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);

        vtkSmartPointer<vtkScalarBarActor> scalarBar =
          vtkSmartPointer<vtkScalarBarActor>::New();
        scalarBar->SetLookupTable(mapper->GetLookupTable());
        scalarBar->SetTitle("Point Spread");
        scalarBar->SetNumberOfLabels(4);

        // Create a lookup table to share between the mapper and the scalarbar
        vtkSmartPointer<vtkLookupTable> hueLut =
          vtkSmartPointer<vtkLookupTable>::New();
        hueLut->SetTableRange (0, 1);
        hueLut->SetHueRange (0, 1);
        hueLut->SetSaturationRange (1, 1);
        hueLut->SetValueRange (1, 1);
        hueLut->Build();

        mapper->SetLookupTable( hueLut );
        scalarBar->SetLookupTable( hueLut );
        scalarBar->SetLookupTable(mapper->GetLookupTable());
        scalarBar->SetMaximumHeightInPixels(120);
        scalarBar->SetMaximumWidthInPixels(20);

        scalarBar->SetPosition(0.9, 0.1);

        // Create a renderer and render window
        vtkSmartPointer<vtkRenderer> renderer =
          vtkSmartPointer<vtkRenderer>::New();

        renderer = qvtkWidget->GetInteractor()->GetRenderWindow()->
                GetRenderers()->GetFirstRenderer();


        renderer->AddActor2D(scalarBar);
}
