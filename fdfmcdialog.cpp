//------------------------------------------------------------------------------
/*! \file fdfmcdialog.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus fine dead fuel moisture correction tool dialog class
 *  methods.
 */

// Custom include files
#include "appfilesystem.h"
#include "appmessage.h"
#include "appproperty.h"
#include "apptranslator.h"
#include "fdfmcdialog.h"

// Qt include files
#include <qcombobox.h>
#include <qfiledialog.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>

// Standard include files
#include <time.h>

/*! \var FieldNameKey
 *  \brief Array of translator keys for the 3 entry field labels,
 *  Convert button, and initial results text.  The translated results are
 *  stored in the FieldName[] array.
 */
static QString FdfmcName[Rows] =
    { "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" };
static const QString FdfmcNameKey[Rows] =
{
    "FdfmcDialog:Fdfmc:DryBulb",
    "FdfmcDialog:Fdfmc:RH",
    "FdfmcDialog:Fdfmc:Reference",
    "FdfmcDialog:Fdfmc:Blank",
    "FdfmcDialog:Fdfmc:Month",
    "FdfmcDialog:Fdfmc:TimeOfDay",
    "FdfmcDialog:Fdfmc:Elevation",
    "FdfmcDialog:Fdfmc:Slope",
    "FdfmcDialog:Fdfmc:Aspect",
    "FdfmcDialog:Fdfmc:Shading",
    "FdfmcDialog:Fdfmc:Correction",
    "FdfmcDialog:Fdfmc:Blank",
    "FdfmcDialog:Fdfmc:Result",
    "FdfmcDialog:Fdfmc:Blank",
    "FdfmcDialog:Fdfmc:Description"
};

static const int AspValues = 4;
static const char *AspValue[AspValues] =
{
    "North",
    "East",
    "South",
    "West"
};

static const int DbValues = 6;
static const char *DbValue[DbValues] =
{
    "10 - 29 oF",
    "30 - 49 oF",
    "50 - 69 oF",
    "70 - 89 oF",
    "90 -109 oF",
    "> 109 oF"
};

static const int ElevValues = 3;
static const char *ElevValue[ElevValues] =
{
    "Below (1000 - 2000 ft)",
    "Level (within 1000 ft)",
    "Above (1000 - 2000 ft)"
};

static const int MonValues = 3;
static const char *MonValue[MonValues] =
{
    "May June July",
    "Feb Mar Apr Aug Sep Oct",
    "Nov Dec Jan"
};

static const int RhValues = 21;
static const char *RhValue[RhValues] =
{
    " 0 -  4 %",
    " 5 -  9 %",
    "10 - 14 %",
    "15 - 19 %",
    "20 - 24 %",
    "25 - 29 %",
    "30 - 34 %",
    "35 - 39 %",
    "40 - 44 %",
    "45 - 49 %",
    "50 - 54 %",
    "55 - 59 %",
    "60 - 64 %",
    "65 - 69 %",
    "70 - 74 %",
    "75 - 79 %",
    "80 - 84 %",
    "85 - 89 %",
    "90 - 94 %",
    "95 - 99 %",
    "100 %"
};

static const int SlpValues = 2;
static const char *SlpValue[SlpValues] =
{
    "0 - 30%",
    "31+ %"
};

static const int ShdValues = 2;
static const char *ShdValue[ShdValues] =
{
    "Exposed (< 50% shading)",
    "Shaded (>=50% shading)"
};

static const int TodValues = 6;
static const char *TodValue[TodValues] =
{
    "08:00 - 09:59",
    "10:00 - 11:59",
    "12:00 - 13:59",
    "14:00 - 15:59",
    "16:00 - 17:59",
    "18:00 - Sunset"
};

static const int Reference[DbValues][RhValues] =
{
    { 1, 2, 2, 3, 4, 5, 5, 6, 7, 8, 8, 8, 9, 9, 10, 11, 12, 12, 13, 13, 14 },
    { 1, 2, 2, 3, 4, 5, 5, 6, 7, 7, 7, 8, 9, 9, 10, 10, 11, 12, 13, 13, 13 },
    { 1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9,  9, 10, 11, 12, 12, 12, 13 },
    { 1, 1, 2, 2, 3, 4, 5, 5, 6, 7, 7, 8, 8, 8,  9, 10, 10, 11, 12, 12, 13 },
    { 1, 1, 2, 2, 3, 4, 4, 5, 6, 7, 7, 8, 8, 8,  9, 10, 10, 11, 12, 12, 13 },
    { 1, 1, 2, 2, 3, 4, 4, 5, 6, 7, 7, 8, 8, 8,  9, 10, 10, 11, 12, 12, 12 }
};

