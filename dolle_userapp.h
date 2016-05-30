#ifndef DOLLE_USERAPP_H
#define DOLLE_USERAPP_H

#include <QMainWindow>
#include <QSerialPort>
#include <QByteArray>

namespace Ui {
class Dolle_UserApp;
}

class Dolle_UserApp : public QMainWindow
{
    Q_OBJECT

public:
    explicit Dolle_UserApp(QWidget *parent = 0);
    ~Dolle_UserApp();

private slots:
    void readSerial();
    void updateTemperature(QString);
    void updateHumidity(QString);
    void updateStatus(QString);

    void on_pushButton_clicked();

    void on_actionContact_Dolle_triggered();

    void on_actionQuit_triggered();

private:
    Ui::Dolle_UserApp *ui;

    QSerialPort *arduino;
    //static const quint16 arduino_uno_vendor_id = 10755;
    //static const quint16 arduino_uno_product_id = 67;
    QByteArray serialData;
    QString serialBuffer;
    QString parsed_data;
    double temperature_value;
    double humidity_value;
    QString open_value;
};

#endif // DOLLE_USERAPP_H
