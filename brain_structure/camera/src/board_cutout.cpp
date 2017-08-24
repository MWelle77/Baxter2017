//************************************************
//**********Baxter 2017 Tic-Tac-Toe***************
//*******Nadine Drollinger & Michael Welle********
//************************************************
//*******Camera node - board_cutout*************
//************************************************

//************************************************
// Description: cuts out the board in a rough fashion
//************************************************

//Includes
#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"

//Namespace
using namespace cv;  
using namespace std;  


//Class board_cutout
class board_cutout
{
  ros::NodeHandle n_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_raw_;
  image_transport::Publisher image_pub_cut_;
  
  public:
    //vars for cutting out board
    double cutout_x; //start cut out point x
    double cutout_y; //start cut out point y
    double cutout_width; //start cut out point width
    double cutout_height; //start cut out point height
    bool debug_flag;
    //Open cv images
    Mat org, grey, game; 
    Mat M, rotated, cropped;
    board_cutout() : it_(n_),debug_flag(true),cutout_x(10),cutout_y(80),cutout_width(450),cutout_height(350) 
    {
      // Subscrive and publisher
      image_sub_raw_ = it_.subscribe("/TTTgame/webcam/input_image_raw", 1, &board_cutout::imageCb, this);
      image_pub_cut_ = it_.advertise("/TTTgame/cut_board", 1);
      
      //debug
      if(debug_flag){
        namedWindow("Input", CV_WINDOW_AUTOSIZE);
        namedWindow("Cut output",CV_WINDOW_AUTOSIZE);
      }      
    }

    ~board_cutout()
    {
      if(debug_flag){
        destroyWindow("Input");
        destroyWindow("Cut output"); 
      }    
    }


    void imageCb(const sensor_msgs::ImageConstPtr& msg)
    {
      //read in image from subscribed topic and transform it to opencv
      cv_bridge::CvImagePtr cv_ptr;
      try
      {
        cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
      }
      catch (cv_bridge::Exception& e)
      {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
      }

      // process the image
      org = cv_ptr->image;
//debug output
            if(debug_flag){
              waitKey(1);
              imshow("Input", org); //show the frame in "MyVideo" window
              waitKey(1);
            }
      // we cut out a smaller portion of the image
       Rect Rec(cutout_x, cutout_y, cutout_width, cutout_height);

       Mat cutorg  = org(Rec).clone();

       //publish the image in ros
    cv_bridge::CvImage out_msg;
    out_msg.header   = cv_ptr->header; // Same timestamp and tf frame as input image
    out_msg.header.stamp =ros::Time::now(); // new timestamp
    out_msg.encoding = sensor_msgs::image_encodings::BGR8; // encoding, might need to try some diffrent ones
    out_msg.image    = cutorg; 
    image_pub_cut_.publish(out_msg.toImageMsg()); //transfer to Ros image message  

            //debug output
            if(debug_flag){
              waitKey(1);
              imshow("Cut output", cutorg); //show the frame in "MyVideo" window
              waitKey(1);
            }


  }
};


int main(int argc, char** argv)
{
  ros::init(argc, argv, "board_tracker");
  board_cutout bc;
  ROS_INFO("board tracker initilized");
  ros::spin();
  return 0;
}