static const int Correction[36][18] =
{
    // May-Jun-Jul Exposed
    { 2, 3, 4, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 2, 3, 4 },
    { 3, 4, 4, 1, 2, 2, 1, 1, 2, 1, 1, 2, 1, 2, 2, 3, 4, 4 },
    { 2, 2, 3, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 2, 3, 4, 4 },
    { 1, 2, 2, 0, 0, 1, 0, 0, 1, 1, 1, 2, 2, 3, 4, 4, 5, 6 },
    { 2, 3, 3, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 2, 3, 3 },
    { 2, 3, 3, 1, 1, 2, 0, 1, 1, 0, 1, 1, 1, 1, 2, 2, 3, 3 },
    { 2, 3, 4, 1, 1, 2, 0, 0, 1, 0, 0, 1, 0, 1, 1, 2, 3, 3 },
    { 4, 5, 6, 2, 3, 4, 1, 1, 2, 0, 0, 1, 0, 0, 1, 1, 2, 2 },
    // May-Jun-Jul Shaded
    { 4, 5, 5, 3, 4, 5, 3, 3, 4, 3, 3, 4, 3, 4, 5, 4, 5, 5 },
    { 4, 4, 5, 3, 4, 5, 3, 3, 4, 3, 4, 4, 3, 4, 5, 4, 5, 6 },
    { 4, 4, 5, 3, 4, 5, 3, 3, 4, 3, 3, 4, 3, 4, 5, 4, 5, 5 },
    { 4, 5, 6, 3, 4, 5, 3, 3, 4, 3, 3, 4, 3, 4, 5, 4, 4, 5 },
    // Feb-Mar-Apr/Aug-Sep-Oct Exposed
    { 3, 4, 5, 1, 2, 3, 1, 1, 2, 1, 1, 2, 1, 2, 3, 3, 4, 5 },
    { 3, 4, 5, 3, 3, 4, 2, 3, 4, 2, 3, 4, 3, 3, 4, 3, 4, 5 },
    { 3, 4, 5, 1, 2, 3, 1, 1, 1, 1, 1, 2, 1, 2, 3, 3, 4, 5 },
    { 3, 3, 4, 1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 4, 5, 3, 4, 6 },
    { 3, 4, 5, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 4, 5 },
    { 3, 4, 5, 1, 2, 2, 0, 1, 1, 0, 1, 1, 1, 2, 2, 3, 4, 5 },
    { 3, 4, 5, 1, 2, 3, 1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 4, 5 },
    { 4, 5, 6, 3, 4, 5, 1, 2, 3, 1, 1, 1, 1, 1, 1, 3, 3, 4 },
    // Feb-Mar-Apr/Aug-Sep-Oct Shaded
    { 4, 5, 6, 4, 5, 5, 3, 4, 5, 3, 4, 5, 4, 5, 5, 4, 5, 6 },
    { 4, 5, 6, 3, 4, 5, 3, 4, 5, 3, 4, 5, 4, 5, 6, 4, 5, 6 },
    { 4, 5, 6, 3, 4, 5, 3, 4, 5, 3, 4, 5, 3, 4, 5, 4, 5, 6 },
    { 4, 5, 6, 4, 5, 6, 3, 4, 5, 3, 4, 5, 3, 4, 5, 4, 5, 6 },
    // Nov-Dec-Jan Exposed
    { 4, 5, 6, 3, 4, 5, 2, 3, 4, 2, 3, 4, 3, 4, 5, 4, 5, 6 },
    { 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6 },
    { 4, 5, 6, 3, 4, 4, 2, 3, 3, 2, 3, 3, 3, 4, 5, 4, 5, 6 },
    { 4, 5, 6, 2, 3, 4, 2, 2, 3, 3, 4, 4, 4, 5, 6, 4, 5, 6 },
    { 4, 5, 6, 3, 4, 5, 2, 3, 3, 2, 2, 3, 3, 4, 4, 4, 5, 6 },
    { 4, 5, 6, 2, 3, 3, 1, 1, 2, 1, 1, 2, 2, 3, 3, 4, 5, 6 },
    { 4, 5, 6, 3, 4, 5, 2, 3, 3, 2, 3, 3, 3, 4, 4, 4, 5, 6 },
    { 4, 5, 6, 4, 5, 6, 3, 4, 4, 2, 2, 3, 2, 3, 4, 4, 5, 6 },
    // Nov-Dec-Jan Shaded
    { 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6 },
    { 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6 },
    { 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6 },
    { 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6 }
};

