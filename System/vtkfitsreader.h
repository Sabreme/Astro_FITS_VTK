#ifndef VTKFITSREADER_H
#define VTKFITSREADER_H

#ifdef _WIN32
    #include <fitsio\fitsio.h>
#elif __linux__
    #include "fitsio.h"
#elif __APPLE__
    #include "fitsio/fitsio.h"
#endif

#include "vtkDataReader.h"



#include <sstream>
//#include "vtkImageData.h"
//#include "vtkPointData.h"
#include "QDebug"
//#include <string.h>
using namespace std;

class vtkStructuredPoints;

extern "C"
{
#ifdef _WIN32
   #include <fitsio\fitsio.h>
#elif __linux__
   #include "fitsio.h"
#endif
}

class VTK_EXPORT vtkFitsReader : public vtkDataReader
{
public:
    //vtkFitsReader();
    static vtkFitsReader *New();
//    vtkTypeRevisionMacro(vtkFitsReader,vtkDataReader);

    const char *GetClassName() {return "vtkFitsReader";}
    void PrintSelf(ostream& os, vtkIndent indent);
    void PrintDetails();

    void SetFileName(const char *name);
    char *GetFileName(){return filename;}
    double GetNAxis(){return naxis;}
    long  GetEpoch() {return epoch;}
    char * GetTelescope() {return telescope;}
    char * GetObjectSky() {return title;}

    long * GetDimensions() { return dimensions;}
    long GetPoints() {return points;}
    float GetDataMin() {return datamin;}
    float GetDataMax() {return datamax;}

    char * GetXAxisInfo() {return xStr;}
    char * GetYAxisInfo() {return yStr;}
    char * GetZAxisInfo() {return zStr;}



    //void Execute();
    // Description
    // Set/Get the output of this reader
    void SetOutput(vtkStructuredPoints * output);
    vtkStructuredPoints * GetOutput(int idx);
    vtkStructuredPoints * GetOutput();



protected:
    vtkFitsReader();
    ~vtkFitsReader();

    virtual int RequestData(vtkInformation *,
                            vtkInformationVector **,
                            vtkInformationVector *outputVector);

    // Default method performs Update to get information.  Not all the old
    // structured points sources compute information
    virtual int RequestInformation(vtkInformation *, vtkInformationVector **,
                                   vtkInformationVector *outputVector);

    virtual int FillOutputPortInformation(int, vtkInformation *);


private:

    vtkFitsReader(const vtkFitsReader&);    // Not Implemented
    void operator=(const vtkFitsReader&);   // Not Implemented

    void ReadHeader();
    void printerror(int status); // from fitsio distribution

    char title[80];       // FITS Header File OBJECT
    char xStr[80];        // FITS Header File CTYPE1 (RA)
    char yStr[80];        // FITS Header File CTYPE2 (DE)
    char zStr[80];        // FITS Header File CTYPE3 (VELO)
    long epoch;           // Part of FITS Header file
    char telescope[80];  // Part of FITS Header file

    //  char dimensions [12];
    //  char points [20];
    //  char point_ratio[80];
    //  char status [20];

    long dimensions[3];   // [x,y,z]
    long points;          // Total # of points in DataSet
    float datamin;        // Part of FITS Header file
    float datamax;        // Part of FITS Header file
    char filename[256];   // static buffer for filename
    double naxis;         // Number of Dimensions in FITS File
};




#endif // VTKFITSREADER_H
