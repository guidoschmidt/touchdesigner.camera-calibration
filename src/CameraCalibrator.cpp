#include "CameraCalibrator.hpp"

CameraCalibrator::CameraCalibrator() {
  // Checkerboard parameters
  this->_imageCount                  = 1;
  this->_images                      = {};
  this->_imageFiles                  = {};
  this->_imageSize                   = cv::Size(1920, 1080);
  this->_boardSize                   = cv::Size(9, 6);
  this->_squareSizeInCm              = 5;
  this->_findCalibrationPatternFlags =
    cv::CALIB_CB_ADAPTIVE_THRESH |
    cv::CALIB_CB_FAST_CHECK      |
    cv::CALIB_CB_NORMALIZE_IMAGE;

  // Measurements
  this->_inputImages                  = {};
  this->_pointsFromCheckerboardImages = {};
  this->_measurementPointList         = {};
  this->_referencePointList           = {};

  // Calibration parameters
  this->_rotationVectors             = {};
  this->_translationVectors          = {};
  this->_reprojectionErrors          = {};
  this->_cameraMatrix                = cv::Mat::eye(3, 3, CV_64F);
  this->_distortionCoefficientMatrix = cv::Mat::zeros(8, 1, CV_64F);
  this->_performCalibrationFlags     =
    // CV_CALIB_FIX_K1 |
	cv::CALIB_FIX_K4 |
    cv::CALIB_FIX_K5;
    // CV_CALIB_FIX_K6;
    // CV_CALIB_FIX_ASPECT_RATIO;

  // CV_CALIB_USE_INTRINSIC_GUESS
  // CV_CALIB_FIX_PRINCIPAL_POINT
  // CALIB_FIX_S1_S2_S3_S4
}

CameraCalibrator::~CameraCalibrator() {
  
}

void CameraCalibrator::addImageFile(std::string filePath)
{
  this->_imageFiles.push_back(filePath);
}

void CameraCalibrator::evaluateImageStack()
{
  std::vector<cv::Point3f> checkerboardPoints = {};
  checkerboardPoints = this->calculateChessboardReferencePoints();
  std::cout << "Evaluation: using " << this->_imageFiles.size() << " images." << std::endl;
  for (unsigned int i = 0; i < this->_imageFiles.size(); i++) {
    std::vector<cv::Point2f> foundPointsBuffer = {};
    /// Read input
    std::string filename = this->_imageFiles[i]; 
    std::cout << "Evaluation: " << filename;
    cv::Mat image = cv::imread(filename);
    this->_images.push_back(image);
    /// Find chessboard
    cv::findChessboardCorners(image,
                              this->_boardSize,
                              foundPointsBuffer,
                              this->_findCalibrationPatternFlags);
    cv::drawChessboardCorners(image,
                              this->_boardSize,
                              cv::Mat(foundPointsBuffer),
                              true);
    std::cout << foundPointsBuffer.size() << std::endl;
    if (foundPointsBuffer.size() == 0) { continue; }
    this->_measurementPointList.push_back(foundPointsBuffer);
    this->_referencePointList.push_back(checkerboardPoints);
    this->_imageSize = cv::Size(image.rows, image.cols);
  }
  for (int i = 0; i < this->_measurementPointList.size(); ++i) {
    this->_referencePointList[i].resize(_measurementPointList[i].size(),
                                        _referencePointList[i][0]);
  }
}

std::vector<cv::Point3f> CameraCalibrator::calculateChessboardReferencePoints()
{
  std::vector<cv::Point3f> cornerPoints = {};
  for(unsigned int i = 0; i < this->_boardSize.width; ++i) {
    for(unsigned int j = 0; j < this->_boardSize.width; ++j) {
      // When using using a planar calibration pattern, we can omit z direction
      cv::Point3f point = cv::Point3f(j * this->_squareSizeInCm,
                                      i * this->_squareSizeInCm,
                                      0);
      cornerPoints.push_back(point);
    }
  }
  return cornerPoints;
}

void CameraCalibrator::performCalibration()
{
  std::cout << "Calibrating ...";
  double reprojectionError = cv::calibrateCamera(this->_referencePointList,
                                                 this->_measurementPointList,
                                                 this->_imageSize,
                                                 this->_cameraMatrix,
                                                 this->_distortionCoefficientMatrix,
                                                 this->_rotationVectors,
                                                 this->_translationVectors);
  cv::calibrationMatrixValues(this->_cameraMatrix,
                              this->_imageSize,
                              this->_aperture[0],
                              this->_aperture[1],
                              this->_fieldOfViewInDegrees[0],
                              this->_fieldOfViewInDegrees[1],
                              this->_focalLength,
                              this->_principalPointInMilliMeter,
                              this->_aspectRatio);
  std::cout << "Calibrated Successfully." << std::endl;
}

cv::Mat CameraCalibrator::undistorImage(cv::Mat image)
{
  cv::Mat imageUndistorted = image.clone();
  cv::undistort(image, imageUndistorted,
                this->_cameraMatrix,
                this->_distortionCoefficientMatrix);
  cv::imshow("Undistorted image:", imageUndistorted);
  cv::waitKey();
  return imageUndistorted;
}

cv::Vec2d CameraCalibrator::getAperture()
{
  return this->_aperture;
}

cv::Vec2d CameraCalibrator::getFieldOfViewInDegrees()
{
  return this->_fieldOfViewInDegrees;
}

double CameraCalibrator::getFocalLength()
{
  return this->_focalLength;
}

cv::Point2d CameraCalibrator::getPrincipalPointInMillimeter()
{
  return this->_principalPointInMilliMeter;
}

double CameraCalibrator::getAspectRatio()
{
  return this->_aspectRatio;
}

std::vector<cv::Mat> CameraCalibrator::getRotationVectors()
{
  return this->_rotationVectors;
}

std::vector<cv::Mat> CameraCalibrator::getTranslationVectors()
{
  return this->_translationVectors;
}

std::vector<float> CameraCalibrator::getReprojectionErrors()
{
  return this->_reprojectionErrors;
}

cv::Mat CameraCalibrator::getDistortionCoefficientMatrix()
{
  std::cout << this->_distortionCoefficientMatrix << std::endl;
  return this->_distortionCoefficientMatrix;
}

cv::Mat CameraCalibrator::getCameraMatrix()
{
  return this->_cameraMatrix;
}
