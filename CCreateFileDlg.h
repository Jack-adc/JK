#pragma once
#include <QtWidgets/QDialog>
#include <string>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

using namespace std;

struct TNFileParam
{
    string strFileName;
    string strFilePath;
};

class CCreateFileDlg : public QDialog
{
    Q_OBJECT

public:
    CCreateFileDlg(QWidget *parent = Q_NULLPTR);  
    ~CCreateFileDlg();
    TNFileParam getParam() const
    {
        return m_param;
    }

private slots:
    void slotOK();
    void slotCancel();
    void slotTextChange(QString);

private:
    void Create();
    void CreateConnect();

private:
    QLabel*		 m_pNameLab;
    QLabel*		 m_pPathLab;
    QLineEdit*	 m_pNameEdit;
    QLineEdit*	 m_pPathEdit;
    QPushButton* m_btnOK;
    QPushButton* m_btnCancel;

private:
    TNFileParam m_param;

};
