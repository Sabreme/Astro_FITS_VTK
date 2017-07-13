#include "vtkfitsreader.h"

#include "vtkStructuredPoints.h"
#include "vtkObjectFactory.h"
#include "vtkFloatArray.h"
#include "vtkDataArray.h"

#include "vtkDataSet.h"
#include "vtkDataSetAttributes.h"

#include "vtkExecutive.h"
#include "vtkErrorCode.h"
#include "vtkPointData.h"
#include "vtkImageData.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"


vtkStandardNewMacro(vtkFitsReader);

vtkFitsReader::vtkFitsReader()
{    
    this->filename[0]='\0';
    this->xStr[0]='\0';
    this->yStr[0]='\0';
    this->zStr[0]='\0';
    this->title[0]='\0';
    this->naxis     = 0      ;   // Number of Dimensions in FITS File
    this->epoch     = 0.0    ;   // Part of FITS Header file
    this->telescope[120] = '\0'; // Part of FITS Header file

    this->dimensions[0] = 0  ;   // [x,y,z]
    this->points    = 0      ;   // Total # of points in DataSet
    this->datamin   = 0.0    ;   // Part of FITS Header file
    this->datamax   = 0.0    ;   // Part of FITS Header file

    vtkStructuredPoints *output = vtkStructuredPoints::New();
    this->SetOutput(output);

    output->ReleaseData();
    output->Delete();   

}

vtkFitsReader::~vtkFitsReader()
{}

void vtkFitsReader::SetOutput(vtkStructuredPoints *output)
{
    this->GetExecutive()->SetOutputData(0, output);
}

vtkStructuredPoints* vtkFitsReader::GetOutput()
{
    return this->GetOutput(0);
}

vtkStructuredPoints* vtkFitsReader::GetOutput(int idx)
{
    return vtkStructuredPoints::SafeDownCast(this->GetOutputDataObject(idx));
}


void vtkFitsReader::SetFileName(const char *name) {

  if (!name) {
    vtkErrorMacro(<<"Null Datafile!");
    return;
  }
  sprintf(this->filename, "%s", name);
}

// Note: from cookbook.c in fitsio distribution.
void vtkFitsReader::printerror(int status) {

    cerr << "vtkFitsReader ERROR.";
    if (status) {
       fits_report_error(stderr, status); /* print error report */
       exit( status );    /* terminate the program, returning error status */
    }
    return;
}

//----------------------------------------------------------------------------
// Default method performs Update to get information.  Not all the old
// structured points sources compute information
int vtkFitsReader::RequestInformation (
  vtkInformation*,
  vtkInformationVector**,
  vtkInformationVector* outputVector)
{ 
  // get the info objects
  //vtkInformation* outInfo = outputVector->GetInformationObject(0);
  return 1;
}

int vtkFitsReader::FillOutputPortInformation(int, vtkInformation *info)
{    
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkStructuredPoints");
    return 1;
}

// Note: This function adapted from readimage() from cookbook.c in
// fitsio distribution.
int vtkFitsReader::RequestData(
        vtkInformation* ,
        vtkInformationVector** ,
        vtkInformationVector* outputVector)
{

  vtkInformation * outInfo = outputVector->GetInformationObject(0);

  this->SetErrorCode( vtkErrorCode::NoError );

  vtkStructuredPoints * output = vtkStructuredPoints::SafeDownCast(
              outInfo->Get(vtkDataObject::DATA_OBJECT()));

  fitsfile *fptr;
  int status = 0, nfound = 0, anynull = 0;
  long naxes[4], fpixel, nbuffer, npixels, ii;
  const int buffsize = 100000;
  char comm[10] ;
  long pNaxis[0], pEpoch[0];

  float datamin, datamax, nullval, buffer[buffsize];

  if ( fits_open_file(&fptr, this->filename, READONLY, &status) )
    printerror( status );

  /* read the NAXIS1 and NAXIS2 keyword to get image size */
  if ( fits_read_keys_lng(fptr, "NAXIS", 1, 3, naxes, &nfound, &status) )
    printerror( status );

   fits_read_key_lng(fptr, "NAXIS",pNaxis,comm, &status);
     //rinterror( status );

  ///fits_read_key_lng(fptr, "EPOCH",pEpoch,comm, &status);

  this->naxis = pNaxis[0];
  this->epoch = pEpoch[0];


  npixels  = naxes[0] * naxes[1] * naxes[2]; /* num of pixels in the image */
  fpixel   = 1;
  nullval  = 0;                /* don't check for null values in the image */
  datamin  = 1.0E30;
  datamax  = -1.0E30;

  this->dimensions[0] = naxes[0] ;
  this->dimensions[1] = naxes[1] ;
  this->dimensions[2] = naxes[2] ;

  this->points = npixels;

  output->SetDimensions(naxes[0], naxes[1], naxes[2]);

  output->SetOrigin(0.0, 0.0, 0.0);

  vtkFloatArray *scalars = vtkFloatArray::New();

  scalars->Allocate(npixels);

  while (npixels > 0) {

    nbuffer = npixels;
    if (npixels > buffsize)
      nbuffer = buffsize;

    if ( fits_read_img(fptr, TFLOAT, fpixel, nbuffer, &nullval,
                       buffer, &anynull, &status) )
      printerror( status );

    for (ii = 0; ii < nbuffer; ii++)
    {
        if (isnan(buffer[ii]))
            buffer[ii] = -1000000.0; // hack for now
        scalars->InsertNextValue(buffer[ii]);

        if ( buffer[ii] < datamin )
                datamin = buffer[ii];
        if ( buffer[ii] > datamax )
            datamax = buffer[ii];
    }

    npixels -= nbuffer;    /* increment remaining number of pixels */
    fpixel  += nbuffer;    /* next pixel to be read in image */
  } 

  this->datamin = datamin;
  this->datamax = datamax;

  if ( fits_close_file(fptr, &status) )
       printerror( status );

  output->GetPointData()->SetScalars(scalars) ;

//  output->SetScalarType(scalars->GetDataType());

  int extent[6];

  extent[0] = 0;
  extent[1] = this->dimensions[0]-1;
  extent[2] = 0;
  extent[3] = this->dimensions[1]-1;
  extent[4] = 0;  
  extent[5] = this->dimensions[2]-1;

//  if (this->dimensions[2] ==1)
//      extent[5] = 1;
//  output->SetWholeExtent(extent);

  ReadHeader();

  return 1;
}

