/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/
#ifndef __OPENCV_ONLINE_BOOSTING_H__
#define __OPENCV_ONLINE_BOOSTING_H__

#include <cv.h>

//check the operating system types
#ifndef OS_type
#if defined(unix)        || defined(__unix)      || defined(__unix__) \
  || defined(linux)       || defined(__linux)     || defined(__linux__) \
  || defined(sun)         || defined(__sun) \
  || defined(BSD)         || defined(__OpenBSD__) || defined(__NetBSD__) \
  || defined(__FreeBSD__) || defined __DragonFly__ \
  || defined(sgi)         || defined(__sgi) \
  || defined(__MACOSX__)  || defined(__APPLE__) \
  || defined(__CYGWIN__)
#define OS_type 1
#elif defined(_MSC_VER) || defined(WIN32)  || defined(_WIN32) || defined(__WIN32__) \
  || defined(WIN64)    || defined(_WIN64) || defined(__WIN64__)
#define OS_type 2
#else
#define OS_type 0
#endif
#elif !(OS_type==0 || OS_type==1 || OS_type==2)
#error CImg Library : Configuration variable 'OS_type' is badly defined.
#error (valid values are '0=unknown OS', '1=Unix-like OS', '2=Microsoft Windows').
#endif


//include platform specific headers
#if OS_type==1
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include <QtCore>

//typedef int32_t __int32;
//typedef int32_t __int64;
//typedef uint32_t  __uint32;
//typedef uint64_t  __uint64;

//MAX_PATH variable used by helmut
#ifdef MAX_PATH /* Work-around for Mingw */
#undef MAX_PATH
#endif /* MAX_PATH */
#define MAX_PATH 260

#elif OS_type==2
#include <windows.h>
#include <shlobj.h>
#include <stdint.h>

//general headers
#include <vector>
#include <vector>
#include <string>
#include <map>
#include <cstdio>
#include <deque>

//MAX_PATH variable used by helmut
#ifdef MAX_PATH /* Work-around for Mingw */
#undef MAX_PATH
#endif /* MAX_PATH */
#define MAX_PATH        260
#ifndef _WIN32_IE
#define _WIN32_IE 0x0400
#endif
#endif

namespace cv
{
  namespace boosting
  {

    typedef uint32_t  __uint32;
    typedef uint64_t  __uint64;

    class Point2D;
    class Size;

    class Color
    {
    public: 

      Color();
      Color(int red, int green, int blue);
      Color(int idx);

      int red;
      int green;
      int blue;
    };

    class Rect
    {
    public:

      Rect();
      Rect(int upper, int left, int height, int width);

      int upper;
      int left;
      int height;
      int width;

      float confidence;

      Rect operator+ (Point2D p);
      Rect operator+ (Rect r);
      Rect operator- (Point2D p);
      Rect operator* (float f);
      Rect operator= (Size s);
      Rect operator= (Rect r);
      bool operator== (Rect r);
      bool isValid (Rect validROI);

      int checkOverlap (Rect rect);
      int getArea(){return height*width;};
      bool isDetection(Rect eval, unsigned char *labeledImg, int imgWidth);

      CvRect getCvRect();
    }; 

    class Size
    {
    public:

      Size();
      Size(int height, int width);

      int height;
      int width;

      Size operator= (Rect r);
      Size operator= (Size s);
      Size operator* (float f);
      bool operator== (Size s);

      int getArea();
    };


    class Point2D
    {
    public:

      Point2D();
      Point2D(int row, int col);

      int row;
      int col;

      Point2D operator+ (Point2D p);
      Point2D operator- (Point2D p);
      Point2D operator= (Point2D p);
      Point2D operator= (Rect r);

    };

    class ImageRepresentation  
    {
    public:

      ImageRepresentation(unsigned char* image, Size imagSize);
      ImageRepresentation(unsigned char* image, Size imagSize, Rect imageROI);
      void defaultInit(unsigned char* image, Size imageSize);
      virtual ~ImageRepresentation();

