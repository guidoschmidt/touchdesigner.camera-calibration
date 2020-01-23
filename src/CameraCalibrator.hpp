#include <opencv2/opencv.hpp>
#include <vector>

class CameraCalibrator {
private:
	// Checkerboard parameters
	int                      _imageCount;
	std::vector<cv::Mat>     _images;
	std::vector<std::string> _imageFiles;
	cv::Size                 _imageSize;
	cv::Size                 _boardSize;
	int                      _squareSizeInCm;
	bool                     _findCalibrationPatternFlags;
	bool                     _isCalibrated;

	// Measurements
	std::vector<cv::Mat>                  _inputImages;
	std::vector<cv::Point2f>              _pointsFromCheckerboardImages;
	std::vector<std::vector<cv::Point2f>> _measurementPointList;
	std::vector<std::vector<cv::Point3f>> _referencePointList;

	// Calibration parameters
	std::vector<cv::Mat> _rotationVectors;
	std::vector<cv::Mat> _translationVectors;
	std::vector<float>   _reprojectionErrors;
	cv::Mat              _cameraMatrix;
	cv::Mat              _distortionCoefficientMatrix;
	bool                 _performCalibrationFlags;
	cv::Vec2d            _aperture;
	cv::Vec2d            _fieldOfViewInDegrees;
	double               _focalLength;
	cv::Point2d          _principalPointInMilliMeter;
	double               _aspectRatio;

	std::vector<cv::Point3f> calculateChessboardReferencePoints();

public:
	CameraCalibrator();
	~CameraCalibrator();

	// Algorithmic
	void                 addImageFile(std::string filePath);
	void                 evaluateImageStack();
	void                 getCameraIntrinsics();
	void                 performCalibration();
	void                 clearImageFiles();
	cv::Mat              undistorImage(cv::Mat image);

	// Getter
	cv::Vec2d                getAperture();
	cv::Vec2d                getFieldOfViewInDegrees();
	double                   getFocalLength();
	cv::Point2d              getPrincipalPointInMillimeter();
	double                   getAspectRatio();
	cv::Mat                  getCameraMatrix();
	std::vector<cv::Mat>     getRotationVectors();
	std::vector<cv::Mat>     getTranslationVectors();
	std::vector<float>       getReprojectionErrors();
	cv::Mat                  getDistortionCoefficientMatrix();
	std::vector<std::string> getImageFiles();
	inline bool              isCalibrated() { return _isCalibrated; }
};
