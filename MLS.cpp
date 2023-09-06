/*
 *
 */

#include "std.h"
using namespace cv;

#include "MLS.h"
#include "Curve.h"

MLS<double> smls;
Mat visualized_curve;
vector<Point> target_curve;
int mls_def_type;

void MLSUpdate() {
	static int framenum = 0;
	
	if (mls_def_type == 0) {
		smls.UpdateAffine();
	} else if (mls_def_type == 1) {
		smls.UpdateSimilarity();
	} else {
		smls.UpdateRigid();
	}

	visualized_curve.setTo(0);
	smls.Draw(visualized_curve);
	imshow("MLS", visualized_curve);
	
//	stringstream ss; ss << "deformed/i" << framenum++ << ".jpg";
//	char buf[255];
//	sprintf(buf, "deformed/i%05d.jpg",framenum++);
//	imwrite(buf, visualized_curve);
	
	waitKey(1);			
}	

void onMouse( int event, int x, int y, int flags, void* )
{
	static Point2d start_touch(-1,-1);
	static Point2d last_touch(-1,-1);
	static int touch_control_point = -1;
	
	Point2d touch(x,y);
    if( event == CV_EVENT_LBUTTONDOWN ) {
		//		cout << "mouse down\n";
		start_touch = last_touch = touch;
		const vector<Point2d>& ctrl_pts = smls.GetDeformedControlPts();
		for (int i=0; i<ctrl_pts.size(); i++) {
			if (norm(touch - ctrl_pts[i]) < 10) {
				//touching point i
				touch_control_point = i;
			}
		}
		if (touch_control_point >= 0) {
			cout << "selected point " << touch_control_point << endl;
		}
	} else if( event == CV_EVENT_LBUTTONUP ) {
		//		cout << "mouse up\n";
		touch_control_point = -1;
	} else if (event == CV_EVENT_MOUSEMOVE && flags == CV_EVENT_FLAG_LBUTTON) {
		if (touch_control_point >= 0) {
			//			cout << "mouse drag\n";
			vector<Point2d>& def_ctrl_pts = smls.GetDeformedControlPts();
			def_ctrl_pts[touch_control_point] += touch - last_touch;
			last_touch = touch;
			
			MLSUpdate();
		}
	}
	
}

void onTrackbar(int, void*)
{
	MLSUpdate();
}

void MLSDeformCurve(const Mat& src, 
					const vector<Point2d>& a_p2d,
					const vector<pair<char,int> >& stringrep
					) 	
{
	//Get extrema as control points
	vector<int> control_pts; 
	for(int i=0;i<stringrep.size();i++) {
		control_pts.push_back(stringrep[i].second);
	}
	
	
	smls.Init(a_p2d, control_pts);
	smls.UpdateRigid();
	
	visualized_curve.create(src.size(),CV_8UC3);
	visualized_curve.setTo(0);
	smls.Draw(visualized_curve);

	namedWindow("MLS");
	setMouseCallback("MLS", onMouse, NULL);
	createTrackbar("Def. type:", "MLS", &mls_def_type, 2, onTrackbar, NULL);
	imshow("MLS", visualized_curve);
	waitKey();
}


int main(int argc, char** argv) {
	Mat src1 = imread("../fish-12.png");
	if (src1.empty()) {
		cerr << "can't read image" << endl; exit(0);
	}


/*    std::vector<cv::Mat> channels;
    cv::Mat hsv;
    cv::cvtColor( src1, hsv, CV_BGR2GRAY );
    cv::split(hsv, channels);
    Mat gray_image = channels[0];
    imwrite("../img1.png", gray_image);
    cv::Mat contours;
    cv::Canny(gray_image,contours,35,90);
    imwrite("../img2.png", contours);

    Mat src2 = imread("../img2.png");*/

	vector<Point> a;
	GetCurveForImage(src1, a, false);
	
	//move curve a bit to the middle, and scale up
	cv::transform(a,a,getRotationMatrix2D(Point2f(0,0),0,1.3));
//	Mat tmp_curve_m(a); tmp_curve_m += Scalar(100,95);
	
	vector<Point2d> a_p2d, a_p2d_smoothed;
	ConvertCurve(a, a_p2d);

	//Get curvature extrema points
	vector<pair<char,int> > stringrep = CurvatureExtrema(a_p2d, a_p2d_smoothed,0.05,4.0);

	//Start interactive deformation
	src1.create(Size(700,600), CV_8UC3);
	MLSDeformCurve(src1,a_p2d,stringrep);
}