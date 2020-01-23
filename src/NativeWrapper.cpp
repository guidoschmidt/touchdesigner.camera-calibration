// #include "NativeWrapper.hpp"

// void AddImageFile(char* imageFilePath)
// {
//   camCalib->addImageFile(imageFilePath);
// }

// void EvaluateImageStack()
// {
//   camCalib->evaluateImageStack();
// }

// void PerformCalibration()
// {
//   camCalib->performCalibration(); 
// }

// void GetCameraMatrix(float matrix[])
// {
//   cv::Mat cameraMatrix = camCalib->getCameraMatrix();
//   matrix[0] = static_cast<float>(cameraMatrix.at<double>(0, 0));
//   matrix[1] = static_cast<float>(cameraMatrix.at<double>(1, 0));
//   matrix[2] = static_cast<float>(cameraMatrix.at<double>(2, 0));
//   matrix[3] = static_cast<float>(cameraMatrix.at<double>(0, 1));
//   matrix[4] = static_cast<float>(cameraMatrix.at<double>(1, 1));
//   matrix[5] = static_cast<float>(cameraMatrix.at<double>(2, 1));
//   matrix[6] = static_cast<float>(cameraMatrix.at<double>(0, 2));
//   matrix[7] = static_cast<float>(cameraMatrix.at<double>(1, 2));
//   matrix[8] = static_cast<float>(cameraMatrix.at<double>(2, 2));
// }

// void GetAperture(float vector[])
// {
//   vector[0] = camCalib->getAperture()[0];
//   vector[1] = camCalib->getAperture()[1];
// }

// void GetDistortionCoefficients(float vector[])
// {
//   cv::Mat coefficientMatrix = camCalib->getDistortionCoefficientMatrix();
//   std::cout << coefficientMatrix << std::endl;
//   vector[0] = static_cast<float>(coefficientMatrix.at<double>(0, 0));
//   vector[1] = static_cast<float>(coefficientMatrix.at<double>(1, 0));
//   vector[2] = static_cast<float>(coefficientMatrix.at<double>(2, 0));
//   vector[3] = static_cast<float>(coefficientMatrix.at<double>(3, 0));
// }

// void GetFieldOfViewInDegree(float vector[])
// {
//   vector[0] = camCalib->getFieldOfViewInDegrees()[0]; 
//   vector[1] = camCalib->getFieldOfViewInDegrees()[1]; 
// }

// double GetFocalLength()
// {
//   return camCalib->getFocalLength();
// }
