/****************************************************************************
* RTIViewer                                                         o o     *
* Single and Multi-View Reflectance Transformation Image Viewer   o     o   *
*                                                                _   O  _   *
* Copyright	(C) 2008-2010                                          \/)\/    *
* Visual Computing Lab - ISTI CNR					              /\/|      *
* and											                     |      *
* Cultural Heritage Imaging							                 \      *
*																			*
* This program is free software: you can redistribute it and/or modify		*
* it under the terms of the GNU General Public License as published by		*
* the Free Software Foundation, either version 3 of the License, or			*
* (at your option) any later version.										*
*																			*
* This program is distributed in the hope that it will be useful,			*
* but WITHOUT ANY WARRANTY; without even the implied warranty of			*
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				*
* GNU General Public License for more details.								*
*																			*
* You should have received a copy of the GNU General Public License			*
* along with this program.  If not, see <http://www.gnu.org/licenses/>.		*
****************************************************************************/

#include "aboutdlg.h"

#include <QDesktopServices>
#include <QDir>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QUrl>
#include <QVBoxLayout>

/*====================================================================
 *
 * Public methods
 *
 *===================================================================*/

AboutDlg::AboutDlg(QWidget *parent) :
    QDialog(parent)
{
    // Initialize the About dialog parameters

    initAbout();

    // Initialize the layout

    initLayout();

    // Connect signals and slots

    setConnections();

    // Translate the UI strings as needed

    retranslateUi();
}

void AboutDlg::retranslateUi()
{
    // Translate the UI strings

    this->setWindowTitle(QApplication::translate("aboutDialog", getAboutDialogTitle(), 0, QApplication::UnicodeUTF8));
    titleLabel->setText(QApplication::translate("aboutDialog", getTitleHTML(), 0, QApplication::UnicodeUTF8));
    gplLabel->setText(QApplication::translate("aboutDialog", getGplHTML(), 0, QApplication::UnicodeUTF8));
    guideLabel->setText(QApplication::translate("aboutDialog", getGuideHTML(), 0, QApplication::UnicodeUTF8));
    descLabel->setText(QApplication::translate("aboutDialog", getDescHTML(), 0, QApplication::UnicodeUTF8));
}

/*====================================================================
 *
 * Private slots
 *
 *===================================================================*/

void AboutDlg::showManual(QString file)
{
    // Show the manual when the user clicks on the User's Guide label

    QString currentDir = QDir::currentPath();
    QString path = tr("file:///%1/%2").arg(currentDir).arg(file);
    QDesktopServices::openUrl(QUrl(path));
}

void AboutDlg::showURL(QString url)
{
    // Show the Web page when the user clicks on a link

    QDesktopServices::openUrl(QUrl(url));
}

/*====================================================================
 *
 * Private initialization methods
 *
 *===================================================================*/

void AboutDlg::initAbout()
{
    // Initialize the main dialog box

    this->setMinimumSize(875, 550);
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    this->setModal(true);
}

void AboutDlg::initLayout()
{
    // The main layout has two parts -- writing on top and logos at the bottom

    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(initInfoLayout());
    mainLayout->addSpacing(15);
    mainLayout->addStretch(1);
    mainLayout->addLayout(initLogosLayout());
    setLayout(mainLayout);
}

QLayout *AboutDlg::initInfoLayout()
{
    // The top half of the dialog box has a product description (descLabel)
    // on the right and a vertical layout (initTitleLayout()) on the left.

    descLabel = new QLabel();
    descLabel->setAlignment(Qt::AlignLeft);
    descLabel->setWordWrap(true);

    QHBoxLayout * infoLayout = new QHBoxLayout();
    infoLayout->setSpacing(20);
    infoLayout->addLayout(initTitleLayout());
    infoLayout->addWidget(descLabel);
    return infoLayout;
}

QLayout *AboutDlg::initTitleLayout()
{
    // The title layout has the product title, a picture, a horizontal
    // user guide layout, and a GPL 3 label.

    titleLabel = new QLabel();
    titleLabel->setMaximumSize(250, 64);
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel * tombImage = new QLabel();
    tombImage->setMaximumSize(250, 250);
    tombImage->setPixmap(QPixmap(":/images/tomb.png"));
    tombImage->setScaledContents(true);
    tombImage->setAlignment(Qt::AlignCenter);

    gplLabel = new QLabel();
    gplLabel->setMaximumSize(250, 32);
    gplLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout * titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(15);
    titleLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::MinimumExpanding));
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(tombImage);
    titleLayout->addLayout(initUserGuideLayout());
    titleLayout->addWidget(gplLabel);
    titleLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::MinimumExpanding));

    return titleLayout;
}

