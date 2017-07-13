#include "sliceDialog.h"
#include "ui_sliceDialog.h"

#include "vtkAxesActor.h"
#include "vtkOrientationMarkerWidget.h"
#include "vtkRenderWindowInteractor.h"
#include "QVTKInteractor.h"

#include "vtkOutlineFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkMarchingCubes.h"
#include "vtkImageDataGeometryFilter.h"
#include "vtkDecimatePro.h"
#include "vtkProperty.h"
#include "vtkImageResample.h"
#include "vtkFixedPointVolumeRayCastMapper.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkImageData.h"
#include "vtkVolumeProperty.h"
#include "vtkInteractorStyleSwitch.h"
#include "vtkTextProperty.h"

#include "vtkCellPicker.h"
#include "vtkImagePlaneWidget.h"
#include "vtkProperty.h"
#include "vtkVolume.h"
#include "vtkAlgorithm.h"
#include "vtkImageData.h"
#include "vtkRendererCollection.h"

#include "vtkCommand.h"
#include "vtkResliceImageViewer.h"
#include "vtkResliceCursorLineRepresentation.h"
#include "vtkResliceCursorThickLineRepresentation.h"
#include "vtkResliceCursorWidget.h"
#include "vtkResliceCursorActor.h"
#include "vtkResliceCursorPolyDataAlgorithm.h"
#include "vtkResliceCursor.h"
#include "vtkPlaneSource.h"
#include "vtkPlane.h"


#include "vtkImageMapper3D.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkBoundedPlanePointPlacer.h"


SliceDialog::SliceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SliceDialog)
{
    ui->setupUi(this);    
    //colorDialog = QColorDialog::;
}

SliceDialog::~SliceDialog()
{  
    delete ui;
}

void SliceDialog::UpdateQVTKWidget(vtkResliceImageViewer * riw, int axis)
{

    double reductionFactor = 1.0;

    vtkResliceCursorWidget * rcw = riw->GetResliceCursorWidget();

    this->riw[0] = vtkSmartPointer<vtkResliceImageViewer>::New();



    originalTable = vtkSmartPointer<vtkScalarsToColors> ::New();
    titleActor = vtkSmartPointer<vtkTextActor>::New();



//    /////////////////////////////////////////////////
//    /// \brief NOTE: You must setRenderWindow to ResliceViewer before any render manipulation
//    ////

     this->ui->qvtkWidgetRight->SetRenderWindow(this->riw[0]->GetRenderWindow());

    this->riw[0]->SetupInteractor(this->ui->qvtkWidgetRight->GetInteractor());

    this->riw[0]->SetInputData(riw->GetInput());

    this->riw[0]->SetSliceOrientation(riw->GetSliceOrientation());

    this->riw[0]->SetResliceCursor(riw->GetResliceCursor());

    //riw->GetInput()->Print(std::cout);

//    riw->Print(std::cout);

//    std::cout << "riw[0] = " << this->riw[0]->GetSlice() << " riw = " << riw->GetSlice() << endl;

    this->riw[0]->SetSlice(riw->GetSlice());

//    std::cout << "riw[0] = " << this->riw[0]->GetSlice() << " riw = " << riw->GetSlice() << endl;

//    std::cout << "riw[0] min,max = "<< this->riw[0]->GetSliceMin() << ","
//                                    << this->riw[0]->GetSliceMax() << endl;

//    std::cout << "riw min,max = "<< riw->GetSliceMin() << ","
//                                    << riw->GetSliceMax() << endl;


    this->riw[0]->SetResliceMode(riw->GetResliceMode());

//    this->riw[0]->GetPointPlacer()->SetObliquePlane(riw->GetPointPlacer()->GetObliquePlane());

 //   this->riw[0]->GetImageActor()->SetMapper(riw->GetImageActor()->GetMapper());


    //this->riw[0]->SetResliceCursor(riw->GetResliceCursor());

//    this->riw[0]->GetResliceCursorWidget()->SetRepresentation(
//                rcw->GetResliceCursorRepresentation());


    //oldActor->get = riw->GetImageActor();

//    oldPlane->se

//    this->riw[0]->GetImageActor()->Set

//    std::cout << "_____________________________________" << endl;

//   std::cout << "riw[0] = " << this->riw[0]->GetSlice() << " riw = " << riw->GetSlice() << endl;

//    this->riw[0]->Print(std::cout);

    //this->riw[0]->GetImageActor()->


    //this->riw[0]->SetResliceMode(riw->GetResliceMode());
    //this->riw[0]->Set


    double color [3] = {0,0,0};

    color[axis] = 1;

    color[0] /= 3.0;
    color[1] /= 3.0;
    color[2] /= 3.0;

    this->riw[0]->GetRenderer()->SetBackground(color);


    vtkSmartPointer<vtkRenderWindow> renderWindow =
            this->ui->qvtkWidgetRight->GetRenderWindow();

    vtkSmartPointer<vtkRenderer> renderer =

            renderWindow->GetRenderers()->GetFirstRenderer();

    /////////////////////////////////
    /// Insert Axes Labels to the Image
    ///
    vtkSmartPointer<vtkCubeAxesActor> imageAxesActor =
            vtkSmartPointer<vtkCubeAxesActor>::New();

    imageAxesActor->SetBounds(this->riw[0]->GetImageActor()->GetBounds());
    imageAxesActor->SetCamera(renderer->GetActiveCamera());

    renderer->AddActor(imageAxesActor);

    renderer->ResetCamera();

    /// Disable User Interaction with widget
    this->riw[0]->GetInteractor()->RemoveAllObservers();

    //this->riw[0]->GetInteractor()->AddObserver('LeftButtonPressEvent',this,)

    originalTable = this->riw[0]->GetLookupTable();

    connect(this->ui->horizontalSlider_Sat, SIGNAL(valueChanged(int)), this, SLOT(on_SaturationChanged(int)));
    connect(this->ui->horizontalSlider_Hue, SIGNAL(valueChanged(int)), this, SLOT(on_HueChanged(int)));
    connect(this->ui->horizontalSlider_Val, SIGNAL(valueChanged(int)), this, SLOT(on_TableValueChanged(int)));

    //renderer->SetBackground(0.2,0.2,0.2);
}

