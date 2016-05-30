/* This program will read serial data, consisting of tilt, temperature and humidity sensor output.
 * This data will be saved to an external .csv file in order to add spreadsheet capabilities for
 * its further use
 */

#include "dolle_userapp.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dolle_UserApp w;
    w.show();

    return a.exec();
}
