#include "vtkfitsreader.h"
//#include <vtkSca.h"

//#include "vtkFLo"

vtkFitsReader::vtkFitsReader()
{
      this->filename[0]='\0';
      this->xStr[0]='\0';
      this->yStr[0]='\0';
      this->zStr[0]='\0';
      this->title[0]='\0';

    this->dimensions[0] = 0  ;   // [x,y,z]
    this->points    = 0      ;   // Total # of points in DataSet
    this->datamin   = 0.0    ;   // Part of FITS Header file
    this->datamax   = 0.0    ;   // Part of FITS Header file
}

void vtkFitsReader::SetFileName(const char *name) {

  if (!name) {
    vtkErrorMacro(<<"Null Datafile!");
    return;
  }
  sprintf(this->filename, "%s", name);

  this->Modified(); // Needs to Execute() next time needed (load new dataset)
  this->Execute();
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

// Note: This function adapted from readimage() from cookbook.c in
// fitsio distribution.
void vtkFitsReader::Execute() {

  vtkStructuredPoints *output = (vtkStructuredPoints *) this->GetOutput();

  fitsfile *fptr;
  int status = 0, nfound = 0, anynull = 0;
  long naxes[3], fpixel, nbuffer, npixels, ii;
  const int buffsize = 1000;

  float datamin, datamax, nullval, buffer[buffsize];

  if ( fits_open_file(&fptr, this->filename, READONLY, &status) )
    printerror( status );

  /* read the NAXIS1 and NAXIS2 keyword to get image size */
  if ( fits_read_keys_lng(fptr, "NAXIS", 1, 3, naxes, &nfound, &status) )
    printerror( status );

  npixels  = naxes[0] * naxes[1] * naxes[2]; /* num of pixels in the image */
  fpixel   = 1;
  nullval  = 0;                /* don't check for null values in the image */
  datamin  = 1.0E30;
  datamax  = -1.0E30;

  //cerr << "\nvtkFitsReader: " << this->filename << endl;

//  qDebug() << "\nvtkFitsReader: " << this->filename;

  this->dimensions[0] = naxes[0] ;
  this->dimensions[1] = naxes[1] ;
  this->dimensions[2] = naxes[2] ;
  //qDebug() << "\nDim: " << naxes[0] << " " << naxes[1] << " " << naxes[2]  ;

  this->points = npixels;
  //qDebug() << "\npoints: " << npixels  ;

  output->SetDimensions(naxes[0], naxes[1], naxes[2]);

  output->SetOrigin(0.0, 0.0, 0.0);

  vtkFloatArray *scalars = vtkFloatArray::New();
//  vtkFloatScalars *scalars = vtkFloatScalars::New();
    scalars->Allocate(npixels);

  while (npixels > 0) {

    nbuffer = npixels;
    if (npixels > buffsize)
      nbuffer = buffsize;

    if ( fits_read_img(fptr, TFLOAT, fpixel, nbuffer, &nullval,
                       buffer, &anynull, &status) )
      printerror( status );

    for (ii = 0; ii < nbuffer; ii++)  {

      if (isnanf(buffer[ii])) buffer[ii] = -1000000.0; // hack for now
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
  //qDebug() << "min: " << datamin << " max: " << datamax << endl;

  if ( fits_close_file(fptr, &status) )
       printerror( status );

  output->GetPointData()->SetScalars(scalars) ;
 // output->GetPointData()->SetScalars(scalars) ;

  //qDebug() << "Loaded." << endl;

//qDebug().stream->
  return;
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
            //cerr << card << endl;
            char *first = strchr(card, '\'');
            char *last = strrchr(card, '\'');
            *last = '\0';
            strcpy(title, first+1);
          }
      }
  }
  //cerr << "\nextracted strings:\n" << title << "\n" << xStr << "\n" << yStr << "\n" << zStr << endl;
}

void vtkFitsReader::PrintSelf(ostream& os, vtkIndent indent) {
  os << indent << "FITS File Name: " << (this->filename) << "\n";
  vtkStructuredPointsSource::PrintSelf(os, indent);
}

void vtkFitsReader::PrintDetails()
{
    string path = this->filename;
    string test = "heellloo";

    //qDebug() << test << endl;
    //cout << path.substr(path.find_last_of('\\') + 1) << endl;
    //qDebug() << "Filename = " << path.substr(path.find_last_of('\\') + 1) << endl;
    qDebug() << "Filename = " << this->filename << endl;

    qDebug() << "Dimensions =  x:" << this->dimensions[0];
    qDebug() <<               "y:" << this->dimensions[1];
    qDebug() <<               "z:" << this->dimensions[2] << endl;

    qDebug() << "Points  = " << this->points << endl;
    qDebug() << "DataMin = " << this->datamin << endl;
    qDebug() << "DataMax = " << this->datamax << endl;
    qDebug() << "---------------------------------" << endl;
}