      int getSum(Rect imageROI);
      float getMean(Rect imagROI);
      unsigned int getValue(Point2D position);
      Size getImageSize(void){return m_imageSize;};
      Rect getImageROI(void){return m_ROI;};
      void setNewImage(unsigned char* image);
      void setNewROI(Rect ROI);
      void setNewImageSize( Rect ROI );
      void setNewImageAndROI(unsigned char* image, Rect ROI);
      float getVariance(Rect imageROI);
      long getSqSum(Rect imageROI);
      bool getUseVariance(){return m_useVariance;};
      void setUseVariance(bool useVariance){ this->m_useVariance = useVariance; };


    private:

      bool m_useVariance;
      void createIntegralsOfROI(unsigned char* image);

      Size m_imageSize;
      __uint32* intImage;
      __uint64* intSqImage;
      Rect m_ROI;
      Point2D m_offset;
    };

    class Patches
    {
    public:

      Patches();
      Patches(int num);
      virtual ~Patches(void);

      virtual Rect getRect(int index);
      virtual Rect getSpecialRect(const char* what);
      virtual Rect getSpecialRect(const char* what, Size patchSize);

      virtual Rect getROI();
      virtual int getNum(void){return num;};

      int checkOverlap(Rect rect);

      virtual bool isDetection(Rect eval, unsigned char *labeledImg, int imgWidth);
      virtual int getNumPatchesX(){return numPatchesX;}; 
      virtual int getNumPatchesY(){return numPatchesY;};

    protected:

      void setCheckedROI(Rect imageROI, Rect validROI);

      Rect* patches;
      int num;
      Rect ROI;
      int numPatchesX; 
      int numPatchesY;
    };

    class PatchesRegularScan : public Patches
    {
    public:

      PatchesRegularScan(Rect imageROI, Size patchSize, float relOverlap);
      PatchesRegularScan(Rect imageROI, Rect validROI, Size patchSize, float relOverlap);
      virtual ~PatchesRegularScan (void);

      Rect getSpecialRect(const char* what);
      Rect getSpecialRect(const char* what, Size patchSize);
      Size getPatchGrid(){return m_patchGrid;};

    private:

      void calculatePatches(Rect imageROI, Rect validROI, Size patchSize, float relOverlap);

      Rect m_rectUpperLeft;
      Rect m_rectUpperRight;
      Rect m_rectLowerLeft;
      Rect m_rectLowerRight;
      Size m_patchGrid;

    };

    class PatchesRegularScaleScan : public Patches
    {
    public:

      PatchesRegularScaleScan (Rect imageROI, Size patchSize, float relOverlap, float scaleStart, float scaleEnd, float scaleFactor);
      PatchesRegularScaleScan (Rect imageROI, Rect validROI, Size patchSize, float relOverlap, float scaleStart, float scaleEnd, float scaleFactor);
      virtual ~PatchesRegularScaleScan();

      Rect getSpecialRect (const char* what);
      Rect getSpecialRect (const char* what, Size patchSize);

    private:

      void calculatePatches (Rect imageROI, Rect validROI, Size patchSize, float relOverlap, float scaleStart, float scaleEnd, float scaleFactor);

    };

    class PatchesFunctionScaleScan : public Patches
    {
    public:

      typedef float (*GetScale)(int, int);

      PatchesFunctionScaleScan (Rect imageROI, Size patchSize, float relOverlap, GetScale getScale);
      PatchesFunctionScaleScan (Rect imageROI, Rect validROI, Size PatchSize, float relOverlap, GetScale getScale);
      PatchesFunctionScaleScan (Rect imageROI, Size patchSize, float relOverlap, float coefY, float coef1, float minScaleFactor=1.0f);
      PatchesFunctionScaleScan (Rect imageROI, Rect validROI, Size patchSize, float relOverlap, float coefY, float coef1, float minScaleFactor = 1.0f);
      virtual ~PatchesFunctionScaleScan();

      Rect getSpecialRect (const char* what);
      Rect getSpecialRect (const char* what, Size patchSize);

    private:

      void calculatePatches (Rect imageROI, Rect validROI, Size patchSize, float relOverlap, GetScale getScale);
      void calculatePatches (Rect imageROI, Rect validROI, Size patchSize, float relOverlap, float coefY, float coef1, float minScaleFactor);