QLayout *AboutDlg::initUserGuideLayout()
{
    // The user guide layout has a label (User Guide) and a PDF logo.

    guideLabel = new QLabel();
    guideLabel->setMaximumSize(112, 32);
    guideLabel->setAlignment(Qt::AlignCenter);

    QLabel * pdfLogo = new QLabel();
    pdfLogo->setMaximumSize(32, 32);
    pdfLogo->setPixmap(QPixmap(":/images/logo-pdf.png"));
    pdfLogo->setScaledContents(true);
    pdfLogo->setAlignment(Qt::AlignCenter);

    QHBoxLayout * userGuideLayout = new QHBoxLayout();
    userGuideLayout->setSizeConstraint(QLayout::SetMaximumSize);
    userGuideLayout->setSpacing(5);
	userGuideLayout->addSpacerItem(new QSpacerItem(100, 32, QSizePolicy::Maximum, QSizePolicy::Fixed));
    userGuideLayout->addWidget(guideLabel);
    userGuideLayout->addWidget(pdfLogo);
	userGuideLayout->addSpacerItem(new QSpacerItem(100, 32, QSizePolicy::Maximum, QSizePolicy::Fixed));

    return userGuideLayout;
}

QLayout *AboutDlg::initLogosLayout()
{
    // The logos layout is horizontal. From left to right, it has
    // a VCG logo, vertical layout with Univ. Minho and UCSC, vertical
    // layout with IMSL and CHI, and vertical layout with HP and OK button.

    QLabel * vcgLogo = new QLabel();
    vcgLogo->setMinimumSize(202, 173);
    vcgLogo->setPixmap(QPixmap(":/images/vcglogo.png"));
    vcgLogo->setAlignment(Qt::AlignTop);

    QLayout * universityLayout = initUniversityLayout();
    QLayout * museumLayout = initMuseumLayout();
    QLayout * hpButtonLayout = initHPButtonLayout();

    QHBoxLayout * logosLayout = new QHBoxLayout();
    logosLayout->addStretch(1);
    logosLayout->addWidget(vcgLogo);
    logosLayout->addStretch(1);
    logosLayout->addLayout(universityLayout);
    logosLayout->addStretch(1);
    logosLayout->addLayout(museumLayout);
    logosLayout->addStretch(1);
    logosLayout->addLayout(hpButtonLayout);
    logosLayout->addStretch(1);

    return logosLayout;
}

QLayout * AboutDlg::initUniversityLayout()
{
    // Lay out the university logos.

    QLabel * minhoLogo = new QLabel();
    minhoLogo->setMaximumSize(160, 120);
    minhoLogo->setPixmap(QPixmap(":/images/minho.jpeg"));
    minhoLogo->setAlignment(Qt::AlignCenter);

    QLabel * ucscLogo = new QLabel();
    ucscLogo->setMaximumSize(160, 44);
    ucscLogo->setPixmap(QPixmap(":/images/ucsantacruz_resize.png"));
    ucscLogo->setAlignment(Qt::AlignCenter);

    QVBoxLayout * universityLayout = new QVBoxLayout();
    universityLayout->addWidget(minhoLogo);
    universityLayout->addStretch(1);
    universityLayout->addWidget(ucscLogo);

    return universityLayout;
}

QLayout * AboutDlg::initMuseumLayout()
{
    // Lay out the IMLS and CHI logos.

    QLabel * ilmsLogo = new QLabel();
    ilmsLogo->setMaximumSize(211, 86);
    ilmsLogo->setPixmap(QPixmap(":/images/logo_imls.gif"));
    ilmsLogo->setAlignment(Qt::AlignCenter);

    QLabel * chiLogo = new QLabel();
    chiLogo->setMaximumSize(211, 80);
    chiLogo->setPixmap(QPixmap(":/images/chi-logo.png"));
    chiLogo->setAlignment(Qt::AlignCenter);

    QVBoxLayout * museumLayout = new QVBoxLayout();
    museumLayout->addWidget(ilmsLogo);
    museumLayout->addStretch(1);
    museumLayout->addWidget(chiLogo);

    return museumLayout;
}

QLayout * AboutDlg::initHPButtonLayout()
{
    // Lay out the HP logo and OK button

    QLabel * hpLogo = new QLabel();
    hpLogo->setMaximumSize(141, 116);
    hpLogo->setPixmap(QPixmap(":/images/hptran.png"));
    hpLogo->setAlignment(Qt::AlignCenter);

    QLayout * buttonLayout = initButtonLayout();

    QVBoxLayout * hpButtonLayout = new QVBoxLayout();
    hpButtonLayout->addWidget(hpLogo);
    hpButtonLayout->addStretch(1);
    hpButtonLayout->addLayout(buttonLayout);
    hpButtonLayout->addStretch(1);

    return hpButtonLayout;
}

QLayout * AboutDlg::initButtonLayout()
{
    // The OK button is in a horizontal layout with
    // stretchable space on either side to keep it centered.

    okButton = new QPushButton("OK");
    okButton->setDefault(true);
    okButton->setMaximumSize(100, 30);
    okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QHBoxLayout * buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(okButton);
    buttonLayout->addStretch(1);

    return buttonLayout;
}