//void SliceDialog::UpdateQVTKWidget(vtkImagePlaneWidget * ipw, int axis)
//{

//    double reductionFactor = 1.0;

//   // vtkResliceCursorWidget * rcw = ipw->GetResliceCursorWidget();

//    //this->riw[0] = vtkSmartPointer<vtkResliceImageViewer>::New();

//    this->planeWidget[0] = vtkSmartPointer<vtkImagePlaneWidget>::New();

////    vtkSmartPointer<vtkImageActor> image =
////            vtkSmartPointer<vtkImageActor>::New();

//    ipw->Print(std::cout);

//   // image->SetInput(ipw->GetResliceOutput());

//    /////////////////////////////////////////////////
//    /// \brief NOTE: You must setRenderWindow to ResliceViewer before any render manipulation
//    ////

//    //this->ui->qvtkWidgetRight->SetRenderWindow(this->riw[0]->GetRenderWindow());

//    this->planeWidget[0]->SetInteractor(this->ui->qvtkWidgetRight->GetInteractor());

//    this->planeWidget[0]->SetInput(ipw->GetInput());

//    this->planeWidget[0]->SetPlaneOrientation(ipw->GetPlaneOrientation());

//    this->planeWidget[0]->SetSliceIndex(ipw->GetSliceIndex());

//    double origin [3] = {0,1,0};

//    this->planeWidget[0]->SetOrigin(origin);

//    this->planeWidget[0]->UpdatePlacement();

//    double color[3] = {0, 0, 0};
//    color[0] = 1;
//    planeWidget[0]->GetPlaneProperty()->SetColor(color);



//    //this->planeWidget[0]->SetDefaultRenderer(this->ui->qvtkWidgetRight->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

//    this->planeWidget[0]->On();

////    //riw->GetInput()->Print(std::cout);

////    ipw->Print(std::cout);

//////    std::cout << "riw[0] = " << this->riw[0]->GetSlice() << " riw = " << riw->GetSlice() << endl;

////    this->riw[0]->SetSlice(riw->GetSlice());

////    std::cout << "riw[0] = " << this->riw[0]->GetSlice() << " riw = " << riw->GetSlice() << endl;

