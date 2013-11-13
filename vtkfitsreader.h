#ifndef VTKFITSREADER_H
#define VTKFITSREADER_H
#include "fitsio.h"
//#include <ieeefp.h>
#include "vtkFloatArray.h"
#include <vtkStructuredPointsSource.h>
#include <vtkStructuredPointsReader.h>
#include <vtkStructuredPoints.h>
#include <sstream>
#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "QDebug"
//#include <string.h>
using namespace std;


 extern "C"
{
    #include "fitsio.h"
}

class VTK_EXPORT vtkFitsReader : public vtkStructuredPointsSource
{
  public:
  vtkFitsReader();

  static vtkFitsReader *New() {return new vtkFitsReader;};
  const char *GetClassName() {return "vtkFitsReader";}
  void PrintSelf(ostream& os, vtkIndent indent);  
  void PrintDetails();

  void SetFileName(const char *name);
  char *GetFileName(){return filename;}
  void Execute();

  char title[80];
  char xStr[80];
  char yStr[80];
  char zStr[80];

//  char dimensions [12];
//  char points [20];
//  char point_ratio[80];
//  char status [20];

  long dimensions[3];   // [x,y,z]
  long points;          // Total # of points in DataSet
  float datamin;        // Part of FITS Header file
  float datamax;        // Part of FITS Header file
  char filename[256];   // static buffer for filename



  std::stringstream tempString;

  protected:



  void ReadHeader();
  void printerror(int status); // from fitsio distribution
};




#endif // VTKFITSREADER_H
