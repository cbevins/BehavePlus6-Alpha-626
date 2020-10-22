//------------------------------------------------------------------------------
/*! \file humiditydialog.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2018 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus relative humidity dialog class methods.
 */

// Custom include files
#include "appmessage.h"
#include "appsiunits.h"
#include "apptranslator.h"
#include "appwindow.h"
#include "humiditydialog.h"
#include "xfblib.h"

// Qt include files
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>

//------------------------------------------------------------------------------
/*! \brief HumidityDialog constructor.
 *
 *  \param parent       Pointer to the parent QWidget.
 *  \param captionKey   Dialog caption translator key.
 *  \param name         Widget internal name.
 *  \param acceptKey    Translator key for the accept button
 *  \param rejectKey    Translator key for the reject button
 */

HumidityDialog::HumidityDialog( QWidget *parent, const QString &captionKey,
        const char *name, const QString &acceptKey, const QString &rejectKey ) :
    AppTabDialog( appWindow(), captionKey, name, acceptKey, rejectKey ),
    m_parent(parent),
    m_pageList(0)
{
    // Create the HumidityPage list
    m_pageList = new QPtrList<HumidityPage>();
    Q_CHECK_PTR( m_pageList );
    m_pageList->setAutoDelete( false );

    // Dry bulb temperature, wet bulb temperature, elevation
    HumidityPage *page;
    page = addPage( 1, "HumidityDialog:Tab1", 8,
        "FlatheadLake1.png", "Flathead Lake", "relativeHumidity.html" );

    // Dry bulb temperature, wet bulb depression, elevation
    page = addPage( 2, "HumidityDialog:Tab2", 8,
        "FlatheadLake2.png", "Flathead Lake", "relativeHumidity.html" );

    // Dry bulb and dew point temperatures
    page = addPage( 3, "HumidityDialog:Tab3", 4,
        "FlatheadLake3.png", "Flathead Lake", "relativeHumidity.html" );

    return;
}

//------------------------------------------------------------------------------
/*! \brief HumidityDialog destructor.
 */

HumidityDialog::~HumidityDialog( void )
{
    delete m_pageList;  m_pageList = 0;
    return;
}

bool HumidityDialog::event( QEvent *e )
{
    // Just catch key press events on the BpDocEntry
    if ( e->type() == QEvent::KeyPress )
    {
        // Backward field movements
        QKeyEvent *k = (QKeyEvent *)e;
        if ( k->key() == Key_Return )
		{
			return true;
		}
	}
    return( QWidget::event( e ) );
}

//------------------------------------------------------------------------------
/*! \brief Adds an HumidityPage to the HumidityDialog.
 *
 *  \param tabText      Tab translator key.
 *  \param rows         Number of rows
 *  \param pictureFile  Picture file name.
 *  \param pictureName  Picture name.
 *  \param htmlFile     Page's initial HTML file base name.
 *
 *  \return Pointer to the new HumidityPage.
 */

HumidityPage *HumidityDialog::addPage( int method, const QString &tabKey,
        int rows, const QString &pictureFile, const QString &pictureName,
        const QString &htmlFile )
{
    // Create the new page
    HumidityPage *page = new HumidityPage( method, this, rows, pictureFile,
        pictureName, htmlFile, QString( "%1:%2" ).arg( name() ).arg( tabKey ) );
    checkmem( __FILE__, __LINE__, page, "HumidityPage page", 1 );
    // Add it to the dialog's page list
    m_pageList->append( page );
    // Add the tab
    if ( ! tabKey.isNull() && ! tabKey.isEmpty() )
    {
        QString tabText("");
        translate( tabText, tabKey );
        addTab( page, tabText );
    }
    return( page );
}

//------------------------------------------------------------------------------
/*! \brief HumidityPage constructor.
 *
 *  \param dialog       Pointer to parent HumidityDialog
 *  \param rows         Number of rows
 *  \param pictureFile  Picture file name
 *  \param pictureName  Picture name
 *  \param htmlFile     Page's initial HTML file base name
 *  \param name         Widget's internal name
 *
 *  /note The number of rows is fixed to the maximum for any page (12),
 *  as a work-around to the GridLayout tendency to give different row heights
 *  depending on the number of declared rows.
 */