////    std::cout << "riw[0] min,max = "<< this->riw[0]->GetSliceMin() << ","
////                                    << this->riw[0]->GetSliceMax() << endl;

////    std::cout << "riw min,max = "<< riw->GetSliceMin() << ","
////                                    << riw->GetSliceMax() << endl;


////    this->riw[0]->SetResliceMode(ipw->GetResliceMode());

////    this->riw[0]->GetPointPlacer()->SetObliquePlane(ipw->GetPointPlacer()->GetObliquePlane());


//    //this->riw[0]->SetResliceCursor(riw->GetResliceCursor());

////    this->riw[0]->GetResliceCursorWidget()->SetRepresentation(
////                rcw->GetResliceCursorRepresentation());


//    //oldActor->get = riw->GetImageActor();

////    oldPlane->se

////    this->riw[0]->GetImageActor()->Set

////    std::cout << "_____________________________________" << endl;

////    this->riw[0]->Print(std::cout);

//    //this->riw[0]->GetImageActor()->


//    //this->riw[0]->SetResliceMode(riw->GetResliceMode());
//    //this->riw[0]->Set


////    double color [3] = {0,0,0};

////    color[axis] = 1;

////    color[0] /= 3.0;
////    color[1] /= 3.0;
////    color[2] /= 3.0;

////    this->riw[0]->GetRenderer()->SetBackground(color);


////    vtkSmartPointer<vtkRenderWindow> renderWindow =
////            this->ui->qvtkWidgetRight->GetRenderWindow();

////    vtkSmartPointer<vtkRenderer> renderer =

////            renderWindow->GetRenderers()->GetFirstRenderer();

////    /////////////////////////////////
////    /// Insert Axes Labels to the Image
////    ///
////    vtkSmartPointer<vtkCubeAxesActor> imageAxesActor =
////            vtkSmartPointer<vtkCubeAxesActor>::New();

////    imageAxesActor->SetBounds(this->riw[0]->GetImageActor()->GetBounds());
////    imageAxesActor->SetCamera(renderer->GetActiveCamera());

////    renderer->AddActor(imageAxesActor);

////    renderer->AddActor(image);

////    renderer->ResetCamera();

////    /// Disable User Interaction with widget
////    this->riw[0]->GetInteractor()->RemoveAllObservers();

////    //this->riw[0]->GetInteractor()->AddObserver('LeftButtonPressEvent',this,)

////    originalTable = this->riw[0]->GetLookupTable();

//    connect(this->ui->horizontalSlider_Sat, SIGNAL(valueChanged(int)), this, SLOT(on_SaturationChanged(int)));
//    connect(this->ui->horizontalSlider_Hue, SIGNAL(valueChanged(int)), this, SLOT(on_HueChanged(int)));
//    connect(this->ui->horizontalSlider_Val, SIGNAL(valueChanged(int)), this, SLOT(on_TableValueChanged(int)));

//    //renderer->SetBackground(0.2,0.2,0.2);
//}

void SliceDialog::Render()
{
//  for (int i = 0; i < 3; i++)
//    {
//    riw[i]->Render();
//    }
  this->ui->qvtkWidgetRight->GetRenderWindow()->Render();
}



void SliceDialog::AlignCamera()
{

}

void SliceDialog::CaptureImage()
{

}

void SliceDialog::SetInterpolation()
{

}

void SliceDialog::SetSlice(int slice)
{

}

void SliceDialog::on_closeButton_clicked()
{
//    QString representation = QString("[Saturation(%1,%2)] \t[Hue(%3,%4)] \t[Value(%5,%6)] \n")
//                .arg(customLookup->GetSaturationRange()[0])
//                .arg(customLookup->GetSaturationRange()[1])
////                .arg(customLookup->GetHueRange()[0])
////                .arg(customLookup->GetHueRange()[1])
////                .arg(customLookup->GetValueRange()[0])
////                .arg(customLookup->GetValueRange()[1])
//                ;

//        this->ui->plainTextEdit_Output->insertPlainText(representation);
//        this->ui->plainTextEdit_Output->ensureCursorVisible();
    this->close();
    //this->ui->qvtkWidgetRight->GetRenderWindow()->Delete();
}

