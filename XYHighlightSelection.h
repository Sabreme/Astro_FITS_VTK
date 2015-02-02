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
#include <vtkPlane.h>
#include <vtkCamera.h>
#include <vtkAbstractTransform.h>
#include <vtkRendererCollection.h>

#define VTKISRBP_ORIENT 0
#define VTKISRBP_SELECT 1

#endif // XYHIGHLIGHTSELECTION_H

// Define interaction style

double minX;
double maxX;
double minY;
double maxY;


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
      // Forward eventsp
      vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

      if(this->CurrentMode == VTKISRBP_SELECT)
        {
        vtkPlanes* frustum = static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker())->GetFrustum();
        //vtkPlanes* region = static_cast<vtkHardwareSelector*>(this->GetInteractor()->GetPicker())->
        frustum->Print(std::cout);
        //frustum->Transform


        //form->Print(std::cout);


        //frustum->FunctionValue(19,43,22,46,0,0);
        vtkPoints* points = static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker())->GetClipPoints();


        points->Print(std::cout);

        //frustum->Print(std::cout);

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

        double bounds[6];

        selected->GetBounds(bounds);

        minX = bounds[0];
        maxX = bounds[1];
        minY = bounds[2];
        maxY = bounds[3];

        //selected->


        std::cout << "Region (x1,y1,x2,y2) = ("
                          << bounds[0] << "," << bounds[2] << ","
                          << bounds[1] << "," << bounds[3] << ")" << std::endl ;

 //       double cellBounds[6];

//        selected->GetCellBounds();

//        std::cout << "CellRegion (x1,y1,x2,y2) = ("
//                          << cellBounds[0] << "," << cellBounds[2] << ","
//                          << cellBounds[1] << "," << cellBounds[3] << ")" << std::endl ;

        //std::cout << "Selected " << selected->
        //std::cout << "Selected " << selected->Get
        //std::cout << "Selected " << selected->getNum << "Vertices" << std::endl;
        //vtkIdType

#if VTK_MAJOR_VERSION <= 5
        this->SelectedMapper->SetInputConnection(
          selected->GetProducerPort());
#else
        this->SelectedMapper->SetInputData(selected);
#endif
        this->SelectedMapper->ScalarVisibilityOff();

        vtkIdTypeArray* ids = vtkIdTypeArray::SafeDownCast(selected->GetPointData()->GetArray("OriginalIds"));

        this->SelectedActor->GetProperty()->SetColor(1.0, 0.0, 0.0); //(R,G,B)
        this->SelectedActor->GetProperty()->SetPointSize(5);

        this->GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(SelectedActor);
        this->GetInteractor()->GetRenderWindow()->Render();
        this->HighlightProp(NULL);

        std::cout << "Length: " << selected->GetLength() << std::endl;
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

    vtkSmartPointer<vtkCamera> camera =
            vtkSmartPointer<vtkCamera>::New();

    renderer = qvtkWidget->GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

    camera = renderer->GetActiveCamera();

    camera->Print(std::cout);
    camera->ParallelProjectionOn();

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


double GetMinXBound()
{
    return minX;
}

double GetMaxXBound()
{
    return maxX;
}

double GetMinYBound()
{
    return minY;
}

double GetMaxYBound()
{
    return maxY;
}

//std::array<double,4> GetXYBounds()
//{
//    return (minX, maxX, minY,maxY);
//}




