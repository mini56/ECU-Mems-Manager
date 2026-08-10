#ifndef ABOUTBOX_H  
#define ABOUTBOX_H  
  
#include <QWidget>  
  
/**  
 * The "About" dialog that shows information about the program.  
 */  
class AboutBox  
{  
public:  
    static void showAbout(QWidget *parent = nullptr);  
};  
  
#endif // ABOUTBOX_H
