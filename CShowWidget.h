#pragma once

#include <QtWidgets/QWidget>
#include <string>
class CShowWidget : public QWidget
{
    Q_OBJECT

public:
    CShowWidget(std::string strName,QWidget *parent = Q_NULLPTR);

private:
    std::string m_strFileName;
};