      Rect rectUpperLeft;
      Rect rectUpperRight;
      Rect rectLowerLeft;
      Rect rectLowerRight;
    };

    class PatchesManualSet : public Patches
    {
    public:

      PatchesManualSet(int numPatches, Rect* patches);
      PatchesManualSet(int numPatches, Rect* patches, Rect ROI);
      virtual ~PatchesManualSet (void);

      Rect getSpecialRect (const char* what){return Rect(-1,-1,-1,-1);} ;
      Rect getSpecialRect (const char* what, Size patchSize){return Rect(-1,-1,-1,-1);};
    };

    //#if OS_type==2
    //#define round(x) ( (x) >= 0 ? (x)+0.5 : (x)-0.5 )
    //#define min(x,y) (x > y ? y : x)
    //#define max(x,y) (x > y ? x : y)

    //#define snprintf _snprintf
    //#endif

    class EstimatedGaussDistribution  
    {
    public:

      EstimatedGaussDistribution();
      EstimatedGaussDistribution(float P_mean, float R_mean, float P_sigma, float R_sigma);
      virtual ~EstimatedGaussDistribution();

      void update(float value); //, float timeConstant = -1.0);

      float getMean(){return m_mean;};
      float getSigma(){return m_sigma;};
      void setValues(float mean, float sigma);

    private:

      float m_mean;
      float m_sigma;
      float m_P_mean;
      float m_P_sigma;
      float m_R_mean;
      float m_R_sigma;
    };

    class FeatureHaar
    {

    public:

      FeatureHaar(Size patchSize);
      virtual ~FeatureHaar();

      void getInitialDistribution(EstimatedGaussDistribution *distribution);

      bool eval(ImageRepresentation* image, Rect ROI, float* result); 

      float getResponse(){return m_response;};

      int getNumAreas(){return m_numAreas;};
      int* getWeights(){return m_weights;};
      Rect* getAreas(){return m_areas;};

    private:

      char m_type[20];
      int m_numAreas;
      int* m_weights;
      float m_initMean;
      float m_initSigma;

      void generateRandomFeature(Size imageSize);
      Rect* m_areas;     // areas within the patch over which to compute the feature
      Size m_initSize;   // size of the patch used during training
      Size m_curSize;    // size of the patches currently under investigation
      float m_scaleFactorHeight;  // scaling factor in vertical direction
      float m_scaleFactorWidth;   // scaling factor in horizontal direction
      Rect* m_scaleAreas;// areas after scaling
      float* m_scaleWeights; // weights after scaling
      float m_response;

    };

    class ClassifierThreshold 
    {
    public:

      ClassifierThreshold();
      virtual ~ClassifierThreshold();

      void update(float value, int target);
      int eval(float value);

      void* getDistribution(int target);

    private:

      EstimatedGaussDistribution* m_posSamples;
      EstimatedGaussDistribution* m_negSamples;

      float m_threshold;
      int m_parity;
    };

    class WeakClassifier  
    {

    public:

      WeakClassifier();
      virtual ~WeakClassifier();

      virtual bool update(ImageRepresentation* image, Rect ROI, int target);

      virtual int eval(ImageRepresentation* image, Rect ROI);

      virtual float getValue (ImageRepresentation* image, Rect  ROI);

      virtual int getType();

    };

    class WeakClassifierHaarFeature : public WeakClassifier
    {

    public:

      WeakClassifierHaarFeature(Size patchSize);
      virtual ~WeakClassifierHaarFeature();

      bool update(ImageRepresentation* image, Rect ROI, int target); 

      int eval(ImageRepresentation* image, Rect ROI); 

      float getValue(ImageRepresentation* image, Rect ROI);

      int getType(){return 1;};

      EstimatedGaussDistribution* getPosDistribution();
      EstimatedGaussDistribution* getNegDistribution();

      void resetPosDist();
      void initPosDist();

    private:

      FeatureHaar* m_feature;
      ClassifierThreshold* m_classifier;

      void generateRandomClassifier();

    };

    class BaseClassifier  
    {
    public:

      BaseClassifier(int numWeakClassifier, int iterationInit, Size patchSize); 
      BaseClassifier(int numWeakClassifier, int iterationInit, WeakClassifier** weakClassifier); 