//------------------------------------------------------------------------------
/*! \brief FdfmcDialog constructor.
 */

FdfmcDialog::FdfmcDialog( QWidget *parent, const char *name,
        const QString& program, const QString& version ) :
    AppDialog( parent,
        "FdfmcDialog:Caption",
        "EveningRise.png",
        "Evening Rise",
        "fdfmcDialog.html",
        name,
        "FdfmcDialog:Button:Dismiss",
        0,
        "FdfmcDialog:Button:Export" ),
    m_gridFrame(0),
    m_gridLayout(0),
    m_dbComboBox(0),
    m_rhComboBox(0),
    m_monComboBox(0),
    m_todComboBox(0),
    m_elevComboBox(0),
    m_slpComboBox(0),
    m_aspComboBox(0),
    m_shdComboBox(0),
    m_corLineEdit(0),
    m_refLineEdit(0),
    m_resLineEdit(0),
    m_descriptEdit(0),
    m_program( program ),
    m_version( version )
{
    // Initialization
    int row;
    for ( row = 0;
          row < Rows;
          row++ )
    {
        translate( FdfmcName[row], FdfmcNameKey[row] );
        m_lbl[row] = 0;
    }

    // Hidden frame to contain a grid layout
    m_gridFrame = new QFrame( m_page->m_contentFrame, "m_gridFrame" );
    Q_CHECK_PTR( m_gridFrame );
    m_gridFrame->setFrameStyle( QFrame::NoFrame );

    // Create the label-entry grid layout
    m_gridLayout = new QGridLayout( m_gridFrame, Rows, 2, 0, 2, "m_gridLayout" ) ;
    Q_CHECK_PTR( m_gridLayout );

    // Left column contains the row labels
    for ( row = 0;
          row < Rows;
          row++ )
    {
        m_lbl[row] = new QLabel( FdfmcName[row], m_gridFrame );
        Q_CHECK_PTR( m_lbl[row] );
        m_lbl[row]->setFixedSize( m_lbl[row]->sizeHint() );
        m_gridLayout->addWidget( m_lbl[row], row, 0, AlignLeft );
    }

    // Dry bulb temperature combo box
    m_dbComboBox = new QComboBox( false, m_gridFrame, "m_dbComboBox" );
    Q_CHECK_PTR( m_dbComboBox );
    m_dbComboBox->insertStrList( DbValue, DbValues );
    m_gridLayout->addWidget( m_dbComboBox, 0, 1 );
    connect( m_dbComboBox,  SIGNAL( activated(int) ),
             this,          SLOT( update(int) ) );
    m_dbComboBox->setCurrentItem( appProperty()->integer( "fdfmDb" ) );

    // Relative humidity combo box
    m_rhComboBox = new QComboBox( false, m_gridFrame, "m_rhComboBox" );
    Q_CHECK_PTR( m_rhComboBox );
    m_rhComboBox->insertStrList( RhValue, RhValues );
    m_gridLayout->addWidget( m_rhComboBox, 1, 1 );
    connect( m_rhComboBox,  SIGNAL( activated(int) ),
             this,          SLOT( update(int) ) );
    m_rhComboBox->setCurrentItem( appProperty()->integer( "fdfmRh" ) );

    // Reference fuel moisture result
    m_refLineEdit = new QLineEdit( m_gridFrame, "m_refLineEdit" );
    Q_CHECK_PTR( m_refLineEdit );
    m_refLineEdit->setReadOnly( true );
    m_gridLayout->addWidget( m_refLineEdit, 2, 1 );

    // Month combo box
    m_monComboBox = new QComboBox( false, m_gridFrame, "m_monComboBox" );
    Q_CHECK_PTR( m_monComboBox );
    m_monComboBox->insertStrList( MonValue, MonValues );
    m_gridLayout->addWidget( m_monComboBox, 4, 1 );
    connect( m_monComboBox, SIGNAL( activated(int) ),
             this,          SLOT( update(int) ) );
    m_monComboBox->setCurrentItem( appProperty()->integer( "fdfmMon" ) );

    // Time-of-day combo box
    m_todComboBox = new QComboBox( false, m_gridFrame, "m_todComboBox" );
    Q_CHECK_PTR( m_todComboBox );
    m_todComboBox->insertStrList( TodValue, TodValues );
    m_gridLayout->addWidget( m_todComboBox, 5, 1 );
    connect( m_todComboBox, SIGNAL( activated(int) ),
             this,          SLOT( update(int) ) );
    m_todComboBox->setCurrentItem( appProperty()->integer( "fdfmTod" ) );

    // Elevational difference combo box
    m_elevComboBox = new QComboBox( false, m_gridFrame, "m_elevComboBox" );
    Q_CHECK_PTR( m_elevComboBox );
    m_elevComboBox->insertStrList( ElevValue, ElevValues );
    m_gridLayout->addWidget( m_elevComboBox, 6, 1 );
    connect( m_elevComboBox,SIGNAL( activated(int) ),
             this,          SLOT( update(int) ) );
    m_elevComboBox->setCurrentItem( appProperty()->integer( "fdfmElev" ) );

    // Slope combo box
    m_slpComboBox = new QComboBox( false, m_gridFrame, "m_slpComboBox" );
    Q_CHECK_PTR( m_slpComboBox );
    m_slpComboBox->insertStrList( SlpValue, SlpValues );
    m_gridLayout->addWidget( m_slpComboBox, 7, 1 );
    connect( m_slpComboBox, SIGNAL( activated(int) ),
             this,          SLOT( update(int) ) );
    m_slpComboBox->setCurrentItem( appProperty()->integer( "fdfmSlp" ) );

    // Aspect combo box
    m_aspComboBox = new QComboBox( false, m_gridFrame, "m_aspComboBox" );
    Q_CHECK_PTR( m_aspComboBox );
    m_aspComboBox->insertStrList( AspValue, AspValues );
    m_gridLayout->addWidget( m_aspComboBox, 8, 1 );
    connect( m_aspComboBox, SIGNAL( activated(int) ),
             this,          SLOT( update(int) ) );
    m_aspComboBox->setCurrentItem( appProperty()->integer( "fdfmAsp" ) );

    // Fuel shading from sun combo box
    m_shdComboBox = new QComboBox( false, m_gridFrame, "m_shdComboBox" );
    Q_CHECK_PTR( m_shdComboBox );
    m_shdComboBox->insertStrList( ShdValue, ShdValues );
    m_gridLayout->addWidget( m_shdComboBox, 9, 1 );
    connect( m_shdComboBox, SIGNAL( activated(int) ),
             this,          SLOT( update(int) ) );
    m_shdComboBox->setCurrentItem( appProperty()->integer( "fdfmShd" ) );

    // Fuel moisture correction result
    m_corLineEdit = new QLineEdit( m_gridFrame, "m_corLineEdit" );
    Q_CHECK_PTR( m_corLineEdit );
    m_corLineEdit->setReadOnly( true );
    m_gridLayout->addWidget( m_corLineEdit, 10, 1 );

    // Corrected fuel moisture result
    m_resLineEdit = new QLineEdit( m_gridFrame, "m_resLineEdit" );
    Q_CHECK_PTR( m_resLineEdit );
    m_resLineEdit->setReadOnly( true );
    m_gridLayout->addWidget( m_resLineEdit, 12, 1 );

    // Description entry field
    m_descriptEdit = new QLineEdit( m_gridFrame, "m_descriptEdit" );
    Q_CHECK_PTR( m_descriptEdit );
    m_gridLayout->addWidget( m_descriptEdit, 14, 1 );
    connect( m_descriptEdit, SIGNAL( textChanged( const QString &) ),
             this,           SLOT( descriptionChanged( const QString &) ) );
    m_descriptEdit->setText( appProperty()->string( "fdfmDesc" ) );

    // Do not allow the middle pane to change size, so that if the
    // dialog is maximized the help pane gets all the new space.
    m_page->m_midFrame->setFixedWidth(
        m_page->m_midFrame->sizeHint().width() );

    // Set initial values
    update( 0 );
    return;
}

