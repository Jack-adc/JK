#include "CCreateFileDlg.h"
#include "QMessageBoxDlg.h"
#include<QHBoxLayout>
#include <QRegExpValidator>

CCreateFileDlg::CCreateFileDlg(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Create file"));
    setFixedSize(300, 150);
    Create();
    CreateConnect();
}

void CCreateFileDlg::Create()
{   
    setWindowTitle(tr("Create file"));
    setFixedSize(300, 150);
    QHBoxLayout * pNameLayout = new QHBoxLayout();
    m_pNameLab = new QLabel(tr("name"), this); 
    m_pNameEdit = new QLineEdit(this);
    m_pNameEdit->setFixedHeight(30);
    QRegExp regx("[a-zA-Z0-9\u4e00-\u9fa5]+$");//过滤文件名，只允许字母、数字、中文字符
    QValidator *validator = new QRegExpValidator(regx, m_pNameEdit);
    m_pNameEdit->setValidator(validator);
    pNameLayout->addWidget(m_pNameLab);
    pNameLayout->addWidget(m_pNameEdit);

    QHBoxLayout * pPathLayout = new QHBoxLayout();
    m_pPathLab = new QLabel(tr("path"), this);
    m_pPathEdit = new QLineEdit(this);
    m_pPathEdit->setFixedHeight(30);
    pPathLayout->addWidget(m_pPathLab);
    pPathLayout->addWidget(m_pPathEdit);

    QHBoxLayout * pButtonLayout = new QHBoxLayout();
    m_btnOK = new QPushButton(tr("OK"), this);
    m_btnOK->setFixedSize(QSize(50, 30));
    m_btnCancel = new QPushButton(tr("Cancel"), this);
    m_btnCancel->setFixedSize(QSize(50, 30));
    pButtonLayout->addStretch();
    pButtonLayout->addWidget(m_btnCancel);
    pButtonLayout->addWidget(m_btnOK);


    QVBoxLayout * pMainLayout = new QVBoxLayout();
//     pMainLayout->setMargin(10);
//     pMainLayout->setSpacing(5);
    pMainLayout->addLayout(pNameLayout);
    pMainLayout->addLayout(pPathLayout);
    pMainLayout->addLayout(pButtonLayout);
    
    this->setLayout(pMainLayout);
}

CCreateFileDlg::~CCreateFileDlg()
{

}

void CCreateFileDlg::CreateConnect()
{
    connect(m_btnOK, SIGNAL(clicked()), this, SLOT(slotOK()));
    connect(m_btnCancel, SIGNAL(clicked()), this, SLOT(slotCancel()));
    connect(m_pNameEdit, SIGNAL(textChanged(QString)), this, SLOT(slotTextChange(QString)));

}
void CCreateFileDlg::slotOK()
{
    m_param.strFileName = string(m_pNameEdit->text().toLocal8Bit());
    m_param.strFilePath = string(m_pPathEdit->text().toLocal8Bit());
    if (m_pNameEdit->text().isEmpty())
    {
        QString strLog = tr("please input file name frist!");
        CMessage msg(E_MSGTYPE_INFO, strLog);
        msg.exec();
        return;
    }
    QDialog::accept();
}

void CCreateFileDlg::slotCancel()
{
    QDialog::close();
}

void CCreateFileDlg::slotTextChange(QString strFileName)
{
    strFileName.replace(QString::fromLocal8Bit("【"), "");
    strFileName.replace(QString::fromLocal8Bit("】"), "");
    strFileName.replace(QString::fromLocal8Bit("？"), "");
    strFileName.replace(QString::fromLocal8Bit("！"), "");
    strFileName.replace(QString::fromLocal8Bit("·"), "");
    strFileName.replace(QString::fromLocal8Bit("￥"), "");
    strFileName.replace(QString::fromLocal8Bit("……"), "");
    strFileName.replace(QString::fromLocal8Bit("（"), "");
    strFileName.replace(QString::fromLocal8Bit("）"), "");
    strFileName.replace(QString::fromLocal8Bit("——"), "");
    strFileName.replace(QString::fromLocal8Bit("、"), "");
    strFileName.replace(QString::fromLocal8Bit("："), "");
    strFileName.replace(QString::fromLocal8Bit("；"), "");
    strFileName.replace(QString::fromLocal8Bit("“"), "");    //引号两边分开过滤
    strFileName.replace(QString::fromLocal8Bit("”"), "");
    strFileName.replace(QString::fromLocal8Bit("’"), "");    //引号两边分开过滤
    strFileName.replace(QString::fromLocal8Bit("‘"), "");
    strFileName.replace(QString::fromLocal8Bit("《"), "");
    strFileName.replace(QString::fromLocal8Bit("》"), "");
    strFileName.replace(QString::fromLocal8Bit("，"), "");
    strFileName.replace(QString::fromLocal8Bit("。"), "");
    strFileName.replace(QString::fromLocal8Bit("`"), "");
    strFileName.replace(QString::fromLocal8Bit("｀"), "");
    strFileName.replace(QString::fromLocal8Bit("～"), "");
    strFileName.replace(QString::fromLocal8Bit("！"), "");
    strFileName.replace(QString::fromLocal8Bit("＠"), "");
    strFileName.replace(QString::fromLocal8Bit("＃"), "");
    strFileName.replace(QString::fromLocal8Bit("＄"), "");
    strFileName.replace(QString::fromLocal8Bit("％"), "");
    strFileName.replace(QString::fromLocal8Bit("＾"), "");
    strFileName.replace(QString::fromLocal8Bit("＆"), "");
    strFileName.replace(QString::fromLocal8Bit("＊"), "");
    strFileName.replace(QString::fromLocal8Bit("＿"), "");
    strFileName.replace(QString::fromLocal8Bit("＋"), "");
    strFileName.replace(QString::fromLocal8Bit("｛"), "");
    strFileName.replace(QString::fromLocal8Bit("｝"), "");
    strFileName.replace(QString::fromLocal8Bit("｜"), "");
    strFileName.replace(QString::fromLocal8Bit("："), "");
    strFileName.replace(QString::fromLocal8Bit("＂"), "");
    strFileName.replace(QString::fromLocal8Bit("？"), "");
    strFileName.replace(QString::fromLocal8Bit("＞"), "");
    strFileName.replace(QString::fromLocal8Bit("＜"), "");
    strFileName.replace(QString::fromLocal8Bit("－"), "");
    strFileName.replace(QString::fromLocal8Bit("＝"), "");
    strFileName.replace(QString::fromLocal8Bit("．"), "");
    strFileName.replace(QString::fromLocal8Bit("／"), "");
    strFileName.replace(QString::fromLocal8Bit("＇"), "");
    strFileName.replace(QString::fromLocal8Bit("［"), "");
    strFileName.replace(QString::fromLocal8Bit("］"), "");
    strFileName.replace(QString::fromLocal8Bit("＼"), "");
    m_pNameEdit->setText(strFileName);
}


