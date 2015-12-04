#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolume16Reader.h>
#include <vtkPolyDataMapper.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkActor.h>
#include <vtkOutlineFilter.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkPolyDataNormals.h>
#include <vtkContourFilter.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkCubeSource.h>
#include <vtkOutlineFilter.h>
#include <vtkDecimatePro.h>
#include <vtkScalarBarActor.h>

#include <System/vtkfitsreader.h>
#include <vtkMarchingCubes.h>
#include <vtkSliceCubes.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkStructuredPoints.h>
#include <vtkActor.h>
#include <vtkLookupTable.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkImageResample.h>

#include <vtkOrientationMarkerWidget.h>
#include <vtkAxesActor.h>
#include <vtkPropAssembly.h>
#include <vtkExtractVOI.h>
//#include <vtkBoxWidget.h>
#include <SubVolume/vtkCustomBoxWidget.h>
#include <vtkCubeAxesActor.h>

#include "vtkResliceImageViewer.h"
#include "vtkImagePlaneWidget.h"
#include "vtkDistanceWidget.h"
#include "vtkResliceImageViewerMeasurements.h"
#include "vtkVector.h"
#include "vtkLineWidget.h"

#include <vtkPointWidget.h>
#include <Slicing/vtkimplicitcustomplanewidget.h>

#include <QMainWindow>
#include <QApplication>
#include <QVTKWidget.h>
#include <Touch/vtkTouchWidget.h>

#include <QProgressDialog>
#include <QTime>

#include "SubVolume/subvolumedialog.h"
#include "InfoDlg/infobardialog.h"
#include "Usability/UserTesting.h"
#include "Usability/usertestdialog.h"
#include "Usability/countDialog.h"

#include <QFutureWatcher>
#include <QtGui>
#include "time.h"
#include <QThread>

#include <vtkPlaneWidget.h>
#include <vtkSliderWidget.h>
#include <vtkArrowSource.h>
#include <vtkSphereSource.h>
#include <vtkTextActor.h>

#include <vtkSystemIncludes.h>

#include <Leap/vtkLeapMarkerWidget.h>

#include <Hands/vtkLeapHandWidget.h>


#include "Leap/Leap.h"
#include "Leap/Sample.h"

#include "Hands/handRenderer.h"

using namespace Leap;

namespace Ui {
    class MainWindow;
}