//------------------------------------------------------------------------------
/*! \brief FdfmcDialog destructor.
 *
 *  A precautionary destructor to convince memory leak detectors
 *  that resources have REALLY been released.  This is harmless, but
 *  redundant, since Qt destroys all children when a parent is destroyed.
 */

FdfmcDialog::~FdfmcDialog( void )
{
    int row;
    for ( row = 0;
          row < Rows;
          row++ )
    {
        delete m_lbl[row];    m_lbl[row] = 0;
    }
    delete m_dbComboBox;        m_dbComboBox = 0;
    delete m_rhComboBox;        m_rhComboBox = 0;
    delete m_monComboBox;       m_monComboBox = 0;
    delete m_todComboBox;       m_todComboBox = 0;
    delete m_elevComboBox;      m_elevComboBox = 0;
    delete m_slpComboBox;       m_slpComboBox = 0;
    delete m_aspComboBox;       m_aspComboBox = 0;
    delete m_shdComboBox;       m_shdComboBox = 0;
    delete m_descriptEdit;      m_descriptEdit = 0;
    delete m_corLineEdit;       m_corLineEdit = 0;
    delete m_refLineEdit;       m_refLineEdit = 0;
    delete m_resLineEdit;       m_resLineEdit = 0;
    delete m_gridLayout;        m_gridLayout = 0;
    delete m_gridFrame;         m_gridFrame = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Callback for the Export button.
 */

void FdfmcDialog::clear( void )
{
    // Setup
    QString typeName, fileName, text;
    translate( typeName, "BpDocument:SaveExportedResults:Caption" );
    //QString filters = ( "Spreadsheet ( *.xls);;Html ( *.htm *.html)" );
    QString filters = ( "Html ( *.htm *.html)" );

    // Request the output file
    bool more = true;
    while( more )
    {
        more = false;
        // Display the dialog
        QFileDialog fd( this, "saveResults", true );
        fd.setDir( appFileSystem()->exportPath() );
        fd.setMode( QFileDialog::AnyFile );
        fd.setFilters( filters );
        fd.setCaption( typeName );
        if ( fd.exec() != QDialog::Accepted )
        {
            return;
        }
        // Get the file name
        fileName = fd.selectedFile();
        if ( fileName.isNull() || fileName.isEmpty() )
        {
            return;
        }
        // Determine the source file and extension
        QString selectedFilter = fd.selectedFilter();
        if ( selectedFilter.left( 4 ) == "Html" )
        {
            if ( fileName.right( 4 ) != ".htm"
              && fileName.right( 5 ) != ".html" )
            {
                fileName.append( ".html" );
            }
        }
        //else // if ( selectedFilter.left( 5 ) == "Spread" )
        //{
        //    if ( fileName.right( 4 ) != ".xls" )
        //    {
        //        fileName.append( ".xls" );
        //    }
        //}
        // If the file exists, get permission to overwrite it.
        QFileInfo fi( fileName );
        if ( fi.exists() )
        {
            QString caption2;
            translate( caption2, "FileSelector:OverwriteFile:Caption" );
            translate( text, "FileSelector:OverwriteFile:Text",
                    "Html", fileName );
            if ( yesno( caption2, text ) == 0 )
            {
                more = true;
            }
        }
    }

    // Write the export file
    FILE *fptr;
    if ( ! ( fptr = fopen( fileName.latin1(), "w" ) ) )
    // This code block should never be executed!
    {
        translate( text, "FdfmcDialog:FileOpen:Error", fileName );
        warn( text );
        return;
    }

    // HTML
    time_t now = time( (time_t*) NULL );
    fprintf( fptr,
        "<html>\n"
        "  <head>\n"
        "  </head>\n"
        "  <body>\n"
        "    <h3>%s %s Fine Dead Fuel Moisture Content Tool</h3>\n"
        "    <hr>\n"
        "    <table>\n"
        "      <tr>\n"
        "        <th align='center' colspan='2' bgcolor='#8dcff4'>%s</th>\n"
        "      </tr>\n",
        m_program.latin1(),
        m_version.latin1(),
        m_descriptEdit->text().latin1()
    );
    const char* fmt1 = "      <tr><td align='left'>%s</td><td align='center'>%s</td></tr>\n";
    const char* fmt2 = "      <tr><td align='left'>%s</td><td align='center' bgcolor='#d4ecfb'>%s</td></tr>\n";
    const char* fmt3 = "      <tr><td align='left'><hr></td><td align='center'><hr></td></tr>\n";

    fprintf( fptr, fmt1, m_lbl[0]->text().latin1(), m_dbComboBox->currentText().latin1() );
    fprintf( fptr, fmt1, m_lbl[1]->text().latin1(), m_rhComboBox->currentText().latin1()  );
    fprintf( fptr, fmt2, m_lbl[2]->text().latin1(), m_refLineEdit->text().latin1() );
    fprintf( fptr, fmt3 );
    fprintf( fptr, fmt1, m_lbl[4]->text().latin1(), m_monComboBox->currentText().latin1()  );
    fprintf( fptr, fmt1, m_lbl[5]->text().latin1(), m_todComboBox->currentText().latin1()  );
    fprintf( fptr, fmt1, m_lbl[6]->text().latin1(), m_elevComboBox->currentText().latin1()  );
    fprintf( fptr, fmt1, m_lbl[7]->text().latin1(), m_slpComboBox->currentText().latin1()  );
    fprintf( fptr, fmt1, m_lbl[8]->text().latin1(), m_aspComboBox->currentText().latin1()  );
    fprintf( fptr, fmt1, m_lbl[9]->text().latin1(), m_shdComboBox->currentText().latin1()  );
    fprintf( fptr, fmt2, m_lbl[10]->text().latin1(), m_corLineEdit->text().latin1() );
    fprintf( fptr, fmt3 );
    fprintf( fptr, fmt2, m_lbl[12]->text().latin1(), m_resLineEdit->text().latin1() );
    fprintf( fptr,
        "    </table>\n"
        "    </br>\n"
        "    <hr>\n"
        "    Run on %s"
        "  </body>\n"
        "</html>\n",
        ctime( &now )
    );
    fclose( fptr );

    // Confirm the file was written
    translate( text, "BpDocument:SaveExportedHtml:Saved", fileName );
    info( text );
    return;
}

void FdfmcDialog::descriptionChanged( const QString &description )
{
    appProperty()->string( "fdfmDesc",  description.latin1() );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Callback for all combo boxes to update the reference fuel moisture,
 *  fuel moisture correction, and corrected fuel moisture.
 */

void FdfmcDialog::update( int )
{
    // Get current values
    m_asp  = m_aspComboBox->currentItem();
    m_db   = m_dbComboBox->currentItem();
    m_elev = m_elevComboBox->currentItem();
    m_mon  = m_monComboBox->currentItem();
    m_rh   = m_rhComboBox->currentItem();
    m_shd  = m_shdComboBox->currentItem();
    m_slp  = m_slpComboBox->currentItem();
    m_tod  = m_todComboBox->currentItem();

    // Determine reference fuel moisture
    m_ref = Reference[ m_db ][ m_rh ];

    // Determine fuel moisture correction
    // Correction table row
    int row = ( m_shd == 0 )
            ? ( m_slp + 2 * m_asp )
            : ( 8 + m_asp );
    row += 12 * m_mon;
    // Correction table column
    int col = m_elev + 3 * m_tod;
    m_cor = Correction[row][col];

    // Corrected fuel moisture
    m_res = m_ref + m_cor;

    // Display new values
    m_refLineEdit->setReadOnly( false );
    m_corLineEdit->setReadOnly( false );
    m_resLineEdit->setReadOnly( false );

    m_refLineEdit->setText( QString( "    %1 %" ).arg( m_ref ) );
    m_corLineEdit->setText( QString( "    %1 %" ).arg( m_cor ) );
    m_resLineEdit->setText( QString( "    %1 %" ).arg( m_res ) );

    m_refLineEdit->setReadOnly( true );
    m_corLineEdit->setReadOnly( true );
    m_resLineEdit->setReadOnly( true );

    // Store inputs for next time
    appProperty()->integer( "fdfmAsp",  m_aspComboBox->currentItem() );
    appProperty()->integer( "fdfmDb",   m_dbComboBox->currentItem() );
    appProperty()->integer( "fdfmElev", m_elevComboBox->currentItem() );
    appProperty()->integer( "fdfmMon",  m_monComboBox->currentItem() );
    appProperty()->integer( "fdfmRh",   m_rhComboBox->currentItem() );
    appProperty()->integer( "fdfmShd",  m_shdComboBox->currentItem() );
    appProperty()->integer( "fdfmSlp",  m_slpComboBox->currentItem() );
    appProperty()->integer( "fdfmTod",  m_todComboBox->currentItem() );
    return;
}

//------------------------------------------------------------------------------
//  End of fdfmcdialog.cpp
//------------------------------------------------------------------------------

