#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
//#include <stdio.h>
//#include <stdlib.h>
#include <io.h>

using namespace cv;
using namespace std;

Mat src;
Mat src_gray;
Mat roi,rom,roi_grey,roi_singl_big;
int thresh = 60;
int max_thresh = 255;
RNG rng(12345);
_finddata_t danePliku;
int licz = 0;


///**********************************************************************

template <typename T>  string NumberToString ( T Number ) //  int to string
{
    ostringstream ss;
    ss << Number;
    return ss.str();
}


///**************************************** MAIN **********************************************************************************
int main( int argc, char** argv )
{

///make Folder
    string folderCreateCommand = "mkdir br";
    system(folderCreateCommand.c_str());
    folderCreateCommand = "mkdir gr";
    system(folderCreateCommand.c_str());
    folderCreateCommand = "mkdir grbrtmp";
    system(folderCreateCommand.c_str());

///*** BANCH MODE - Find files and processing
    long uchwyt = _findfirst( "*.png", & danePliku );
    if( (uchwyt = _findfirst( "*.png", &danePliku )) == -1L )
        cout<< "No file with good extention\n" ;
    else
    {
        do
        {
            cout<< danePliku.name<< "\n";
            src = imread(danePliku.name);
            if ( !src.data )
            {
                cout << "No File! \n. ";
                return -1;
            }

            /// Convert image to gray and blur it -  1
            cvtColor( src, src_gray, CV_BGR2GRAY );
            blur( src_gray, src_gray, Size(3,3) );

            Mat threshold_output;
            vector<vector<Point> > contours;
            vector<Vec4i> hierarchy;

            cout<< "Szukanie konturow - calosc..."<<endl;
            /// Detect edges using Threshold
            threshold( src_gray, threshold_output, thresh, 255, THRESH_BINARY );

            //morphological opening (remove small objects from the foreground)
            erode(threshold_output, threshold_output, getStructuringElement(MORPH_OPEN, Size(9, 9)) );
            dilate( threshold_output, threshold_output, getStructuringElement(MORPH_OPEN, Size(9, 9)) );

            //morphological closing (fill small holes in the foreground)
            //dilate( threshold_output, threshold_output, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)) );
            //erode(threshold_output, threshold_output, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)) );

            /*namedWindow("morfologia",CV_WINDOW_NORMAL);
            resizeWindow("morfologia",threshold_output.cols/4,threshold_output.rows/4);
            imshow("morfologia",threshold_output);*/

            cout<< "Elipsy"<<endl;

            /// Find contours
            findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

            /// Find the rotated rectangles and ellipses for each contour
            vector<RotatedRect> minRect( contours.size() );
            vector<RotatedRect> minEllipse( contours.size() );

            for( int i = 0; i < contours.size(); i++ )
            {
                minRect[i] = minAreaRect( Mat(contours[i]) );
                if( contours[i].size() > 75 )
                {
                    minEllipse[i] = fitEllipse( Mat(contours[i]) );
                }
            }

            cout<< "Obrot"<<endl;
/// Contours + rotated rects + ellipses
            //Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
            for( int i = 0; i< contours.size(); i++ )
            {
                if(contourArea( contours[i],false) > 600)
                {
                    roi =src( boundingRect(contours[i])); // set ROI
                    cout<< ".";
                    copyMakeBorder( roi, roi, 50, 50, 50, 50,BORDER_CONSTANT, 0 );
                    rom = getRotationMatrix2D(Point( roi.cols/2., roi.rows/2. ), minEllipse[i].angle, 1.0 );
                    warpAffine(roi, roi, rom, roi.size(), cv::INTER_CUBIC);  // perform the affine transformation


///Visualisation countour
                    /*
                    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
                    // contour
                    drawContours( drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
                    // ellipse
                    //minEllipse[i].angle = 0; // angle test
                    ellipse( drawing, minEllipse[i], color, 2, 8 );
                    // rotated rectangle
                    Point2f rect_points[4]; minRect[i].points( rect_points );
                    for( int j = 0; j < 4; j++ )
                     line( drawing, rect_points[j], rect_points[(j+1)%4], color, 1, 8 );
                    //txt
                    //putText(drawing, NumberToString(topsize), Point(minEllipse[i].center.x + minEllipse[i].size.width/4 +20, minEllipse[i].center.y + minEllipse[i].size.height/2-100),FONT_HERSHEY_DUPLEX, 0.4, cvScalar(0,0,200), 1, CV_AA);
                    //putText(drawing, NumberToString(downsize), Point(minEllipse[i].center.x + minEllipse[i].size.width/4 +20, minEllipse[i].center.y + minEllipse[i].size.height/2-50),FONT_HERSHEY_DUPLEX, 0.4, cvScalar(0,250,0), 1, CV_AA);
                    */

///Show detect results
                    /* namedWindow("morfologia",CV_WINDOW_NORMAL);
                     resizeWindow("morfologia",threshold_output.cols/1,threshold_output.rows/1);
                     imshow("morfologia",drawing);
                    */
//Find the biggest countours ****
                    int thresh = 60;
                    /// Convert image to gray and blur it
                    cvtColor( roi, roi_grey, CV_BGR2GRAY );
                    blur( roi_grey, roi_grey, Size(3,3) );

                    Mat threshold_roi;
                    vector<vector<Point> > contours_roi_big;
                    vector<Vec4i> hierarchy_roi_big;

                    /// Detect edges using Threshold
                    threshold( roi_grey, threshold_roi, thresh, 255, THRESH_BINARY );
                    /// Find contours
                    findContours( threshold_roi, contours_roi_big, hierarchy_roi_big, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
                    int bigest=0;
                    int which_bigest=0;

                    for( int j = 0; j< contours_roi_big.size(); j++ )
                    {
                        if(contourArea( contours_roi_big[j],false) > bigest)
                        {
                            bigest = contourArea( contours_roi_big[j],false);
                            which_bigest = j;

                        }

                    }
                    roi_singl_big = roi( boundingRect(contours_roi_big[which_bigest])); // set ROI 2

/// Detection UP / DOWN
                    cvtColor( roi_singl_big, roi_grey, CV_BGR2GRAY );
                    blur( roi_grey, roi_grey, Size(3,3) );

                    threshold( roi_grey, threshold_roi, thresh, 255, THRESH_BINARY );

                    int topsize = 0;
                    int downsize = 0;
                    for( int y = 0; y < threshold_roi.rows/4; y++ )
                    {
                        for( int x = 0; x < threshold_roi.cols; x++ )
                        {
                            if ( threshold_roi.at<uchar>(y,x) == 255 ) topsize++;
                        }
                    }

                    for( int y = threshold_roi.rows - (threshold_roi.rows/4); y < threshold_roi.rows; y++ )
                    {
                        for( int x = 0; x < threshold_roi.cols; x++ )
                        {
                            if ( threshold_roi.at<uchar>(y,x) == 255 ) downsize++;
                        }
                    }

                    if (downsize<topsize)

                    {
                        flip(roi_singl_big,roi_singl_big,0); // FLIP if needed
                        flip(threshold_roi,threshold_roi,0);
                    }

/// Get the moments
                    vector<vector<Point> > contours_roi;
                    vector<Vec4i> hierarchy_roi;
                    findContours( threshold_roi, contours_roi, hierarchy_roi, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
                    bigest=0;
                    which_bigest=0;

                    for( int j = 0; j< contours_roi.size(); j++ )
                    {
                        if(contourArea( contours_roi[j],false) > bigest)
                        {
                            bigest = contourArea( contours_roi[j],false);
                            which_bigest = j;
                        }
                    }

                    vector<Moments> mu(contours_roi.size() );
                    mu[which_bigest] = moments( contours_roi[which_bigest], false );

///  Get the mass centers:
                    vector<Point2f> mc( contours_roi.size() );
                    mc[which_bigest] = Point2f( mu[which_bigest].m10/mu[which_bigest].m00, mu[which_bigest].m01/mu[which_bigest].m00 );
                    //circle( roi, mc[which_bigest], 4, Scalar( 255, 0, 0 ), -1, 8, 0 );
                    //imshow( "CENTER "+NumberToString(which_bigest),  roi);
                    //imshow( "CENTER2 "+NumberToString(which_bigest),  roi_singl_big);



///Valey detect

                    /// Gradient X
                    Mat roi_grey_sob;
                    //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
                    equalizeHist( roi_grey, roi_grey );
                    Sobel( roi_grey, roi_grey_sob, CV_16S, 1, 0, 3, 1, 2, BORDER_DEFAULT );
                    convertScaleAbs( roi_grey_sob, roi_grey_sob );
                    drawContours( roi_grey_sob, contours_roi, which_bigest, (0,0,0), 16, 8, vector<Vec4i>(), 0, Point() );
                    ///ROI AREA

                    threshold( roi_grey_sob, roi_grey_sob, 85, 255, THRESH_BINARY );

                    int valeysize = 0;
                    int center_y = mc[which_bigest].y;
                    int center_x = mc[which_bigest].x;
                    int y1= center_y - 55;
                    int y2= center_y + 55;
                    int x1= center_x - 10;
                    int x2= center_x + 10;
/// DRAWING SEGMENTS
/*
                    circle( roi_singl_big, mc[which_bigest], 2, Scalar( 255, 255, 255 ), -1, 8, 0 );
                    //circle( roi_singl_big, Point2d(center_x , center_y + center_y/2), center_y/2-5, Scalar( 0,255,0 ), 2, 8, 0 );
                    //circle( roi_singl_big, Point2d(center_x , center_y - center_y/2), center_y/2-5, Scalar( 0,0,255 ), 2, 8, 0 );
                    ellipse(roi_singl_big, Point2d(center_x , center_y + center_y/2),Size( center_x/2+5, center_y/2-5 ), 0, 0, 360, Scalar( 0,255, 0 ), 1, 8 );
                    ellipse(roi_singl_big, Point2d(center_x , center_y - center_y/2),Size( center_x/2+5, center_y/2-5 ), 0, 0, 360, Scalar( 0,0, 255 ), 1, 8 );
                    //putText(roi_singl_big, NumberToString(valleylength_big), Point(0, 10),FONT_HERSHEY_DUPLEX, 0.4, cvScalar(255,255,255), 1, CV_AA);
                    rectangle( roi_singl_big,Point( x1,y1 ),Point( x2,y2),Scalar( 255, 0, 0 ), 1, 8 );
*/

                    //cout<<center_x<<endl;
                    //cout<<roi_grey_sob.cols/2<<endl;
//
//HOW MANY Threshold pixels is?
                    for( int y=y1 ; y < y2; y++ )
                    {
                        for( int x= x1; x < x2; x++ )
                        {
                            if ( roi_grey_sob.at<uchar>(y,x) == 255 ) valeysize++;
                        }
                    }
//HOW LONG IS VALEY
                    int valleylength, valleylength_big=0;
                    for( int x= x1; x < x2; x++ )
                    {
                        valleylength = 0 ;
                        for( int y=y1 ; y < y2; y++ )
                        {
                            if ( roi_grey_sob.at<uchar>(y,x) == 255 ) valleylength++;
                        }
                        if (valleylength>valleylength_big) valleylength_big = valleylength;
                    }
                    //cout<<"("<<valleylength_big<<")";



                    //imshow( "ROI TMP"+NumberToString(i),  roi_grey_sob );


///***Valey end


///Normalize Resize
//width //height

                    int diffrent_size_w, diffrent_size_h;
                    int tmp_flag = 0;

                    if (roi_singl_big.cols <= 80 && roi_singl_big.rows <= 170)
                    {
                        diffrent_size_w = 80-roi_singl_big.cols;
                        diffrent_size_h = 170-roi_singl_big.rows;
                        if (diffrent_size_w%2 == 0 && diffrent_size_h%2 == 0)
                            copyMakeBorder( roi_singl_big, roi_singl_big, diffrent_size_h/2,diffrent_size_h/2, diffrent_size_w/2, diffrent_size_w/2,BORDER_CONSTANT,0);
                        else if (diffrent_size_w%2 != 0 && diffrent_size_h%2 != 0)
                            copyMakeBorder( roi_singl_big, roi_singl_big, diffrent_size_h/2+1,diffrent_size_h/2, diffrent_size_w/2 + 1, diffrent_size_w/2,BORDER_CONSTANT,0);
                        else if (diffrent_size_w%2 != 0 && diffrent_size_h%2 == 0)
                            copyMakeBorder( roi_singl_big, roi_singl_big, diffrent_size_h/2,diffrent_size_h/2, diffrent_size_w/2 + 1, diffrent_size_w/2,BORDER_CONSTANT,0);
                        else if (diffrent_size_w%2 == 0 && diffrent_size_h%2 != 0)
                            copyMakeBorder( roi_singl_big, roi_singl_big, diffrent_size_h/2+1,diffrent_size_h/2, diffrent_size_w/2, diffrent_size_w/2,BORDER_CONSTANT,0);
                    }
                    else if (roi_singl_big.cols > 80 || roi_singl_big.rows > 170) tmp_flag = 1;

///WRITE and SORT  DATA
                    //putText(roi_singl_big, NumberToString(valeysize), Point(0, 10),FONT_HERSHEY_DUPLEX, 0.4, cvScalar(255,255,255), 1, CV_AA);
                    string iplik =
                        NumberToString ( valleylength_big )+"_"+NumberToString ( i );
                    //if(valeysize/roi_singl_big.cols > 3) imwrite("br" + iplik + danePliku.name, roi_singl_big );
                    if (tmp_flag ==1)
                    {
                        imwrite("grbrtmp/" + iplik + danePliku.name, roi_singl_big );                // 60 - good
                        //imwrite("grbrtmp/" + iplik + "_" + danePliku.name, roi_grey_sob );  // B&W
                    }
                    else if(valleylength_big > 40)
                    {
                        imwrite("br/" + iplik + danePliku.name, roi_singl_big );                // 60 - good
                        //imwrite("br/" + iplik + "_" + danePliku.name, roi_grey_sob );  // B&W
                    }
                    else
                    {
                        imwrite("gr/" + iplik + danePliku.name, roi_singl_big );
                        //imwrite("gr/" + iplik + "_" + danePliku.name, roi_grey_sob ); //B&W
                    }
                    //namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
                    //imshow( "Contours", drawing );
                    //licz++;
                    // cout << "zapisano "<<licz<< " plikow\n";


///Open small windows with roi obiects
                    //namedWindow( "roi_"+NumberToString(i), CV_WINDOW_AUTOSIZE );
                    //imshow( "roi_"+NumberToString(i), roi_singl_big );

                    //putText(drawing, NumberToString(minEllipse[i].angle), Point(minEllipse[i].center.x + minEllipse[i].size.width/4,minEllipse[i].center.y + minEllipse[i].size.height/2),FONT_HERSHEY_DUPLEX, 0.4, cvScalar(150,150,150), 1, CV_AA);
                }
                else cout<<"/";
            }
            cout<<endl;
            /// Show in a window
            //namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
            //imshow( "Contours", drawing );
            waitKey(0);

        }

        while( _findnext( uchwyt, &danePliku ) == 0 );
        _findclose( uchwyt );
    }
}






