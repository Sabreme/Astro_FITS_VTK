#ifndef XYHIGHLIGHTSELECTION_H
#define XYHIGHLIGHTSELECTION_H

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkIdTypeArray.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkRendererCollection.h>
#include <vtkProperty.h>
#include <vtkPlanes.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkAreaPicker.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkExtractGeometry.h>
#include <vtkDataSetMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkIdFilter.h>

#include <QVTKWidget.h>
#include <vtkfitsreader.h>
#include <vtkRendererCollection.h>

#define VTKISRBP_ORIENT 0
#define VTKISRBP_SELECT 1

#endif // XYHIGHLIGHTSELECTION_H

// Define interaction style
class HighlightInteractorStyle : public vtkInteractorStyleRubberBandPick
{
  public:
    static HighlightInteractorStyle* New();
    vtkTypeMacro(HighlightInteractorStyle,vtkInteractorStyleRubberBandPick);

    HighlightInteractorStyle() : vtkInteractorStyleRubberBandPick()
    {
      this->SelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
      this->SelectedActor = vtkSmartPointer<vtkActor>::New();
      this->SelectedActor->SetMapper(SelectedMapper);
    }

    virtual void OnLeftButtonUp()
    {
      // Forward events
      vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

      if(this->CurrentMode == VTKISRBP_SELECT)
        {
        vtkPlanes* frustum = static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker())->GetFrustum();

        vtkSmartPointer<vtkExtractGeometry> extractPolyDataGeometry =
          vtkSmartPointer<vtkExtractGeometry>::New();
        extractPolyDataGeometry->SetImplicitFunction(frustum);
#if VTK_MAJOR_VERSION <= 5
        extractPolyDataGeometry->SetInputConnection(
          this->PolyData->GetProducerPort());
#else
        extractPolyDataGeometry->SetInputData(this->PolyData);
#endif
        extractPolyDataGeometry->Update();

        vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter =
          vtkSmartPointer<vtkVertexGlyphFilter>::New();
        glyphFilter->SetInputConnection(extractPolyDataGeometry->GetOutputPort());
        glyphFilter->Update();

        vtkPolyData* selected = glyphFilter->GetOutput();
        std::cout << "Selected " << selected->GetNumberOfPoints() << " points." << std::endl;
        std::cout << "Selected " << selected->GetNumberOfCells() << " cells." << std::endl;
#if VTK_MAJOR_VERSION <= 5
        this->SelectedMapper->SetInputConnection(
          selected->GetProducerPort());
#else
        this->SelectedMapper->SetInputData(selected);
#endif
        this->SelectedMapper->ScalarVisibilityOff();

        vtkIdTypeArray* ids = vtkIdTypeArray::SafeDownCast(selected->GetPointData()->GetArray("OriginalIds"));
        for(vtkIdType i = 0; i < ids->GetNumberOfTuples(); i++)
          {
          std::cout << "Id " << i << " : " << ids->GetValue(i) << std::endl;
          }

        this->SelectedActor->GetProperty()->SetColor(1.0, 0.0, 0.0); //(R,G,B)
        this->SelectedActor->GetProperty()->SetPointSize(5);

        this->GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(SelectedActor);
        this->GetInteractor()->GetRenderWindow()->Render();
        this->HighlightProp(NULL);
        }
    }

    void SetPolyData(vtkSmartPointer<vtkPolyData> polyData) {this->PolyData = polyData;}
  private:
    vtkSmartPointer<vtkPolyData> PolyData;
    vtkSmartPointer<vtkActor> SelectedActor;
    vtkSmartPointer<vtkDataSetMapper> SelectedMapper;

};

vtkStandardNewMacro(HighlightInteractorStyle);

void XYVolumeSelection(QVTKWidget *qvtkWidget, vtkFitsReader *fitsSource)
{
    vtkSmartPointer<vtkIdFilter> idFilter =
      vtkSmartPointer<vtkIdFilter>::New();
    idFilter->SetInputConnection(fitsSource->GetOutputPort());
    idFilter->SetIdsArrayName("OriginalIds");
    idFilter->Update();

    // This is needed to convert the ouput of vtkIdFilter (vtkDataSet) back to vtkPolyData
    vtkSmartPointer<vtkDataSetSurfaceFilter> surfaceFilter =
      vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
    surfaceFilter->SetInputConnection(idFilter->GetOutputPort());
    surfaceFilter->Update();

    vtkPolyData* input = surfaceFilter->GetOutput();

    // Visualize
    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
//    vtkSmartPointer<vtkRenderWindow> renderWindow =
//      vtkSmartPointer<vtkRenderWindow>::New();
//    renderWindow->AddRenderer(renderer);

    renderer = qvtkWidget->GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

    vtkSmartPointer<vtkAreaPicker> areaPicker =
      vtkSmartPointer<vtkAreaPicker>::New();

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();

    renderWindowInteractor = qvtkWidget->GetInteractor();

    renderWindowInteractor->SetPicker(areaPicker);

    vtkSmartPointer<HighlightInteractorStyle> style =
      vtkSmartPointer<HighlightInteractorStyle>::New();
    style->SetPolyData(input);
    renderWindowInteractor->SetInteractorStyle( style );

    //renderWindowInteractor->Start();
}