void SliceDialog::on_button_Colour_clicked()
{
}


void SliceDialog::loadColorBoxes()
{
    const QStringList colorNames = QColor::colorNames();

    QPalette palette[3] ;

    // Do this for each colourBox Index we installed
    for (int i = 0; i < 3; i++)
    {
        palette[i] = colourBox[i]->palette();                       // Holds the widgets palette
        palette[i].setColor(QPalette::Highlight, Qt::transparent);  //construction of teh palette
        colourBox[i]->setPalette(palette[i]);                       //Set the comboBox palette

        int size = colourBox[i]->style()->pixelMetric(QStyle::PM_SmallIconSize);
        QPixmap pixmap(size,size);

        int index = 0;
        foreach(const QString &colorName, colorNames)
        {
            const QColor color(colorName);
            colourBox[i]->addItem("", color);                // Adding ComboItems
            //this->ui->comboColorBox1->addItem(colorName,color);
            pixmap.fill(color);
            colourBox[i]->setItemData(index, pixmap, Qt::DecorationRole); //Setting color Palettes
            //colourBox[i]->model()->setData(colourBox[i]->model()->index(0,0), QSize(100,100), Qt::SizeHintRole);


            //const QModelIndex idx = this->ui->comboColorBox1->model()->index(index++, 0);
            //this->ui->comboColorBox1->model()->setData(idx,color,Qt::BackgroundColorRole);
            index++;

        }

        colourBox[i]->view()->setMinimumWidth(10);
        colourBox[i]->setMaximumWidth(40);
        colourBox[i]->setMinimumWidth(40);
        colourBox[i]->setStyleSheet("QComboBox { combobox-popup: 0; }");
    }
}

//void SliceDialog::updateImage(vtkImagePlaneWidget planeWidget, vtkResliceCursorLineRepresentation reSliceCursLnRep )
void SliceDialog::updateImage()
{
    vtkResliceCursorLineRepresentation *rep =
            vtkResliceCursorLineRepresentation::SafeDownCast(
                riw[0]->GetResliceCursorWidget()->GetRepresentation());
    riw[0]->SetResliceCursor(riw[0]->GetResliceCursor());
}

void SliceDialog::on_colorButton_clicked()
{

}

void SliceDialog::on_colorButton1_clicked()
{
    QPalette palette = this->ui->lineEdit_Color1->palette();    

    QColor selected  = QColorDialog::getColor(palette.color(this->ui->lineEdit_Color1->backgroundRole()), this);                       

    palette.setColor(this->ui->lineEdit_Color1->backgroundRole(),selected);
    this->ui->lineEdit_Color1->setPalette(palette);
    this->ui->lineEdit_Color1->setAutoFillBackground(true);

    //this->ui->colorButton1->setStyleSheet(QString("{ background-color: %1 }").arg(selected));

    color1 = selected;       

    vtkSmartPointer<vtkImageProperty> prop =
            vtkSmartPointer<vtkImageProperty>::New();

    prop = this->riw[0]->GetImageActor()->GetProperty();

    prop->BackingOn();


    prop->SetBackingColor(double(color1.red())/256,
                         double(color1.green())/256,
                         double(color1.blue())/256)
                        ;

    this->riw[0]->Render();
}

void SliceDialog::on_colorButton2_clicked()
{
}

void SliceDialog::on_colorButton3_clicked()
{
}

void SliceDialog::on_pushButtonApply_clicked()
{

    vtkSmartPointer<vtkImageProperty> prop =
            vtkSmartPointer<vtkImageProperty>::New();

    prop = this->riw[0]->GetImageActor()->GetProperty();

    prop->BackingOn();


    prop->SetBackingColor(double(color1.red())/256,
                         double(color1.green())/256,
                         double(color1.blue())/256)
                        ;

    this->riw[0]->Render();



    ///////////////////////////////////////////////////////
}



