/*Main author: Marco Montecchi
             ENEA (Italy)
             email: marco.montecchi@enea.it
Porting to Windows by
             Alberto Mittiga
             ENEA (Italy)
             email: alberto.mittiga@enea.it

This software allows to calibrate a digital camera by menans of a chessboard on the basis of
the tools offered by OpenCV library. After that, any loaded image is corrected for distorsions.
At image loading, some relevant parameters contained in the metadata are read by the means of the library Exiv2.

   Copyright (C) 2026  Marco Montecchi

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "camcalib.h"
#include "ui_camcalib.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <exiv2/exiv2.hpp>
#include <memory>         // Per std::unique_ptr
using namespace std;
using namespace cv;


//global variables *******************************
QString dir;
QString filename1;
Mat imgCam1;

//camera parameters
int Width= 2448;
int Height=2048;
double fcam=8.0; //camera focal (mm)         (0,0) ---> x' (j)
//double fcam_2=8.0; //camera focal (mm)         |
double pxdimX=0.00345;//pixel size(mm)           |
double pxdimY=pxdimX;//
double fx=fcam/pxdimX;
double fy=fcam/pxdimY;
double cx=Width/2.;
double cy=Height/2.;
double k1=0.,k2=0.,k3=0.;
double p1=0.,p2=0.;

Mat cameraMatrix =  (Mat_<double>(3,3) <<  fx,          0 ,     cx,
                      0,fy ,     cy,
                      0,          0 ,      1);

Mat distCoeffs = (Mat_<double>(5,1)   <<    k1,k2,p1,p2,k3);

int iStart=0;

//*********************************

camcalib::camcalib(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::camcalib){

    ui->setupUi(this);

    //signals & slots
    connect(ui->pB_Load,SIGNAL( clicked() ),this,SLOT(selectImg()));
    connect(ui->pB_und,SIGNAL( clicked() ),this,SLOT(imgund()));
    connect(ui->pB_calibrate,SIGNAL( clicked() ),this,SLOT(calibrate()));

    //windows
    namedWindow("ImgAsIs",WINDOW_NORMAL );
    namedWindow("ImgUndist",WINDOW_NORMAL );

    // parameter initialization
#ifdef __unix__
#define IS_POSIX 1
#else
#define IS_POSIX 0
#endif

    QDir Dir;  //current directory
    Dir.cdUp();//cd ..
    if (IS_POSIX == 1) {
        //Linux path initialization
        Dir.cdUp();//cd ..
        dir=Dir.absolutePath()+"/CamCalib/";
    }
    else {
        //windows path inizialization
        //Dir.cdUp();//cd ..
        dir=Dir.absolutePath();
    }
    printf("dir= %s\n",dir.toStdString().c_str() );
}

camcalib::~camcalib()
{
    delete ui;
}


void camcalib::selectImg(){
    filename1 = QFileDialog::getOpenFileName(this,tr("Open Image"),
                                             dir,
                                             tr("Image Files (* *.png *.jpg *.bmp)"));
    //cout<<"setion: "<<filename1.section('/',0,-2).toStdString()<<"\n";
    if(filename1.isEmpty())
        return;
    ui->lineEdit_img->setText(filename1.section('/',-1,-1));
    viewImg(filename1);
}


void camcalib::viewImg(QString fileImgCam1){
    printf("->viewImg(%s)\n",fileImgCam1.toStdString().c_str());
    fflush(stdout);
    double focalLengthValue=0.;
    imgCam1=imread(fileImgCam1.toStdString());//,IMREAD_UNCHANGED );//IMREAD_UNCHANGED);
    if(! imgCam1.data){
        cout << fileImgCam1.toStdString() <<" for viewImg is not accessible!!!\n"<<"\n";
        return;
    }
    Width=imgCam1.cols;
    Height=imgCam1.rows;
    QString sDepth;
    if(imgCam1.depth()==CV_8U)
        sDepth="8 bit u-int";
    else if(imgCam1.depth()==CV_8S)
        sDepth="8 bit s-int";
    else if(imgCam1.depth()==CV_16U)
        sDepth="16 bit u-int";
    else if(imgCam1.depth()==CV_16S)
        sDepth="16 bit s-int";
    else if(imgCam1.depth()==CV_32S)
        sDepth="32 bit s-int";
    else if(imgCam1.depth()==CV_32F)
        sDepth="32 bit-float";
    else if(imgCam1.depth()==CV_64F)
        sDepth="64 bit float";
            // CV_8U- 8-bit unsigned integers ( 0..255 )
            // CV_8S - 8-bit signed integers ( -128..127 )
            // CV_16U - 16-bit unsigned integers ( 0..65535 )
            // CV_16S - 16-bit signed integers ( -32768..32767 )
            // CV_32S - 32-bit signed integers ( -2147483648..2147483647 )
            // CV_32F - 32-bit floating-point numbers ( -FLT_MAX..FLT_MAX, INF, NAN )
            // CV_64F - 64-bit floating-point numbers ( -DBL_MAX..DBL_MAX, INF, NAN )
    ui->lineEdit_depth->setText(sDepth);
    int iChan=imgCam1.channels();
    ui->sB_width->setValue(Width);
    ui->sB_height->setValue(Height);
    ui->sB_channel->setValue(iChan);
    imshow("ImgAsIs",imgCam1);
    //lettura lunghezza focale dai metadati Exif contenuti nel file immagine
    try {
        // Apri il file immagine con Exiv2
        std::unique_ptr<Exiv2::Image> imageExif = Exiv2::ImageFactory::open(fileImgCam1.toStdString());
        if (imageExif.get() == 0) {
            std::cerr << "Errore: Impossibile aprire l'immagine EXIF " << fileImgCam1.toStdString() << std::endl;
            return;
        }
        // Leggi i metadati EXIF
        imageExif->readMetadata();
        // Accedi ai dati EXIF
        Exiv2::ExifData &exifData = imageExif->exifData();
        if (exifData.empty()) {
            std::cout << "Nessun dato EXIF trovato nell'immagine." << std::endl;
        } else {
            // Cerca il tag della lunghezza focale (Exif.Image.FocalLength)
            Exiv2::ExifData::const_iterator end = exifData.end();
            Exiv2::ExifData::const_iterator focalLengthIt = exifData.findKey(Exiv2::ExifKey("Exif.Photo.FocalLength"));
            if (focalLengthIt != end) {
                // Il valore della lunghezza focale è spesso una razionale
                // Puoi stamparlo direttamente o convertirlo in un float
                std::cout << "Lunghezza Focale: " << focalLengthIt->print() << std::endl;
                // Per ottenere il valore numerico (float):
                // Nota: FocalLength è tipicamente un valore razionale (num/den).
                Exiv2::Rational r = focalLengthIt->toRational();
                focalLengthValue = static_cast<double>(r.first) / r.second;
                std::cout << "Lunghezza Focale (numerica): " << focalLengthValue << " mm" << std::endl;
            } else {
                std::cout << "Tag 'FocalLength' non trovato nei dati EXIF." << std::endl;
            }
        }
        if(iStart==0){
            pxdimX=ui->dSB_pxDim->value();// micron
            pxdimX=pxdimX/1000.;// mm
            pxdimY=pxdimX;
            fcam=focalLengthValue;
            fx=fcam/pxdimX;
            fy=fcam/pxdimY;
            cx=Width/2.;
            cy=Height/2.;
            //k1=0.,k2=0.,k3=0.;
            ui->dSB_focal->setValue(focalLengthValue);
            ui->dSB_fx->setValue(fx);
            ui->dSB_fy->setValue(fy);
            ui->dSB_cx->setValue(cx);
            ui->dSB_cy->setValue(cy);
            //ui->dSB_k1->setValue(k1);
            //ui->dSB_k2->setValue(k2);
            //ui->dSB_k3->setValue(k3);
            iStart=1;
        }
    } catch (const Exiv2::Error& e) {
        std::cerr << "Errore Exiv2: " << e.what() << std::endl;
    }

    imgund();
}


void camcalib::imgund(){
    printf("->imgund()\n");
    Mat imgu;
    //double focal=ui->dSB_focal->value();
    //pxdimX=ui->dSB_pxDim->value();
    //fx=focal/pxdimX*1000.;
    fx=ui->dSB_fx->value();
    fy=ui->dSB_fy->value();
    cx=ui->dSB_cx->value();
    cy=ui->dSB_cy->value();
    k1=ui->dSB_k1->value();
    k2=ui->dSB_k2->value();
    k3=ui->dSB_k3->value();
    p1=ui->dSB_p1->value();
    p2=ui->dSB_p2->value();
    cameraMatrix.at<double>(0,0)=fx;
    cameraMatrix.at<double>(1,1)=fx;
    cameraMatrix.at<double>(0,2)=cx;
    cameraMatrix.at<double>(1,2)=cy;
    distCoeffs.at<double>(0)=k1;
    distCoeffs.at<double>(1)=k2;
    distCoeffs.at<double>(2)=p1;
    distCoeffs.at<double>(3)=p2;
    distCoeffs.at<double>(4)=k3;
    undistort(imgCam1,imgu,cameraMatrix,distCoeffs);
    imshow("ImgUndist",imgu);
    imwrite((dir+"undImg.jpg").toStdString(),imgu);
}


void camcalib::calibrate(){
    int Nstart=ui->sB_Nstart->value();
    printf("->calibrate() start with Nstart=%d\n",Nstart);
    int numCornersHor = ui-> sB_HnC -> value();
    int numCornersVer = ui-> sB_VnC -> value();
    int numSquares = numCornersHor * numCornersVer;
    double cornerStep = ui->dSB_step -> value();
    Size board_sz = Size(numCornersHor, numCornersVer);
    vector<vector<Point3f>> object_points;
    vector<vector<Point2f>> image_points;
    vector<Point2f> corners;
    Mat imageCal8b;
    Mat imageCal(Height,Width,CV_8UC1);
    Mat imageRGB(Height,Width,CV_8UC3);
    uint8_t val8;
    vector<Point3f> obj;
    for(int j=0;j<numSquares;j++){
        //obj.push_back(Point3d((j/numCornersHor)*cornerStep, (j%numCornersHor)*cornerStep, 0.0));
        obj.push_back(Point3d((j%numCornersHor)*cornerStep,(j/numCornersHor)*cornerStep,0.0));
        //printf("obj[%d]=(%f,%f,%f)\n",j,obj[j].x,obj[j].y,obj[j].z);
    }
    //    for(int i=0;i<numCornersVer;i++){
    //        for(int j=0;j<numCornersHor;j++)
    //          printf("Point[%d][%d]=(%f,%f,%f)\n",j,i,obj[i*numCornersVer+j].x,
    //                  obj[i*numCornersVer+j].y,obj[i*numCornersVer+j].z);
    //    }
    QString fileCalImg;
    printf("Camera Calibration is started...\n");
    QString PathPro="";
    PathPro=QFileDialog::getExistingDirectory(this, tr("Open Directory for calibrate"),
                                                dir+"/camCalib",
                                                QFileDialog::ShowDirsOnly);
    if(PathPro.isEmpty()){
        printf("abort!\n");
        return;
    }
    PathPro=PathPro+"/";
    QMessageBox msgBox1;
    int iOK=0;
    int iContinue=1;
    QString baseName=ui->lineEdit_baseName->text();
    QString fileExt=ui->lineEdit_type->text();
    while(iContinue==1){
        fileCalImg=PathPro+baseName+QString::number(Nstart)+"."+fileExt;
        cout << "processing img "<<fileCalImg.toStdString() <<"\n";
        QFileInfo check_file(fileCalImg);
        // check if file exists and if yes: Is it really a file and no directory?
        if (check_file.exists() && check_file.isFile()) {
            imageCal8b=imread(fileCalImg.toStdString(),IMREAD_GRAYSCALE);
        } else {
            iContinue=0;
            printf("calibration aborted!!!\n");
        }
        if(iContinue==1){
            for(int i=0; i<Height; i++){
                for(int j=0; j< Width; j++){
                    val8=imageCal8b.at<uint8_t>(i,j);
                    imageCal.at<uint8_t>(i,j)=val8;
                    for(int k=0; k<3; k++)
                        imageRGB.at<Vec3b>(i,j)[k]=val8;
                }
            }
            imshow("ImgAsIs",imageRGB);
            waitKey(10);
            bool found = findChessboardCorners(imageCal, board_sz, corners, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FILTER_QUADS);
            if(found){
                cornerSubPix(imageCal, corners, Size(11, 11), Size(-1, -1), TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 30, 0.0001 ));
                drawChessboardCorners(imageRGB, board_sz, corners, found);
            }
            imshow("ImgUndist",imageRGB);
            msgBox1.setText("ATTENTION!");
            msgBox1.setInformativeText("Were corners rightly located?\n");
            msgBox1.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Abort);
            msgBox1.setDefaultButton(QMessageBox::Yes);
            int ret = msgBox1.exec();
            if(ret==QMessageBox::Yes){
                image_points.push_back(corners);
                object_points.push_back(obj);
                cout <<"positive!\n";
                //                for(int iCor=0;iCor<numSquares;iCor++)
                //                    printf("%d %f %f\n",iCor,corners[iCor].x,corners[iCor].y);
                iOK++;
            }
            else if(ret==QMessageBox::No)
                cout <<"negative!\n";
            else if(ret==QMessageBox::Abort)
                iContinue=0;
            Nstart++;
            printf("iOK= %d\n",iOK);
        }
    }
    if(iOK>0){
        cout<<"calibreCamera...."<<"\n";
        vector<Mat> rvecs;
        vector<Mat> tvecs;
        Mat corrImg;
        int iMth=0;//ui->comboBox_calibMethod->currentIndex();
        int method=0;
        int Ndim=5;
        if(iMth==0){
            method=CALIB_FIX_ASPECT_RATIO;
            Ndim=5;
        }
        else if(iMth==1){
            method=CALIB_FIX_ASPECT_RATIO+CALIB_ZERO_TANGENT_DIST+
                     CALIB_FIX_K1+CALIB_FIX_K2+CALIB_FIX_K3;
            //CALIB_FIX_K4+CALIB_FIX_K5+CALIB_FIX_K6+
            //CALIB_FIX_S1_S2_S3_S4+
            //CALIB_FIX_TAUX_TAUY;
            Ndim=5;
        }
        else if(iMth==2){
            method=CALIB_FIX_ASPECT_RATIO+CALIB_RATIONAL_MODEL;
            Ndim=8;
        }
        else if(iMth==3){
            method=CALIB_FIX_ASPECT_RATIO+CALIB_THIN_PRISM_MODEL;
            Ndim=12;
        }
        else if(iMth==4){
            method=CALIB_FIX_ASPECT_RATIO+CALIB_TILTED_MODEL;
            Ndim=14;
        }
        else if(iMth==5){
            method=CALIB_FIX_ASPECT_RATIO+CALIB_RATIONAL_MODEL+
                     CALIB_THIN_PRISM_MODEL+CALIB_TILTED_MODEL;
            Ndim=14;
        }
        else if(iMth==6){
            method=CALIB_USE_INTRINSIC_GUESS+CALIB_FIX_FOCAL_LENGTH+
                     CALIB_FIX_PRINCIPAL_POINT;
            Ndim=5;
        }
        else{
            method=CALIB_USE_INTRINSIC_GUESS+CALIB_FIX_FOCAL_LENGTH+
                     CALIB_FIX_PRINCIPAL_POINT+CALIB_RATIONAL_MODEL+
                     CALIB_THIN_PRISM_MODEL+CALIB_TILTED_MODEL;
            Ndim=14;
        }
        printf("method=%d\n",method);


        Mat discoe;
        //if(iMth<=5)
        calibrateCamera(object_points, image_points, imageCal.size(), cameraMatrix, discoe, rvecs, tvecs,method);
        //else
        //    solvePnP(object_points, image_points, cameraMatrix_1, discoe, rvecs, tvecs,false,SOLVEPNP_ITERATIVE);
        printf("fx= %f fy= %f \ncx=%f cy=%f\n",
               cameraMatrix.at<double>(0,0),cameraMatrix.at<double>(1,1),
               cameraMatrix.at<double>(0,2),cameraMatrix.at<double>(1,2));
        for(int j=0;j<Ndim;j++){
            distCoeffs.at<double>(j)=discoe.at<double>(j);
            printf("distCoeffs[%d]=%e\n",j,discoe.at<double>(j));
        }
        // for(int j=Ndim;j<14;j++){
        //     distCoeffs.at<double>(j)=0.;
        // }
        // for(int j=0;j<14;j++){
        //     if(abs(distCoeffs.at<double>(j))<1.e-100)
        //         distCoeffs.at<double>(j)=0.;
        // }
        ui->dSB_fx->setValue(cameraMatrix.at<double>(0,0));
        ui->dSB_fy->setValue(cameraMatrix.at<double>(1,1));
        ui->dSB_cx->setValue(cameraMatrix.at<double>(0,2));
        ui->dSB_cy->setValue(cameraMatrix.at<double>(1,2));
        ui->dSB_k1->setValue(distCoeffs.at<double>(0));
        ui->dSB_k2->setValue(distCoeffs.at<double>(1));
        ui->dSB_p1->setValue(distCoeffs.at<double>(2));
        ui->dSB_p2->setValue(distCoeffs.at<double>(3));
        ui->dSB_k3->setValue(distCoeffs.at<double>(4));
        //ui->lineEdit_k4_1->setText(QString::number(distCoeffs_1.at<double>(5)));
        //ui->lineEdit_k5_1->setText(QString::number(distCoeffs_1.at<double>(6)));
        //ui->lineEdit_k6_1->setText(QString::number(distCoeffs_1.at<double>(7)));
        //ui->lineEdit_s1_1->setText(QString::number(distCoeffs_1.at<double>(8)));
        //ui->lineEdit_s2_1->setText(QString::number(distCoeffs_1.at<double>(9)));
        //ui->lineEdit_s3_1->setText(QString::number(distCoeffs_1.at<double>(10)));
        //ui->lineEdit_s4_1->setText(QString::number(distCoeffs_1.at<double>(11)));
        //ui->lineEdit_tx_1->setText(QString::number(distCoeffs_1.at<double>(12)));
        //ui->lineEdit_ty_1->setText(QString::number(distCoeffs_1.at<double>(13)));
        fx=cameraMatrix.at<double>(0,0);
        fy=cameraMatrix.at<double>(1,1);
        cx=cameraMatrix.at<double>(0,2);
        cy=cameraMatrix.at<double>(1,2);
        fcam=(cameraMatrix.at<double>(0,0)*pxdimX+cameraMatrix.at<double>(1,1)*pxdimY)/2.;
        ui-> dSB_focal  -> setValue(fcam);
        //undistort(imageRGB,corrImg,cameraMatrix,distCoeffs);
    }
}
