#include "dolle_userapp.h"
#include "ui_dolle_userapp.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <string>
#include <QDebug>
#include <QMessageBox>
#include <unistd.h>

#include <QFile>
#include <QTextStream>
#include <QFileDialog>

#include <QTime>

QStringList sheet;


Dolle_UserApp::Dolle_UserApp(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Dolle_UserApp)
{
    ui->setupUi(this);
    ui->tempLCD->display("-----");
    arduino = new QSerialPort(this);
        serialBuffer = "";
        parsed_data = "";
        temperature_value = 0.0; // set the value 0 to the variable temperature_value.
        humidity_value = 0.0;
        open_value = "Unknown"; // sets the value to 'empty' meaning waiting to receive any values 


        bool arduino_is_available = true;
        //Declare the Arduino port according to Unix standards
        //For Windows systems the default port would be "COM3"
        QString arduino_uno_port_name = "/dev/ttyACM0";

         /*  Open and configure the arduino port if available
         */
        if(arduino_is_available){  //check if data can be read from Arduino or if arduino is ready 
            qDebug() << "Found the arduino port...\n"; // it writes the message 'Found the arduino port' to the prompt/application output if the arduino is found.
            arduino->setPortName(arduino_uno_port_name); // these functions are checking the arduino and setting the values of what to expect: baud rate, parity, stop bits...
            arduino->open(QSerialPort::ReadOnly); // we read in async mode meaning that the value registered by Arduino are not lively displayed
            arduino->setBaudRate(QSerialPort::Baud9600);
            arduino->setDataBits(QSerialPort::Data8);
            arduino->setFlowControl(QSerialPort::NoFlowControl);
            arduino->setParity(QSerialPort::NoParity);
            arduino->setStopBits(QSerialPort::OneStop);
            QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(readSerial()));
        }else{
            qDebug() << "Couldn't find the correct port for the arduino.\n"; // if the arduino is not ready then display this message.
            QMessageBox::information(this, "Serial Port Error", "Couldn't open serial port to arduino.");
        }
    }

    Dolle_UserApp::~Dolle_UserApp() // this is the destructor
    {
        if(arduino->isOpen()){
            arduino->close(); //    Close the serial port if it's open.
        }
        delete ui;
    }

    void Dolle_UserApp::readSerial()
    {
        /*
         * Serial sensor data is separated by commas, therefore
         * the buffer needs to be separated and assigned to respective variables
         *
         */
       QStringList buffer_split = serialBuffer.split(","); //  split the serialBuffer string, parsing with ',' as the separator
// the variable buffer_split calls the variable we created at the top of this code, in the beginning, serialBuffer that creates a string separated by commas.  
// with the help of a pre-defined function .split the string created by serialBuffer is transformed into an array at the sign of comma. 
       qDebug() << buffer_split << "\n";


        /*  Check to see if there less than 3 tokens in buffer_split
         *  It means that there are three comma separated values and all sensor data has been read
         */

        if(buffer_split.length() < 3){ // this checks the array created by the buffer.split has all three values int he array; if not then redo 
            // no parsed value yet so continue accumulating bytes from serial in the buffer.
            serialData = arduino->readAll();
            serialBuffer = serialBuffer + QString::fromStdString(serialData.toStdString());
            serialData.clear();
        }else{

            //Cear the buffer so it would be ready for the next cycle
            serialBuffer = "";

            open_value = buffer_split[0];
            temperature_value = buffer_split[1].toDouble(); // celsius
            humidity_value = buffer_split[2].toDouble();

            // format precision of sensor values to 4 digits or fewer and call the function to display them
            parsed_data = QString::number(temperature_value, 'g', 4);
            Dolle_UserApp::updateTemperature(parsed_data);

            parsed_data = QString::number(humidity_value, 'g', 4);
            Dolle_UserApp::updateHumidity(parsed_data);


            Dolle_UserApp::updateStatus(open_value);

            QTime time = QTime::currentTime();

            //Use the sheet array to store comma separated sensor values, to store in the .csv file
            sheet.append(open_value+",");
            sheet.append(buffer_split[2]+",");
            sheet.append(buffer_split[1]+",");
            sheet.append(time.toString());
            sheet.append("\n");

            //Arduino waits for 2 seconds before writing to serial
            //The 3 second sleep is to prevent reading data before its been written
            sleep(3);

        }

    }

    //Functions to display sensor values in the GUI
    void Dolle_UserApp::updateTemperature(QString sensor_reading)
    {
        //  update the value displayed on the lcdNumber
        ui->tempLCD->display(sensor_reading);
    }

    void Dolle_UserApp::updateHumidity(QString sensor_reading)
    {
        //  update the value displayed on the lcdNumber
        ui->humLCD->display(sensor_reading);
    }

    void Dolle_UserApp::updateStatus(QString sensor_reading)
    {
        //  update the value displayed on the lcdNumber
        ui->status->setText(sensor_reading);
    }

/* On the click of the GUI button the values of the sheet array, containing the serial data
 * will be written to an external file specified by the user
 */
void Dolle_UserApp::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString(),
               tr("Spreadsheet files (*.csv);;C++ Files (*.cpp *.h)"));

       //File write mode is sent to append so the previous values won't be erased
       if (!fileName.isEmpty()) {
           QFile file(fileName);
           //if (file.open(QIODevice::WriteOnly)) {
           if (file.open(QFile::Append)) {

            //Write the column titles to the file
             QTextStream stream(&file);
             stream << "Ladder Status,"
                    <<"Humidity,"
                    <<"Temperature,"
                    <<"Time Stamp"
                    <<"\n";

             /*Register only the last two values of the serial readings
              * such as to avoid multiple simmilar values
              */
             for(int i=sheet.length()-10; i<sheet.length();i++){
                 stream << sheet[i];
                 }

           file.close();
           }
    }
}

//Functions for Contact Dolle and Quit menu items

void Dolle_UserApp::on_actionContact_Dolle_triggered()
{
    QMessageBox::information(this, "Contact Dolle", "In order to contact Dolle please use\n"
                                                    "the following e-mail address:\n"
                                                    "\n"
                                                    "supporte@dolle.dk");
}

void Dolle_UserApp::on_actionQuit_triggered()
{
    qApp->quit(); //quit() is a QT specific function. The arrow means that qApp is passed as an argument to the quit() function-
                // it also can be re-written as qApp::quit() meaning that the function quit() is called by qApp.   
}