class Thread :public QThread{
public:
    Thread(QObject *parent = 0) : QThread(parent)
    {

    }

protected:
    void run(){
        exec();
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void log(QString message);

    void loadFitsFile(QString filename);


    void closeTabs();

    /////////////////////////////////////////////////////
    /////////////////////////////////////////////////////
    /////////////////////////////////////////////////////
    /// ENUMERATORS FOR SYSTEM STATES
    ///
    enum app_Mode_t {Mouse, Leap, Touch};
    enum app_Tab_t {Information, SubVolume, SliceAxis, SliceArb};
    enum app_TransForm_t {Rotation, Translation, Scaling, None};
    enum userTest_t {RotateCount, TranslateCount, ScaleCount, ResetCount, SubVolResetCount, SliceResetCount};

    QVTKWidget* vtkWidget;      // Used for Main loop for FrameRate Calculation
    QTimer resizeTimer;
    QTimer buttonEnablerTimer;


    double Distance2Point(double *point1, double *point2);
    void UpdateFitsFileInfo();
    void KeyboardResetCamera();

    //// SUB-VOLUME
    void loadSubVolume();

    //// AXIS-SLICE
    void LoadSliderWidgets();

    //// ARB-SLICE
    vtkLookupTable *BuildHueIntensityBaseMap(double min, double max);

    //// LEAP MOTION
    void LeapMotion();

    bool event(QEvent *event);


public slots:
    virtual void slotExit();
    void infoTabCloseSignal();
    bool MessageBoxQuery(QString title, QString question);

    void releaseTabFocus();
    void resizeDone();

    //// Interactor Slots
    double getDefaultCameraPos(int dimension);

    /// VTK TOuch Widget Public Slots
    void updateCameraPosition();

    /////////////////////////////////////////////////////
    ///USERTESTING PUBLIC SLOTS
    ///
    void startUserTest();
    void startUserPractice();
    void stopUserPractice();

    QString getUserTestResults(int task);


    void stopUserTest();

    bool userTestRunning();

    void countInteraction(int testType);
    void saveScreenShot();


protected:
        void ModifiedHandler();
        void resizeEvent(QResizeEvent* event);

private slots:
        /////////////////////////////////////////////////
        //// GENERAL SLOTS
        void updateme();
        void button_Exit_clicked();
        void button_Open_clicked();

        void mySlot();
        void slot_Load_Finished();
        void on_actionOpen_triggered();
        void on_actionReload_triggered();
        void on_actionExit_triggered();

        void on_actionDemo_triggered();

        //void actionDefault_triggered();
        void on_actionBlack_White_triggered();
        void on_actionBlue_Red_triggered();

        void on_actionDefault_triggered();

        void spectrumToggled(bool status);
        void cubeAxesToggled(bool status);

        void closeTab(int index);

        void resetTransformCoords();

        void printBounds(const char*, double[6]);

        /////////////////////////////////////////////////////
        /// SYSTEM STATUS SLOTS
        ///

        void on_buttonModeMouse_clicked();
        void on_buttonModeLeap_clicked();
        void on_buttonModeTouch_clicked();

        void on_buttonTabInfo_pressed();
        void on_buttonTabSubVol_pressed();
        void on_buttonTabSliceAxis_pressed();
        void on_buttonTabSliceArb_pressed();

        /////////////////////////////////////////////////////
        ///// INFORMAITON SLOTS
        ///
        void infoTab_Triggered();

        /////////////////////////////////////////////////////
        ///// INTERACTOR SLOTS
        ///
        void setMouseInteractor();
        void setTouchInteractor();
        void setLeapInteractor();

        /////////////////////////////////////////////////////
        //// CAMERA SLOTS
        void on_buttonCameraReset_clicked();
        void on_actionReset_Camera_triggered();

        void on_actionLeft_Side_View_triggered();
        void on_actionRight_Side_View_triggered();
        void on_actionTop_Side_View_triggered();
        void on_actionBottom_Side_View_triggered();
        void on_actionRear_Side_View_triggered();
        void on_actionFront_Side_View_triggered();


        ////////////////////////////////////////////////////
        //// SUB-VOLUME SLOTS
        void mouseBeginSubVol();
        void on_buttonSubVolReset_clicked();

        void on_actionSubVolumeXY_triggered();
        void on_action_SubVolume_Z_triggered();
        //void on_actionSubVolSelect_triggered();
        void on_actionSubVol_Export_triggered();
        void boxWidgetCallback();
        void on_actionSubVolSelection_triggered();
        void trackSubVolume(double*, double*);

        void on_checkSubVolVolumeEnable_stateChanged(int arg1);
        void on_buttonSubVolExport_clicked();

        ///////////////////////////////////////////////////////
        //// AXIS-SLICE SLOTS

        void beginSliceAxis();
        void on_actionSliceAxisAligned_triggered();
        void on_checkAxisSliceVolumeEnable_stateChanged(int arg1);

        void ResetViews();
        void ResetViewX();
        void ResetViewY();
        void ResetViewZ();
        void UpdateSliceX();
        void UpdateSliceY();
        void UpdateSliceZ();
        void UpdateSliceX(int slice);
        void UpdateSliceY(int slice);
        void UpdateSliceZ(int slice);
        void Render();
        void AddDistanceMeasurementToView1();
        void AddDistanceMeasurementToView(int i);

        void sliceXEnable(int mode);
        void sliceYEnable(int mode);
        void sliceZEnable(int mode);

        void captureSlice(int axis);
        void on_buttonExportXSlice_clicked();
        void on_buttonExportYSlice_clicked();
        void on_buttonExportZSlice_clicked();

        void on_actionWorldCoords_triggered();

        ///////////////////////////////////////////////////////
        //// ARB-SLICE SLOTS
        ///
        void beginSliceArb();
        void on_buttonArbReset_clicked();

        void on_actionSliceAxisArbitrary_triggered();
        void on_checkArbSliceVolumeEnable_stateChanged(int arg1);
        void on_buttonArbSliceContourBackground_clicked();
        void on_slider_CntourDisplay_valueChanged(int value);

        //////////////////////////////////////////////////////
        //// LEAP-MOTION SLOTS

        void leapTrackingOn(bool arg1);
        void leapBeginSubVol();
        void leapBeginSliceAxis();
        void leapBeginSliceArb();

        void on_actionLeapBasic_triggered();

        void on_actionTracking_triggered();
        void addSecondPointer();
        void addThirdPointer();
        void on_actionStats_triggered();

        //////////////////////////////////////////////////////
        //// MULTI-TOUCH SLOTS
        ///
        void touchInteractionEvent();
        void touchBeginSubVol();
        void touchUpdateSubVol();

        void touchTranslatePressed();
        void touchTranslateReleased();

        void touchRotationPressed();
        void touchRotationReleased();
        void touchTransformsOn(bool status);

        void touchFinger1Pressed();





    ///    void touchBeginSliceAxis();
    ///    void touchBeginSliceArb();


        ////////////////////////////////////////////////////////
        //// TEST-SECTION SLOTS
        void on_actionTestButton_triggered();

        void on_actionInfoBarToggle_toggled(bool arg1);

        void on_actionFPSToggle_toggled(bool arg1);

        void on_actionUserTesting_toggled(bool arg1);

        void scaleButtonChanged();
        void scaleButtonDelay();

        /////////////////////////////////////////////////////////
        /// \brief on_buttonCameraReset_clicked
        ////////////////////////////////////////////////////////

        void on_actionTestButton2_triggered();

private:

        //////////////////////////////////////////////////
        /// GENERAL
        ///

        Ui::MainWindow      *ui;                                // Global pointer for the Mainwindow
        app_Mode_t          systemMode;
        app_Tab_t           systemTab;
        app_TransForm_t     systemTransF;

        bool                infoTabOpen;
        QString             currentFitsFile;
        InfoBarDialog       * infoTabDlg;

        //////////////////////////// USER TESTING /////////////////////////
        UserTesting         * userTest;
        UserTestDialog* userTestDlg;
        countDialog* userPractice;

        bool userTestActive  = false;

        int userTestCountRotation = 0;



        vtkFitsReader               * global_Reader;            // Global Pointer for the loaded FitsReader
      //  vtkStructuredPoints *global_Points;                   // Global Pointer for the point Dataset
        vtkVolume                   * global_Volume;            // Global Pointer for the Current Volume
        vtkActor                    * global_Outline;           // Global pointer for the Outline volume
        vtkCubeAxesActor            * defaultCubeAxes;          // Global pointer for the axes of volume
        vtkGPUVolumeRayCastMapper   * global_Mapper;            // Global Pointer for Volume Mapper
        vtkLookupTable              * default_Volume_Colours;   // Global Pointer for the Default Volume schema
        vtkRenderWindowInteractor   * defaultInter;             // Global Pointer for the Default Interactor
        vtkRenderer                 * defaultRenderer;          // Global Pointer for the Default Renderer
        vtkRenderWindow             * defaultRenWindow;         // Global Pointer for Default RenderWindow
        vtkImageResample            * global_Resample;           // Global Pointer for the Input Image Source;
        vtkScalarBarActor               * defaultScalarBar;         //Global Pointer for the ScalarBar;

        QTime Timer;
        QProgressDialog             * ProgressDialog;
        QFutureWatcher<void> FutureWatcher;

          int TabCount = 6;

        //////////////////////////////////////////////////
        /// CAMERA SETTINGS
        ///
          double global_subVolBounds_[6] ;            // Global Pointer for the Sub-Volume Bounds
          double cameraAzimuth;                       // Global counter for Current Camera's Azimuth
          double cameraElevation;                     // Global counter for Current Camera's Azimuth

          double cameraFocalPoint[3];                 // Global counter for current Camera's Focal Point
          double cameraPosition[3];                   // Global counter for current Camera's Focal Point
          double cameraViewUp[3];                     // Global counter for current Camera's Focal Point

          double defaultCameraPosition[3];            // Default Camera Position
          double defaultCameraFocalPnt[3];            // Default Camera Focal Point
          double defaultCameraViewUp[3];              // Default Camera ViewUp Vector

          double defaultCameraDistance;               // Default Camera Distance from Focal Point


        //////////////////////////////////////////////////
        /// SUB-VOLUME SETTINGS
        ///

         SubVolumeDialog *subVolDialog;              // Global pointer for the SubVolume
         vtkCubeSource *global_subVolume;            // Global Pointer for the Sub-Volume
         vtkCustomBoxWidget* boxWidget_;                   // Global Pointer for the BoxWidget;


        //////////////////////////////////////////////////
        /// AXIS-SLICE SETTINGS
        ///

         vtkSmartPointer<vtkSliderWidget> sliderWidgetX;  // Global Pointer for the Slice Sliders
         vtkSmartPointer<vtkSliderWidget> sliderWidgetY;  // Global Pointer for the Slice Sliders
         vtkSmartPointer<vtkSliderWidget> sliderWidgetZ;  // Global Pointer for the Slice Sliders


        //vtkActor volumeActor;

        //////////////////////////////////////////////////
        /// ARB-SLICE SETTINGS
        ///
        vtkImplicitCustomPlaneWidget * customArbPlaneWidget ;

        //////////////////////////////////////////////////
        /// HAND MODEL SETTINGS
        ///
        double                              boxBounds[6] = {-3, 3, 1, 5, -2,3};
        HandRenderer                * handRenderer;
        bool                                    handModelActive = false;

        //////////////////////////////////////////////////
        /// LEAP MOTION SETTINGS
        ///

        SampleListener*   listener_;
        Controller*       controller_;
        Leap::Matrix      leapMatrixTotalMotionRotation;
        Leap::Vector      leapVectorTotalMotionalTranslation;
        float             leapFloatTotalMotionScale;

        vtkPlaneWidget  * leapDbgPlaneWidget;        // leap Diagnostic DIsplay
        vtkArrowSource  * leapDbgArrow;
        vtkPlaneSource  * leapDbgPlane;
        vtkSphereSource * leapDbgSphere;
        vtkPointWidget  * leapDbgPointWidget;

        vtkActor        * leapDbgSphereActor;
        vtkActor        * leapDbgArrowActor;
        vtkTextActor    * leapTrackingActor;



        vtkPointWidget* pointWidget1_;               // Global Pointer for the PointWidget
        vtkPointWidget* pointWidget2_;              // Global Pointer for the PointWidget
        vtkPointWidget* pointWidget3_;              // Global Pointer for the PointWidget


        bool Leaping_ = false;
        bool leapAxisSliceMode = false;
        bool leapArbSliceMode = false;
        bool leapSubVolMode = false;
        bool longSubVolMode_ = true;                // Global Val for default SubVolumeMode Selection

        bool leapHand1Move = false;
        int leapHand1FrameBuffer = 0;                      //Global Variable for counting # of action frames
        int leapHand1GestureCounter = 0;              // Global Variable for Keeping track of new gestures

        bool leapHand2Move = false;
        int leapHand2FrameBuffer = 0;
        int leapHand2GestureCounter = 0;

        vtkSmartPointer< vtkResliceImageViewer > riw[3];
        vtkSmartPointer< vtkImagePlaneWidget > planeWidget[3];
        vtkSmartPointer< vtkDistanceWidget > DistanceWidget[3];
        vtkSmartPointer< vtkResliceImageViewerMeasurements > ResliceMeasurements;
        vtkSmartPointer< vtkLineWidget>    lineWidget[3];
        vtkSmartPointer< vtkLookupTable>  lookupTable3D;
        vtkSmartPointer< vtkLookupTable>  lookupTable2D;
        vtkSmartPointer< vtkContourFilter> global_Contours2D;

        int xAxisSlice = 0;
        int yAxisSlice = 0;
        int zAxisSlice = 0;
        int imageDims[3];

        vtkSmartPointer< vtkPlaneWidget > customPlaneWidget;

        vtkLeapMarkerWidget * leapMarkerWidget   = NULL;
        vtkLeapHandWidget * leapHandWidget = NULL;


};

#endif // MAINWINDOW_H