void SliceDialog::on_saveButton_clicked()
{
    QString location = QFileDialog::getSaveFileName(
                this,"Save File",this->ui->titleEdit->text(), tr("Files (*.png)"));

    vtkSmartPointer<vtkWindowToImageFilter> win2Image =
            vtkSmartPointer<vtkWindowToImageFilter>::New();

    vtkSmartPointer<vtkPNGWriter> pngWriter =
            vtkSmartPointer<vtkPNGWriter>::New();

    vtkSmartPointer<vtkRenderWindow> renderWindow =
            this->riw[0]->GetRenderWindow();

/// Get the Image snapshot of the volumetric slice
    win2Image->SetInput(renderWindow);
    win2Image->SetMagnification(2);
    titleActor->SetPosition(titleActor->GetPosition()[0], titleActor->GetPosition()[1] * 2);
    win2Image->Update();

    //);


/// RenderWindow Writer
    pngWriter->SetInputConnection(win2Image->GetOutputPort());

    QByteArray ba = location.toLatin1();
    const char * c_Str_Location = ba.data();
    pngWriter->SetFileName(c_Str_Location);
    renderWindow->Render();

    pngWriter->Write();

     titleActor->SetPosition(titleActor->GetPosition()[0], titleActor->GetPosition()[1] / 2);
}

void SliceDialog::on_resetButton_clicked()
{
    this->riw[0]->SetLookupTable(originalTable);

    this->ui->horizontalSlider_Hue->setValue(0);
    this->ui->horizontalSlider_Sat->setValue(0);
    this->ui->horizontalSlider_Val->setValue(0);

    this->riw[0]->Render();
}


////
/// \brief SliceDialog::on_SaturationChanged
/// \param value
/// Manipulates the Intensity of the Selector
///
void SliceDialog::on_SaturationChanged(int value)
{

    double factor = 0.01;
    double change = value * factor;

    this->ui->saturation_Max->setText(QString("%1").arg(change));

     //vtkSmartPointer<vtkLookupTable >
    customLookup = vtkSmartPointer<vtkLookupTable>::New();

    // bwLut->SetRange(scale2color->GetRange());
     customLookup->SetSaturationRange(change,change);
     //bwLut->SetHueRange(.6,.6);
     //bwLut->SetValueRange(1,1);
     this->riw[0]->SetLookupTable(customLookup);


    this->riw[0]->Render();
}

////
/// \brief SliceDialog::on_HueChanged
/// \param value
/// Manipulates the Opactity and works in conjunction with the Color Tint Selector
///
void SliceDialog::on_HueChanged(int value)
{

    vtkSmartPointer<vtkImageProperty> prop =
            vtkSmartPointer<vtkImageProperty>::New();

    prop = this->riw[0]->GetImageActor()->GetProperty();


    double factor = 0.01;
    double change = value * factor;
    double reversed = 1 - change;

    this->ui->hue_Max->setText(QString("%1").arg(value));

     customLookup = vtkSmartPointer<vtkLookupTable>::New();

    //customLookup->SetHueRange(change,change);
     prop->SetOpacity(reversed);

    this->riw[0]->Render();
}


///
/// \brief SliceDialog::on_TableValueChanged
/// \param value
/// Manipulates the Brightness of the IMage
///
void SliceDialog::on_TableValueChanged(int value)
{

        double factor = 0.1;
        double change = value * factor;
        double reverse = 10 - change;

        this->ui->value_Max->setText(QString("%1").arg(value));

        //this->riw[0]->SetLookupTable(customLookup);
        this->riw[0]->SetColorLevel(reverse);

        this->riw[0]->Render();
    }


void SliceDialog::on_buttonInsertTitle_clicked()
{
    QString titleInput = this->ui->titleEdit->text();
    std::string s = titleInput.toStdString();
    const char* title = s.c_str();

    vtkSmartPointer <vtkRenderer> renderer =
            //renderWindow->GetRenderers()->GetFirstRenderer();
            //this->riw[0]->GetRenderer();
            this->ui->qvtkWidgetRight->GetRenderWindow()->
                GetRenderers()->GetFirstRenderer();


    int *position = riw[0]->GetRenderWindow()->GetSize();


    std::cout << "Window position=" << position[0] << "," << position[1] << endl;


    titleActor->GetTextProperty()->SetFontSize(16);
    titleActor->SetPosition(24,position[1] - 20);
    renderer->AddActor(titleActor);
    titleActor->SetInput(title);
    //titleActor->GetTextProperty()->
    titleActor->GetTextProperty()->SetColor(1.0 ,1.0 ,1.0);
    titleActor->GetTextProperty()->SetBold(1);
    titleActor->GetTextProperty()->SetItalic(1);
    //titleActor->GetTextProperty()->SetVerticalJustificationToCentered();
    //titleActor->GetTextProperty()->SetJustificationToCentered();

    this->riw[0]->Render();

}

