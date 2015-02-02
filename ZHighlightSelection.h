#ifndef ZHIGHLIGHTSELECTION_H
#define ZHIGHLIGHTSELECTION_H

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
//#include <vtkIdType.h>

#include <QVTKWidget.h>
#include <vtkfitsreader.h>
#include <vtkRendererCollection.h>

//#include <XYHighlightSelection.h>

#define VTKISRBP_ORIENT 0
#define VTKISRBP_SELECT 1

#endif // ZHIGHLIGHTSELECTION_H

double minZ;
double maxZ;

double pminX;
double pmaxX;
double pminY;
double pmaxY;

bool IsValidSelection(int, int);

// Define interaction style
class HighlightInteractorStyleZ : public vtkInteractorStyleRubberBandPick
{
  public:
    static HighlightInteractorStyleZ* New();
    vtkTypeMacro(HighlightInteractorStyleZ,vtkInteractorStyleRubberBandPick);

    HighlightInteractorStyleZ() : vtkInteractorStyleRubberBandPick()
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
        frustum->Print(std::cout);

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

        pminX = bounds[0];
        pmaxX = bounds[1];
        pminY = bounds[2];
        pmaxY = bounds[3];


        minZ = bounds[4];
        maxZ = bounds[5];

//        std::cout << "Region (z1,z2) = ("
//                          << bounds[4] << "," << bounds[5] << ")" << std::endl;
//                          //<< bounds[1] << "," << bounds[3] << ")" << std::endl ;

//        std::cout   << "Old Region (y1, y2) = ("
//                    << GetMinYBound() << "," << GetMaxYBound() <<  ")" << std::endl;

//        std::cout   << "New Region (y1, y2) = ("
//                    << pminY << "," << pmaxY <<  ")" << std::endl;

        bool isValid = false;

        isValid = IsValidSelection(pminY, pmaxY);

        if (isValid)
        {

#if VTK_MAJOR_VERSION <= 5
            this->SelectedMapper->SetInputConnection(
                        selected->GetProducerPort());
#else
            this->SelectedMapper->SetInputData(selected);
#endif
            this->SelectedMapper->ScalarVisibilityOff();

            vtkIdTypeArray* ids = vtkIdTypeArray::SafeDownCast(selected->GetPointData()->GetArray("OriginalIds"));

            this->SelectedActor->GetProperty()->SetColor(0.0, 0.0, 1.0); //(R,G,B)
            this->SelectedActor->GetProperty()->SetPointSize(5);

            this->GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(SelectedActor);
            this->GetInteractor()->GetRenderWindow()->Render();
            this->HighlightProp(NULL);

            std::cout << "Length: " << selected->GetLength() << std::endl;
        }

        std::cout << "Region (z1,z2) = ("
                          << bounds[4] << "," << bounds[5] << ")" << std::endl;
                          //<< bounds[1] << "," << bounds[3] << ")" << std::endl ;

        std::cout   << "Old Region (y1, y2) = ("
                    << GetMinYBound() << "," << GetMaxYBound() <<  ")" << std::endl;

        std::cout   << "New Region (y1, y2) = ("
                    << pminY << "," << pmaxY <<  ")" << std::endl;
      }

    }

    void SetPolyData(vtkSmartPointer<vtkPolyData> polyData) {this->PolyData = polyData;}
  private:
    vtkSmartPointer<vtkPolyData> PolyData;
    vtkSmartPointer<vtkActor> SelectedActor;
    vtkSmartPointer<vtkDataSetMapper> SelectedMapper;

};

vtkStandardNewMacro(HighlightInteractorStyleZ);

void ZVolumeSelection(QVTKWidget *qvtkWidget, vtkFitsReader *fitsSource)
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

    vtkSmartPointer<HighlightInteractorStyleZ> style =
      vtkSmartPointer<HighlightInteractorStyleZ>::New();
    style->SetPolyData(input);
    renderWindowInteractor->SetInteractorStyle( style );

    //renderWindowInteractor->Start();
}

bool IsValidSelection(int minY, int maxY)
{
    bool valid = false;

   // int testY1 = GetMinYBound;

    if ((minY > GetMinYBound()) && (maxY < GetMaxYBound()))
    {
        std::cout << "Valid Selection"  << std::endl;

        valid =  true;
    }
    else
    {
        std::cout << "InValid Selection"  << std::endl;
       valid = false;
    }

    return valid;
}

double GetMinZBound()
{
    return minZ;
}

double GetMaxZBound()
{
    return maxZ;
}