      virtual ~BaseClassifier();

      void trainClassifier(ImageRepresentation* image, Rect ROI, int target, float importance, bool* errorMask); 

      void getErrorMask(ImageRepresentation* image, Rect ROI, int target, bool* errorMask); 
      void getErrors(float* errors);
      virtual int selectBestClassifier(bool* errorMask, float importance, float* errors); 

      virtual int replaceWeakestClassifier(float* errors, Size patchSize);
      virtual float getError(int curWeakClassifier = -1);

      void replaceClassifierStatistic(int sourceIndex, int targetIndex);

      int eval(ImageRepresentation* image, Rect ROI); 

      float getValue(ImageRepresentation *image, Rect ROI, int weakClassifierIdx = -1);

      WeakClassifier** getReferenceWeakClassifier(){return weakClassifier;};
      void setReferenceWeakClassifier(WeakClassifier** weakClassifier){this->weakClassifier = weakClassifier;};

      int getNumWeakClassifier(){return m_numWeakClassifier;};
      int getIterationInit(){return m_iterationInit;};
      float getWCorrect(){return m_wCorrect[m_selectedClassifier];};
      float getWWrong(){return m_wWrong[m_selectedClassifier];};
      void setWCorrect(int idx, float value){ if(idx < m_numWeakClassifier) m_wCorrect[idx] = value; };
      void setWWrong(int idx, float value){ if(idx < m_numWeakClassifier) m_wWrong[idx] = value; };

      int getTypeOfSelectedClassifier(){return weakClassifier[m_selectedClassifier]->getType();};
      int getIdxOfSelectedClassifier(){return m_selectedClassifier;};
      int getIdxOfNewWeakClassifier(){return m_idxOfNewWeakClassifier;};

    protected:

      WeakClassifier** weakClassifier;
      bool m_referenceWeakClassifier;
      int m_numWeakClassifier;
      int m_selectedClassifier;
      int m_idxOfNewWeakClassifier;
      float* m_wCorrect;
      float* m_wWrong;
      int m_iterationInit;
      void generateRandomClassifier (Size patchSize);

    };

    class StrongClassifier  
    {
    public:

      StrongClassifier( int numBaseClassifier, 
        int numWeakClassifier, 
        Size patchSize, 
        bool useFeatureExchange = false, 
        int iterationInit = 0);

      ~StrongClassifier();

      virtual float eval(ImageRepresentation *image, Rect ROI); 

      virtual bool update(ImageRepresentation *image, Rect ROI, int target, float importance = 1.0f); 
      virtual bool updateSemi(ImageRepresentation *image, Rect ROI, float priorConfidence);

      Size getPatchSize(){return patchSize;};
      int getNumBaseClassifier(){return numBaseClassifier;};
      int getIdxOfSelectedClassifierOfBaseClassifier (int baseClassifierIdx=0){return baseClassifier[baseClassifierIdx]->getIdxOfSelectedClassifier();};
      virtual float getSumAlpha(int toBaseClassifier = -1);
      float getAlpha(int idx){return alpha[idx];};

      float getFeatureValue(ImageRepresentation *image, Rect ROI, int baseClassifierIdx);
      float getImportance(ImageRepresentation *image, Rect ROI, int traget, int numBaseClassifiers = -1);

      WeakClassifier** getReferenceWeakClassifier(){return baseClassifier[0]->getReferenceWeakClassifier();};

      void resetWeightDistribution();

    protected:

      int numBaseClassifier;
      int numAllWeakClassifier;

      BaseClassifier** baseClassifier;
      float* alpha;
      Size patchSize;

      bool useFeatureExchange;

    };

    class StrongClassifierDirectSelection : public StrongClassifier
    {
    public:

      StrongClassifierDirectSelection(int numBaseClassifier, int numWeakClassifier, Size patchSize, bool useFeatureExchange = false, int iterationInit = 0); 

      virtual ~StrongClassifierDirectSelection();

      bool update(ImageRepresentation *image, Rect ROI, int target, float importance = 1.0); 

    private:

