#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <windows.h>

using namespace cv;
using namespace std;




int main(int argc, char** argv)
{
    VideoCapture cap(0); //capturer la vidéo de la webcam

    if (!cap.isOpened())  // si ce n'est pas le cas, quittez le programme
    {
        cout << "Cannot open the web cam" << endl;
        return -1;
    }


    int iLowH = 170;
    int iHighH = 179;

    int iLowS = 150;
    int iHighS = 255;

    int iLowV = 60;
    int iHighV = 255;
    Mat frame2;


    int iLastX = -1;
    int iLastY = -1;

    //Capturer une image temporaire à partir de l'appareil photo
    Mat imgTmp;
    cap.read(imgTmp);

    //Créer une image noire avec la taille comme sortie de la caméra
    Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);;


    while (true)
    {
        Mat imgOriginal;

        bool bSuccess = cap.read(imgOriginal); // lire une nouvelle image de la vidéo



        if (!bSuccess) // si ce n'est pas réussi, rompre la boucle
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }

        Mat imgHSV;

        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convertir l'image capturée de BGR en HSV

        Mat imgThresholded, imgThresholded2, imgThresholded3;

        //  inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

        inRange(imgHSV, Scalar(0, 160, 160), Scalar(10, 255, 255), imgThresholded);
        inRange(imgHSV, Scalar(170, 160, 160), Scalar(180, 255, 255), imgThresholded2);
        // inRange(imgHSV, Scalar(94, 80, 2), Scalar(126, 255, 255), imgThresholded3);
        frame2 = imgThresholded | imgThresholded2;



        //morphological opening (removes small objects from the foreground)
        //erode(frame2, frame2, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        //dilate(frame2, frame2, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

        //morphological closing (removes small holes from the foreground)
        //dilate(frame2, frame2, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        //erode(frame2, frame2, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

        //Calculer les moments de l'image seuillée
        Moments oMoments = moments(frame2);

        double dM01 = oMoments.m01;
        double dM10 = oMoments.m10;
        double dArea = oMoments.m00;

        // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
        if (dArea > 10000)
        {
            //calculate the position of the ball
            int posX = dM10 / dArea;
            int posY = dM01 / dArea;

            if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
            {
                //Draw a red line from the previous point to the current point
                line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0, 0, 255), 2);
                SetCursorPos(posX, posY);

            }


            iLastX = posX;
            iLastY = posY;
            cout << " X :" << posX << "   Y : " << posY << endl;
        }

        imshow("Thresholded Image", frame2); //show the thresholded image

        imgOriginal = imgOriginal + imgLines;
        imshow("Original", imgOriginal); //show the original image

        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
    }

    return 0;
}