//void SliceDialog::on_pushButton_clicked()
//{
//    QString titleInput = this->ui->titleEdit->text();
//    std::string s = titleInput.toStdString();
//    const char* title = s.c_str();

//    vtkSmartPointer <vtkRenderer> renderer =
//            //renderWindow->GetRenderers()->GetFirstRenderer();
//            this->riw[0]->GetRenderer();

//    vtkSmartPointer <vtkTextActor> titleActor =
//            vtkSmartPointer <vtkTextActor>::New();

//    double pos1;
//    double pos2;

//    pos1 = this->ui->textPos1->text().toDouble();
//    pos2 = this->ui->textPos2->text().toDouble();


//    titleActor->GetTextProperty()->SetFontSize(24);
//    titleActor->SetPosition(pos1,pos2);
//    renderer->AddActor(titleActor);
//    titleActor->SetInput(title);
//    //titleActor->GetTextProperty()->
//    titleActor->GetTextProperty()->SetColor(1.0 ,1.0 ,1.0);
//    titleActor->GetTextProperty()->SetVerticalJustificationToCentered();
//    titleActor->GetTextProperty()->SetJustificationToCentered();

//    this->riw[0]->Render();

//}

void SliceDialog::on_pushButton_clicked()
{
//        QString titleInput = this->ui->titleEdit->text();
//        std::string s = titleInput.toStdString();
//        const char* title = s.c_str();

//        vtkSmartPointer <vtkRenderer> renderer =
//                //renderWindow->GetRenderers()->GetFirstRenderer();
//                this->riw[0]->GetRenderer();

//        vtkSmartPointer <vtkTextActor> titleActor =
//                vtkSmartPointer <vtkTextActor>::New();

//        double pos1;
//        double pos2;

//        pos1 = this->ui->textPos1->text().toDouble();
//        pos2 = this->ui->textPos2->text().toDouble();

//        int *position = riw[0]->GetPosition();

//        std::cout << "position=" << position[0] << "," << position[1] << endl;


//        titleActor->GetTextProperty()->SetFontSize(24);
//        titleActor->SetPosition(pos1,pos2);
//        renderer->AddActor(titleActor);
//        titleActor->SetInput(title);
//        //titleActor->GetTextProperty()->
//        titleActor->GetTextProperty()->SetColor(1.0 ,1.0 ,1.0);
//        titleActor->GetTextProperty()->SetVerticalJustificationToCentered();
//        titleActor->GetTextProperty()->SetJustificationToCentered();

//        this->riw[0]->Render();
}

void SliceDialog::on_buttonTitle_clicked()
{
//    QString titleInput = this->ui->titleEdit->text();
//    std::string s = titleInput.toStdString();
//    const char* title = s.c_str();

//    vtkSmartPointer <vtkRenderer> renderer =
//            //renderWindow->GetRenderers()->GetFirstRenderer();
//            this->riw[0]->GetRenderer();

//    vtkSmartPointer <vtkTextActor> titleActor =
//            vtkSmartPointer <vtkTextActor>::New();

//    double pos1;
//    double pos2;

//    pos1 = this->ui->textPos1->text().toDouble();
//    pos2 = this->ui->textPos2->text().toDouble();

//    int *position = riw[0]->GetRenderWindow()->GetSize();

//    std::cout << "position=" << position[0] << "," << position[1] << endl;


//    titleActor->GetTextProperty()->SetFontSize(24);
//    titleActor->SetPosition(pos1,pos2);
//    renderer->AddActor(titleActor);
//    titleActor->SetInput(title);
//    //titleActor->GetTextProperty()->
//    titleActor->GetTextProperty()->SetColor(1.0 ,1.0 ,1.0);
//    titleActor->GetTextProperty()->SetVerticalJustificationToCentered();
//    titleActor->GetTextProperty()->SetJustificationToCentered();

//    this->riw[0]->Render();
}
