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
#ifndef CAMCALIB_H
#define CAMCALIB_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
class camcalib;
}
QT_END_NAMESPACE

class camcalib : public QWidget
{
    Q_OBJECT

public:
    camcalib(QWidget *parent = nullptr);
    ~camcalib();

private:
    Ui::camcalib *ui;

public slots:
    void selectImg();
    void viewImg(QString fileImgCam1);
    void imgund();
    void calibrate();
};
#endif // CAMCALIB_H