      bool * m_errorMask;
      float* m_errors;
      float* m_sumErrors;
    };

    class StrongClassifierStandard  : public StrongClassifier
    {
    public:

      StrongClassifierStandard(int numBaseClassifier, int numWeakClassifier,
        Size patchSize, bool useFeatureExchange = false, int iterationInit = 0); 

      virtual ~StrongClassifierStandard();

      bool update(ImageRepresentation *image, Rect ROI, int target, float importance = 1.0);

    private:

      bool *m_errorMask;
      float *m_errors;

    };

    class StrongClassifierStandardSemi  : public StrongClassifier
    {
    public:

      StrongClassifierStandardSemi(int numBaseClassifier, int numWeakClassifier, 
        Size patchSize,  bool useFeatureExchange = false, int iterationInit = 0);

      virtual ~StrongClassifierStandardSemi();

      bool updateSemi(ImageRepresentation *image, Rect ROI, float priorConfidence);
      void getPseudoValues(ImageRepresentation *image, Rect ROI, float priorConfidence, float* pseudoLambdaInOut, int* pseudoTargetInOut);

    private:

      bool *m_errorMask;
      float *m_errors;
      float* m_pseudoLambda;
      int* m_pseudoTarget;

    };

    class Detector
    {
    public:

      Detector(StrongClassifier* classifier);
      virtual ~Detector(void);

      void classify(ImageRepresentation* image, Patches* patches, float minMargin = 0.0f);
      void classify(ImageRepresentation* image, Patches* patches, float minMargin, float minVariance );

      void classifySmooth(ImageRepresentation* image, Patches* patches, float minMargin = 0);

      int getNumDetections();
      float getConfidence(int patchIdx);
      float getConfidenceOfDetection (int detectionIdx);


      float getConfidenceOfBestDetection (){return m_maxConfidence;};
      int getPatchIdxOfBestDetection();

      int getPatchIdxOfDetection(int detectionIdx);

      int* getIdxDetections(){return m_idxDetections;};
      float* getConfidences(){return m_confidences;};

      IplImage* getConfImageDisplay() const { return m_confImageDisplay; }

    private:

      void prepareConfidencesMemory(int numPatches);
      void prepareDetectionsMemory(int numDetections);

      StrongClassifier* m_classifier;
      float* m_confidences;
      int m_sizeConfidences;
      int m_numDetections;
      int* m_idxDetections;
      int m_sizeDetections;
      int m_idxBestDetection;
      float m_maxConfidence;
      CvMat *m_confMatrix;
      CvMat *m_confMatrixSmooth;
      IplImage* m_confImageDisplay;
    };


    /** The main Online Boosting tracker class */
    class BoostingTracker
    {
    public:
      BoostingTracker(ImageRepresentation* image, Rect initPatch, Rect validROI, int numBaseClassifier);
      virtual ~BoostingTracker();

      bool track(ImageRepresentation* image, Patches* patches);

      Rect getTrackingROI(float searchFactor);
      float getConfidence();
      Rect getTrackedPatch();
      Point2D getCenter();
      IplImage* getConfImageDisplay() const { return detector->getConfImageDisplay(); }

    private:
      StrongClassifier* classifier;
      Detector* detector;
      Rect validROI;
      Rect trackedPatch;
      float confidence;
      Point2D dxDy;
    };

    class SemiBoostingTracker
    {
    public:
      SemiBoostingTracker(ImageRepresentation* image, Rect initPatch, Rect validROI, int numBaseClassifier);
      virtual ~SemiBoostingTracker();

      bool track(ImageRepresentation* image, Patches* patches);

      Rect getTrackingROI(float searchFactor);
      float getConfidence();
      float getPriorConfidence();
      Rect getTrackedPatch();
      Point2D getCenter();
      IplImage* getConfImageDisplay() const { return detector->getConfImageDisplay(); }

    private:
      StrongClassifier* classifierOff;
      StrongClassifierStandardSemi* classifier;
      Detector* detector;
      Rect trackedPatch;
      Rect validROI;
      float confidence;
      float priorConfidence;
    };

  }
}

#endif  // #ifndef __OPENCV_ONLINE_BOOSTING_H__

/* End of file. */