/*====================================================================
 *
 * Private method to connect signals and slots
 *
 *===================================================================*/

void AboutDlg::setConnections()
{
    // Connect the links in the User Guide link and the main description
    // to slots that will display them.

    connect(guideLabel, SIGNAL(linkActivated(QString)), this, SLOT(showManual(QString)));
    connect(descLabel, SIGNAL(linkActivated(QString)), this, SLOT(showURL(QString)));
	connect(titleLabel, SIGNAL(linkActivated(QString)), this, SLOT(showURL(QString)));
    connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
}

/*====================================================================
 *
 * Private text initialization methods
 *
 *===================================================================*/

char * AboutDlg::getAboutDialogTitle()
{
    return "About RTIViewer";
}

char * AboutDlg::getTitleHTML()
{
    return "<!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 4.0//EN' 'http://www.w3.org/TR/REC-html40/strict.dtd'>\n"
            "<html><head />"
            "<body style=\" font-family:'MS Shell Dlg 2'; font-style:normal; font-size:16pt; font-weight:600;\">"
            "<p align='center'>RTIViewer v1.1.0 <br /> <a href = 'http://culturalheritageimaging.org/What_We_Offer/Downloads/View/index.html' style = \"font-weight: 100; font-size: 14pt;\">Web page</a></p>"
            "</body></html>";
}

char * AboutDlg::getGplHTML()
{
    return "<!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 4.0//EN' 'http://www.w3.org/TR/REC-html40/strict.dtd'>\n"
            "<html><head />"
            "<body style=\" font-family:'MS Shell Dlg 2'; font-style:normal; font-size:12pt; font-weight:400;\">"
            "<p align='center'>Protected by GPL 3</p>"
            "</body></html>";
}

char * AboutDlg::getGuideHTML()
{
    return "<!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 4.0//EN' 'http://www.w3.org/TR/REC-html40/strict.dtd'>\n"
            "<html><head />"
            "<body style=\" font-family:'MS Shell Dlg 2'; font-style:normal; font-size:14pt; font-weight:400;\">"
            "<p><a href='manual.pdf'><span style=' text-decoration: underline; color:#0000ff;'>User Guide</span></a></p>"
            "</body></html>";
}

char * AboutDlg::getDescHTML()
{
    return "<!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 4.0//EN' 'http://www.w3.org/TR/REC-html40/strict.dtd'>\n"
            "<html>"
            "<head>"
            "<style type='text/css'>a {'text-decoration: underline; color:#0000ff;}</style>"
            "</head>"
            "<body style=\" font-family:'Lucida Grande'; font-style:normal; font-size:10pt; font-weight:400;\">"

            "<p>RTIViewer allows you to view and explore Reflectance Transformation "
                "Imaging (RTI) files. It is designed for cultural heritage and natural "
                "science applications and can be used with local and remote (via HTTP) "
                "files. For more information about RTI, as well as a user guide and "
                "software updates, see "
                "<a href='http://culturalheritageimaging.org/downloads'>http://culturalheritageimaging.org/downloads</a>.</p>"

            "<p>RTIViewer was primarily developed by the "
                "<a href='http://vcg.isti.cnr.it'>Visual Computing Laboratory (VCG)</a> "
                "of the Italian National Research Council's (CNR) Institute for "
                "Information Science and Technology (ISTI). "
                "<a href='http://culturalheritageimaging.org'>Cultural Heritage Imaging "
                "(CHI)</a> provided user requirements, documentation, and testing, as "
                "well as financing. Majority funding came from the "
                "<a href='http://www.imls.gov/'>Institute of Museum and Library Services "
                "(IMLS)</a>, Award Number LG-25-06-010706.</p>"

            "<p>RTIViewer also contains significant software and design contributions "
                "from the University of California, Santa Cruz, the Universidade do "
                "Minho in Portugal, Tom Malzbender of HP Labs, and CHI. Version 1.1 "
                "was organized by CHI, majority funded from an additional IMLS grant, "
                "and developed by Ronald Bourret, Gianpaolo Palma, Leif Isaksen, and "
                "several volunteers.</p>"

            "<p>This software is available under the Gnu General Public License, "
                "version 3. To receive a copy of the source code, please email "
                "<a href='mailto:info@c-h-i.org'>info@c-h-i.org</a>.</p>"

            "<p>"
            "<table>"
                "<tr>"
                    "<td>Authors:<br />Developers:</td>"
                    "<td>Massimiliano Corsini, Gianpaolo Palma<br />Massimiliano Corsini, Gianpaolo Palma, Joao Barbosa (Univ. Minho)</td>"
                "</tr>"
            "</table>"
            "</p>"
            "</body></html>";
}