HumidityPage::HumidityPage( int method, HumidityDialog *dialog, int rows,
        const QString &pictureFile, const QString &pictureName,
        const QString &htmlFile, const char *name ) :
    AppPage( dialog, pictureFile, pictureName, htmlFile, name ),
    m_dialog(dialog),
    m_gridFrame(0),
    m_gridLayout(0),
	m_calculateButton1( 0 ),
	m_calculateButton2( 0 ),
	m_calculateButton3( 0 ),
    m_method(method),
    m_rows(0),
	m_english(true)
{
    // Initialization
    int i;
    for ( i=0; i<6; i++ )
    {
        m_name[i] = 0;
        m_units[i] = 0;
    }
    for ( i=0; i<3; i++ )
    {
        m_spin[i] = 0;
        m_output[i] = 0;
    }

    // Hidden frame to contain a grid layout.
    m_gridFrame = new QFrame( m_contentFrame, QString( "%1:Frame" ).arg( name ) );
    Q_CHECK_PTR( m_gridFrame );
    m_gridFrame->setFrameStyle( QFrame::NoFrame );

    // Create grid layout with rows+1 rows, 3 columns, 0 border, and 2 spacing.
    m_gridLayout = new QGridLayout( m_gridFrame, rows, 3, 0, 2,
        QString( "%1:Grid" ).arg( name ) );
    Q_CHECK_PTR( m_gridLayout );

    // Radio buttons appear on grid's top row and span all columns
    m_group = new QButtonGroup( 2, Qt::Horizontal, "Units",
        m_gridFrame, "buttonGroup" );
    Q_CHECK_PTR( m_group );
    m_group->setAlignment( AlignHCenter );
    m_gridLayout->addMultiCellWidget( m_group, 0, 0, 0, 3, AlignCenter );

    m_radio[0] = new QRadioButton( "oF and ft", m_group, "radioEnglish" );
    Q_CHECK_PTR( m_radio[0] );
    m_radio[0]->setChecked( true );
    connect( m_radio[0], SIGNAL( clicked() ),
             this,       SLOT( useEnglish() ) );

    m_radio[1] = new QRadioButton( "oC and m", m_group, "radioMetric" );
    Q_CHECK_PTR( m_radio[1] );
    connect( m_radio[1], SIGNAL( clicked() ),
             this,       SLOT( useMetric() ) );

	// Pane 1: Dry bulb, wet bulb, and site elevation
    if ( method == 1 )
    {
        addInput( "HumidityDialog:Field:Db", "oF", -40, 120, 80, 1, 10 );
        connect( m_spin[0], SIGNAL(valueChanged(int)), this, SLOT( dbChanged1() ) );
        addInput( "HumidityDialog:Field:Wb", "oF", -40, 120, 60, 1, 10 );
        connect( m_spin[1], SIGNAL(valueChanged(int)), this, SLOT( wbChanged1() ) );
        addInput( "HumidityDialog:Field:El", "ft", -1000, 30000, 0, 100, 1000 );
        connect( m_spin[2], SIGNAL(valueChanged(int)), this, SLOT( seChanged1() ) );

		m_rows++;
		m_calculateButton1 = new QPushButton( "Calculate", m_gridFrame, "m_calculateButton1" );
		m_gridLayout->addMultiCellWidget( m_calculateButton1, m_rows, m_rows, 0, 2 );
		connect( m_calculateButton1, SIGNAL(clicked()), this, SLOT(calculate1()) );

        addOutput( 0, "HumidityDialog:Field:Wd", "oF" );
        addOutput( 1, "HumidityDialog:Field:Dp", "oF" );
        addOutput( 2, "HumidityDialog:Field:Rh", "%" );
    }
	// Pane 2: Dry bulb, depression, site elevation
    else if ( method == 2 )
    {
        addInput( "HumidityDialog:Field:Db", "oF", -40, 120, 80, 1, 10 );
        connect( m_spin[0], SIGNAL(valueChanged(int)), this, SLOT( dbChanged2() ) );
        addInput( "HumidityDialog:Field:Wd", "oF", 0, 54, 20, 1, 10 );
        connect( m_spin[1], SIGNAL(valueChanged(int)), this, SLOT( wdChanged2() ) );
        addInput( "HumidityDialog:Field:El", "ft", -1000, 30000, 0, 100, 1000 );
        connect( m_spin[2], SIGNAL(valueChanged(int)), this, SLOT( seChanged2() ) );

		m_rows++;
		m_calculateButton2 = new QPushButton( "Calculate", m_gridFrame, "m_calculateButton2" );
		m_gridLayout->addMultiCellWidget( m_calculateButton2, m_rows, m_rows, 0, 2 );
		connect( m_calculateButton2, SIGNAL(clicked()), this, SLOT(calculate2()) );

        addOutput( 0, "HumidityDialog:Field:Wb", "oF" );
        addOutput( 1, "HumidityDialog:Field:Dp", "oF" );
        addOutput( 2, "HumidityDialog:Field:Rh", "%" );
    }
	// Pane 3: dry bulb and dew point temperature
    else
    {
        addInput( "HumidityDialog:Field:Db", "oF", -40, 120, 80, 1, 10 );
        connect( m_spin[0], SIGNAL(valueChanged(int)), this, SLOT( dbChanged3() ) );
        addInput( "HumidityDialog:Field:Dp", "oF", -40, 120, 45, 1, 10 );
        connect( m_spin[1], SIGNAL(valueChanged(int)), this, SLOT( dpChanged3() ) );

		m_rows++;
		m_calculateButton3 = new QPushButton( "Calculate", m_gridFrame, "m_calculateButton3" );
		m_gridLayout->addMultiCellWidget( m_calculateButton3, m_rows, m_rows, 0, 2 );
		connect( m_calculateButton3, SIGNAL(clicked()), this, SLOT(calculate3()) );

		addOutput( 0, "HumidityDialog:Field:Rh", "%" );
    }

    // Make sure the output entry fields are big enough.
    QFontMetrics fm( m_output[0]->font() );
    int w = fm.width( "MMMMM" );
    for ( i=0; i<3; i++ )
    {
        if ( m_spin[i] )
        {
            m_spin[i]->setFixedWidth( w );
        }
        if ( m_output[i] )
        {
            m_output[i]->setFixedWidth( w );
        }
    }

    // Initialize
    m_spin[0]->setFocus();
    if ( method == 1 )
    {
        dbChanged1();
        wbChanged1();
        seChanged1();
		update1(2);
    }
    else if ( method == 2 )
    {
        dbChanged2();
        wdChanged2();
        seChanged2();
		update2(2);
    }
    else
    {
        dbChanged3();
        dpChanged3();
		update3(3);
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief HumidityPage destructor.
 */

HumidityPage::~HumidityPage( void )
{
    int i;
    for ( i=0; i<6; i++ )
    {
        if ( m_name[i] )
        {
            delete m_name[i];   m_name[i] = 0;
        }
        if ( m_units[i] )
        {
            delete m_units[i];  m_units[i] = 0;
        }
    }
    for ( i=0; i<3; i++ )
    {
        if ( m_spin[i] )
        {
            delete m_spin[i];   m_spin[i] = 0;
        }
        if ( m_output[i] )
        {
            delete m_output[i]; m_output[i] = 0;
        }
    }
	if ( m_calculateButton1 )
	{
		delete m_calculateButton1;	m_calculateButton1 = 0;
	}
	if ( m_calculateButton2 )
	{
		delete m_calculateButton2;	m_calculateButton2 = 0;
	}
	if ( m_calculateButton3 )
	{
		delete m_calculateButton3;	m_calculateButton3 = 0;
	}
    return;
}

//------------------------------------------------------------------------------
/*! \brief Adds the name QLabel, units QLabel, and input SpinBox widgets
 *  for one input item.
 */

void HumidityPage::addInput( const QString &nameKey, const QString &units,
    int minVal, int maxVal, int initVal, int lineStep, int pageStep )
{
    int col = 0;
    int id = m_rows;
    m_rows++;
    // Names are in the first column
    QString name;
    translate( name, nameKey );
    m_name[id] = new QLabel( name, m_gridFrame );
    Q_CHECK_PTR( m_name[id] );
    m_name[id]->setFixedSize( m_name[id]->sizeHint() );
    m_gridLayout->addWidget( m_name[id], m_rows, col, AlignLeft );

    // Spin boxes are in the second column
    m_spin[id] = new QSpinBox( m_gridFrame );
    Q_CHECK_PTR( m_spin[id] );
    m_gridLayout->addWidget( m_spin[id], m_rows, col+1, AlignLeft );
    m_spin[id]->setRange( minVal, maxVal );
    m_spin[id]->setValue( initVal );
    m_spin[id]->setSteps( lineStep, pageStep );

    // Unit labels are in the third column
    QString spacer = ( col < 6 ) ? "    " : "";
    m_units[id] = 0;
    m_units[id] = new QLabel( units+spacer, m_gridFrame );
    Q_CHECK_PTR( m_units[id] );
    m_units[id]->setFixedSize( m_units[id]->sizeHint() );
    m_gridLayout->addWidget( m_units[id], m_rows, col+2, AlignLeft );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Adds the name QLabel, units QLabel, and results QEntry widgets
 *  for one input item.
 */

void HumidityPage::addOutput( int id1, const QString &nameKey,
        const QString &units )
{
    int col = 0;
    int id2 = id1 + 3;	// = m_rows;
    m_rows++;
    // Names are in the first column
    QString name;
    translate( name, nameKey );
    m_name[id2] = new QLabel( name, m_gridFrame );
    Q_CHECK_PTR( m_name[id2] );
    m_name[id2]->setFixedSize( m_name[id2]->sizeHint() );
    m_gridLayout->addWidget( m_name[id2], m_rows, col, AlignLeft );

    // Output read-only entry field are in the second column
    m_output[id1] = new QLineEdit( m_gridFrame );
    Q_CHECK_PTR( m_output[id1] );
    m_gridLayout->addWidget( m_output[id1], m_rows, col+1 );
    m_output[id1]->setText( "MMMMM" );
    m_output[id1]->setReadOnly( true );

    // Output units are in the third column
    QString spacer = ( col < 6 ) ? "    " : "";
    m_units[id2] = new QLabel( units+spacer, m_gridFrame );
    Q_CHECK_PTR( m_units[id2] );
    m_units[id2]->setFixedSize( m_units[id2]->sizeHint() );
    m_gridLayout->addWidget( m_units[id2], m_rows, col+2, AlignLeft );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Validates method 1 entry fields and fills in the result.
 *
 *	\param whom Indicates the event that invoked this callback
 *		0 = change to English units
 *		1 = change to Metric units
 *		2 = dry bulb value changed
 *		3 = wet bulb value changed
 *		4 = site elevation value changed
 *  \return TRUE if entries are Ok, FALSE if errors are found.
 */

bool HumidityPage::update1( int whom )
{
    // Block all signals
    m_spin[0]->blockSignals( true );
    m_spin[1]->blockSignals( true );
    m_spin[2]->blockSignals( true );

    // Get current input values
    double db = (double) m_spin[0]->value();
    double wb = (double) m_spin[1]->value();
    double se = (double) m_spin[2]->value();

    // If changing to English units...
    if ( whom == 0 )
    {
        m_units[0]->setText( "oF" );
        m_units[1]->setText( "oF" );
        m_units[2]->setText( "ft" );
        m_units[3]->setText( "oF" );
        m_units[4]->setText( "oF" );

        m_spin[0]->setRange( -40, 120 );
        m_spin[1]->setRange( -40, 120 );
        m_spin[2]->setRange( -1000, 30000 );

        db = 32. + 9. * db / 5.;
        wb = 32. + 9. * wb / 5.;
        se = 3.28084 * se;

        m_spin[0]->setValue( (int) ( 0.5 + db ) );
        m_spin[1]->setValue( (int) ( 0.5 + wb ) );
        m_spin[2]->setValue( (int) ( 0.5 + se ) );

        // Lastly, update WD, DP, and RH, using the english units
        updateRh1( db, wb, se, false );
    }
    // else if changing to metric units...
    else if ( whom == 1 )
    {
        // First, update RH using the current english units
        // but displaying metric outputs
        updateRh1( db, wb, se, true );

        // Change to metric settings
        m_units[0]->setText( "oC" );
        m_units[1]->setText( "oC" );
        m_units[2]->setText( "m" );
        m_units[3]->setText( "oC" );
        m_units[4]->setText( "oC" );

        m_spin[0]->setRange( -40, 50 );
        m_spin[1]->setRange( -40, 50 );
        m_spin[2]->setRange( -300, 9000 );

        db = 5. * ( db - 32. ) / 9.;
        wb = 5. * ( wb - 32. ) / 9.;
        se = se / 3.28084;

        m_spin[0]->setValue( (int) ( 0.5 + db ) );
        m_spin[1]->setValue( (int) ( 0.5 + wb ) );
        m_spin[2]->setValue( (int) ( 0.5 + se ) );
    }
    // else if an input change...
    else
    {
        // If current units are metric ...
        if ( m_radio[1]->isChecked() )
        {
            // Get English equivalents
            db = 32. + 9. * db / 5.;	// 5. * ( db - 32. ) / 9.;
            wb = 32. + 9. * wb / 5.;	// 5. * ( wb - 32. ) / 9.;
            se = se * 3.28084;
        }
        // Lastly, update using the english units and current output units
        updateRh1( db, wb, se, m_radio[1]->isChecked() );
    }
    // Enable all signals
    m_spin[0]->blockSignals( false );
    m_spin[1]->blockSignals( false );
    m_spin[2]->blockSignals( false );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Validates method2 entry fields and fills in the result.
 *
 *  \return TRUE if entries are Ok, FALSE if errors are found.
 */

bool HumidityPage::update2( int whom )
{
    // Block all signals
    m_spin[0]->blockSignals( true );
    m_spin[1]->blockSignals( true );
    m_spin[2]->blockSignals( true );

    // Get current input values
    double db = (double) m_spin[0]->value();
    double wd = (double) m_spin[1]->value();
    double se = (double) m_spin[2]->value();

    // If changing to English units...
    if ( whom == 0 )
    {
        m_units[0]->setText( "oF" );
        m_units[1]->setText( "oF" );
        m_units[2]->setText( "ft" );
        m_units[3]->setText( "oF" );
        m_units[4]->setText( "oF" );

        m_spin[0]->setRange( -40, 120 );
        m_spin[1]->setRange(   0,  54 );
        m_spin[2]->setRange( -1000, 30000 );

        db = 32. + 9. * db / 5.;
        wd = 9. * wd / 5.;
        se = 3.28084 * se;

        m_spin[0]->setValue( (int) ( 0.5 + db ) );
        m_spin[1]->setValue( (int) ( 0.5 + wd ) );
        m_spin[2]->setValue( (int) ( 0.5 + se ) );

        // Lastly, update DB, WD, and RH using the english input & output units
        updateRh2( db, wd, se, false );
    }
    // else if changing to metric units units...
    else if ( whom == 1 )
    {
        // Firstly, update DB, WD, and RH using current english input units
        // but displaying metric output units
        updateRh2( db, wd, se, true );

        // Change to metric settings
        m_units[0]->setText( "oC" );
        m_units[1]->setText( "oC" );
        m_units[2]->setText( "m" );
        m_units[3]->setText( "oC" );
        m_units[4]->setText( "oC" );

        m_spin[0]->setRange( -40, 50 );
        m_spin[1]->setRange(   0, 30 );
        m_spin[2]->setRange( -300, 9000 );

        db = 5. * ( db - 32. ) / 9.;
        wd = 5. * wd / 9.;
        se = se / 3.28084;

        m_spin[0]->setValue( (int) ( 0.5 + db ) );
        m_spin[1]->setValue( (int) ( 0.5 + wd ) );
        m_spin[2]->setValue( (int) ( 0.5 + se ) );
    }
    // else if an input change...
    else
    {
        // If current units are metric ...
        if ( m_radio[1]->isChecked() )
        {
            // Get English equivalents
            db = 32. + 9. * db / 5.;	// 5. * ( db - 32. ) / 9.;
            wd = 9. * wd / 5.;
            se = se * 3.28084;
        }
        // Lastly, update using the english units and current output units
        updateRh2( db, wd, se, m_radio[1]->isChecked() );
    }
    // Enable all signals
    m_spin[0]->blockSignals( false );
    m_spin[1]->blockSignals( false );
    m_spin[2]->blockSignals( false );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Validates method 3 entry fields and fills in the result.
 *
 *  \return TRUE if entries are Ok, FALSE if errors are found.
 */

bool HumidityPage::update3( int whom )
{
    // Block all signals
    m_spin[0]->blockSignals( true );
    m_spin[1]->blockSignals( true );

    // Get current input values
    double db = (double) m_spin[0]->value();
    double dp = (double) m_spin[1]->value();

    // If changing to English units...
    if ( whom == 0 )
    {
        m_units[0]->setText( "oF" );
        m_units[1]->setText( "oF" );

        m_spin[0]->setRange( -40, 120 );
        m_spin[1]->setRange( -40, 120 );

        db = 32. + 9. * db / 5.;
        dp = 32. + 9. * dp / 5.;

        m_spin[0]->setValue( (int) ( 0.5 + db ) );
        m_spin[1]->setValue( (int) ( 0.5 + dp ) );

        // Lastly, update RH using the english input and output units
        updateRh3( db, dp );
    }
    // else if changing to metric units units...
    else if ( whom == 1 )
    {
        // Firstly, update RH using current english input units
        // but displaying metric output units
        updateRh3( db, dp );

        // Change to metric settings
        m_units[0]->setText( "oC" );
        m_units[1]->setText( "oC" );

        m_spin[0]->setRange( -40, 50 );
        m_spin[1]->setRange( -40, 50 );

        db = 5. * ( db - 32. ) / 9.;
        dp = 5. * ( dp - 32. ) / 9.;

        m_spin[0]->setValue( (int) ( 0.5 + db ) );
        m_spin[1]->setValue( (int) ( 0.5 + dp ) );
    }
    // else if an input change...
    else
    {
        // If dry bulb has been changed ...
        if ( whom == 2 )
        {
            // If dry bulb has been changed so it is less than the dew pt ...
            if ( db < dp )
            {
                // Set the dew pt to the dry bulb
                m_spin[1]->setValue( m_spin[0]->value() );
            }
        }
        // If dew pt has been changed ...
        else if ( whom == 3 )
        {
            // If wet bulb has been changed so its above the dry bulb ...
            if ( db < dp )
            {
                // Set dry bulb to the dew pt
                m_spin[0]->setValue( m_spin[1]->value() );
            }
        }
        // If current units are metric ...
        if ( m_radio[1]->isChecked() )
        {
            // Get English equivalents
            db = 32. + 9. * db / 5.;	// 5. * ( db - 32. ) / 9.;
            dp = 32. + 9. * dp / 5.;	// 5. * ( wb - 32. ) / 9.;
        }
        // Lastly, update using the english units
        updateRh3( db, dp );
    }
    // Enable all signals
    m_spin[0]->blockSignals( false );
    m_spin[1]->blockSignals( false );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Performs the method 1 relative humidity and dew point computations.
 *
 *  \return TRUE if entries are Ok, FALSE if errors are found.
 */

void HumidityPage::updateRh1( double db, double wb, double se,
        bool metricResults )
{
    // Calculate dew point and RH
    double dp = FBL_DewPointTemperature( db, wb, se );  // oF
    double rh = 100. * FBL_RelativeHumidity( db, dp );  // fraction
    double wd = db - wb;

    // If using metric units, convert from english
    if ( metricResults )
    {
        dp = 5. * ( dp - 32. ) / 9.;
        wd = 5. * wd / 9.;
    }

    // Display the new wet bulb depression
    QString qStr;
    qStr.setNum( (int) (0.5 + wd) );
    m_output[0]->setText( qStr );

    // Display the new dew point temperature
    qStr.setNum( (int) (0.5 + dp) );
    m_output[1]->setText( qStr );

    // Display the new relative humidity
    qStr.setNum( (int) (0.5 + rh) );
    m_output[2]->setText( qStr );
	unmark(3);		// Unmark all 3 output fields
    return;
}

//------------------------------------------------------------------------------
/*! \brief Performs the method 2 relative humidity and dew point computations.
 *
 *  \return TRUE if entries are Ok, FALSE if errors are found.
 */

void HumidityPage::updateRh2( double db, double wd, double se,
        bool metricResults )
{
    // Calculate dew point and RH
    double wb = db - wd;
    double dp = FBL_DewPointTemperature( db, wb, se );  // oF
    double rh = 100. * FBL_RelativeHumidity( db, dp );  // fraction

    // If using metric units, convert from english
    if ( metricResults )
    {
        dp = 5. * ( dp - 32. ) / 9.;
        wb = 5. * ( wb - 32. ) / 9.;
    }

    // Display the new wet bulb temperature
    QString qStr;
    qStr.setNum( (int) (0.5 + wb) );
    m_output[0]->setText( qStr );

    // Display the new dew point temperature
    qStr.setNum( (int) (0.5 + dp) );
    m_output[1]->setText( qStr );

    // Display the new relative humidity
    qStr.setNum( (int) (0.5 + rh) );
    m_output[2]->setText( qStr );
	unmark(3);		// Unmark all 3 output fields
    return;
}

//------------------------------------------------------------------------------
/*! \brief Performs the method 3 relative humidity and dew point computations.
 *
 *  \return TRUE if entries are Ok, FALSE if errors are found.
 */

void HumidityPage::updateRh3( double db, double dp )
{
    // Calculate dew point and RH
    double rh = 100. * FBL_RelativeHumidity( db, dp );  // fraction

    // Display the new relative humidity
    QString qStr;
    qStr.setNum( (int) (0.5 + rh) );
    m_output[0]->setText( qStr );
	unmark(1);		// Unmark just the 1 output field
    return;
}

//------------------------------------------------------------------------------
/*! \brief Input widget slots.
 *
 *  \return TRUE if entries are Ok, FALSE if errors are found.
 */

bool HumidityPage::calculate1( void )
{
    return( update1(2) );
}

bool HumidityPage::calculate2( void )
{
    return( update2(2) );
}

bool HumidityPage::calculate3( void )
{
    return( update3(2) );
}

bool HumidityPage::dbChanged1( void )
{
    double db = (double) m_spin[0]->value();
    double wb = (double) m_spin[1]->value();
    // If dry bulb has been changed so it is less than the wet bulb ...
    if ( db < wb )
    {
        // Set the wet bulb to the dry bulb
        m_spin[1]->setValue( m_spin[0]->value() );
    }
	mark(3);				// Mark all 3 output fields
	return true;
}

bool HumidityPage::dbChanged2( void )
{
	mark(3);				// Mark all 3 output fields
    return true;
}

bool HumidityPage::dbChanged3( void )
{
	mark(1);				// Mark just 1 output field
	return true;
    //return( update3(2) );
}

bool HumidityPage::dpChanged3( void )
{
	mark(1);				// Mark just 1 output field
	return true;
    //return( update3(3) );
}

bool HumidityPage::seChanged1( void )
{
	mark(3);				// Mark all 3 output fields
    return true;
}

bool HumidityPage::seChanged2( void )
{
	mark(3);				// Mark all 3 output fields
    return true;
}

bool HumidityPage::wbChanged1( void )
{
    double db = (double) m_spin[0]->value();
    double wb = (double) m_spin[1]->value();
    // If wet bulb has been changed so its above the dry bulb ...
    if ( db < wb )
    {
        // Set dry bulb to the wet bulb
        m_spin[0]->setValue( m_spin[1]->value() );
    }
	mark(3);				// Mark all 3 output fields
    return true;
}

bool HumidityPage::wdChanged2( void )
{
	mark(3);				// Mark all 3 output fields
    return true;
}

void HumidityPage::mark( int nOutputs )
{
    // Set up the masked input palette.
    QPalette Masked( m_spin[0]->palette() );
    QString baseColor = "lightBlue";
    Masked.setColor( QPalette::Normal,   QColorGroup::Base, baseColor );
    Masked.setColor( QPalette::Disabled, QColorGroup::Base, baseColor );
    Masked.setColor( QPalette::Active,   QColorGroup::Base, baseColor );
    Masked.setColor( QPalette::Inactive, QColorGroup::Base, baseColor );
	for ( int i=0; i<nOutputs; i++ )
	{
		m_output[i]->setPalette( Masked );
	}
}

void HumidityPage::unmark( int nOutputs )
{
    // Assume entry field 0 has the normal palette since its never masked.
    QPalette Normal( m_spin[0]->palette() );
	for ( int i=0; i<nOutputs; i++ )
	{
		m_output[i]->setPalette( Normal );
	}
}

//------------------------------------------------------------------------------
/*! \brief Units radio button slots.
 *
 *  \return TRUE if entries are Ok, FALSE if errors are found.
 */

bool HumidityPage::useEnglish( void )
{
	// Convert only if we're not already in english units
	if ( ! m_english )
	{
		if ( m_method == 1 )
		{
			update1(0);
		}
		else if ( m_method == 2 )
		{
			update2(0);
		}
		else if ( m_method == 3 )
		{
			update3(0);
		}
		m_english = true;
	}
    return( true );
}

bool HumidityPage::useMetric( void )
{
	// Convert only if we are currently in english
	if ( m_english )
	{
		if ( m_method == 1 )
		{
			update1(1);
		}
		else if ( m_method == 2 )
		{
			update2(1);
		}
		else if ( m_method == 3 )
		{
			update3(1);
		}
		m_english = false;
	}
    return( true );
}

//------------------------------------------------------------------------------
//  End of humiditydialog.cpp
//------------------------------------------------------------------------------