// Note: This function adapted from printheader() from cookbook.c in
// fitsio distribution.
void vtkFitsReader::ReadHeader() {
  fitsfile *fptr;       /* pointer to the FITS file, defined in fitsio.h */

  int status, nkeys, keypos, hdutype, ii, jj;
  char card[FLEN_CARD];   /* standard string lengths defined in fitsioc.h */

  status = 0;

  if ( fits_open_file(&fptr, filename, READONLY, &status) )
       printerror( status );

  /* attempt to move to next HDU, until we get an EOF error */
  for (ii = 1; !(fits_movabs_hdu(fptr, ii, &hdutype, &status) ); ii++)
  {
      /* get no. of keywords */
      if (fits_get_hdrpos(fptr, &nkeys, &keypos, &status) )
          printerror( status );

      //cerr << "from header:" << endl;
      // not part of header...
      //printf("Header listing for HDU #%d:\n", ii);
      for (jj = 1; jj <= nkeys; jj++)  {
          if ( fits_read_record(fptr, jj, card, &status) )
               printerror( status );

          if (!strncmp(card, "CTYPE", 5)) {
            //cerr << card << endl;
            char *first = strchr(card, '\'');
            char *last = strrchr(card, '\'');
            *last = '\0';
            if (card[5] == '1')
              strcpy(xStr, first+1);
            if (card[5] == '2')
              strcpy(yStr, first+1);
            if (card[5] == '3')
              strcpy(zStr, first+1);
          }
          if (!strncmp(card, "OBJECT", 6)) {
            char *first = strchr(card, '\'');
            char *last = strrchr(card, '\'');
            *last = '\0';
            strcpy(title, first+1);
          }

          if (!strncmp(card, "TELESCOP", 6)) {
            char *first = strchr(card, '\'');
            char *last = strrchr(card, '\'');
            *last = '\0';
            strcpy(telescope, first+1);
          }
      }
  }
  //cerr << "\nextracted strings:\n" << title << "\n" << xStr << "\n" << yStr << "\n" << zStr << endl;
}

void vtkFitsReader::PrintSelf(ostream& os, vtkIndent indent) {
  os << indent << "FITS File Name: " << (this->filename) << "\n";  
}

void vtkFitsReader::PrintDetails()
{
    string path = this->filename;
    string test = "heellloo";

    //cout << path.substr(path.find_last_of('\\') + 1) << endl;
    //qDebug() << "Filename = " << path.substr(path.find_last_of('\\') + 1) << endl;
    qDebug() << "Filename = " << this->filename << endl;

    qDebug() << "Dimensions =  x:" << this->dimensions[0];
    qDebug() <<               "y:" << this->dimensions[1];
    qDebug() <<               "z:" << this->dimensions[2] << endl;

    qDebug() << "title = " << this->title << endl;
    qDebug() << "xStr = " << this->xStr << endl;
    qDebug() << "yStr = " << this->yStr << endl;
    qDebug() << "zStr = " << this->zStr << endl;
    qDebug() << "telescope = " << this->telescope << endl;

    qDebug() << "naxis = " << this->naxis << endl;
    qDebug() << "epoch = " << this->epoch << endl;

    qDebug() << "Points  = " << this->points << endl;
    qDebug() << "DataMin = " << this->datamin << endl;
    qDebug() << "DataMax = " << this->datamax << endl;
    qDebug() << "---------------------------------" << endl;
}
