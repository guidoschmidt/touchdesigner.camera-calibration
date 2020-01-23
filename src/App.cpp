#include <iostream>
#include <future>
#include <thread>

#include "CameraCalibrator.hpp"

void execute_async(CameraCalibrator* cc) {
  cc->evaluateImageStack();
  cc->performCalibration();
}

int main() {
  CameraCalibrator camera_calibrator;
  camera_calibrator.addImageFile("../img/series-0/image-01.jpg");
  camera_calibrator.addImageFile("../img/series-0/image-02.jpg");
  camera_calibrator.addImageFile("../img/series-0/image-03.jpg");
  camera_calibrator.addImageFile("../img/series-0/image-04.jpg");

  auto thread = std::thread(execute_async, &camera_calibrator);
  thread.join();

  std::cout << camera_calibrator.getFocalLength() << std::endl;
  std::cout << camera_calibrator.getPrincipalPointInMillimeter() << std::endl;

  return 0;
}
