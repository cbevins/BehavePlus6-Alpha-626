//------------------------------------------------------------------------------
/*! \file slopetooldialog.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2010 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus relative humidity dialog class methods.
 */

// Custom include files
#include "appfilesystem.h"
#include "appmessage.h"
#include "appsiunits.h"
#include "apptranslator.h"
#include "appwindow.h"
#include "slopetooldialog.h"

// Qt include files
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvalidator.h>
#include <qfiledialog.h>

#include <math.h>
#include <time.h>

static const double SlopeToolMrfs[] =
{
    1980., 3960., 7920., 10000., 15840., 21120., 24000., 31680.,
    50000., 62500., 63360., 100000., 126720., 250000.,
	253440., 506880., 1000000., 1013760.
};

static const char *SlopeToolUnits[] =
{
    "ft and in",
    "m and cm"
};

//------------------------------------------------------------------------------
/*! \brief SlopeToolDialog constructor.
 *
 *  \param parent       Pointer to the parent QWidget.
 *  \param captionKey   Dialog caption translator key.
 *  \param name         Widget internal name.
 *  \param acceptKey    Translator key for the accept button
 *  \param rejectKey    Translator key for the reject button
 */

SlopeToolDialog::SlopeToolDialog( QWidget *parent, const char *name,
        const QString& program, const QString& version ) :
    AppDialog( parent,
        "SlopeToolDialog:Caption",
        "FlatheadLake1.png",
        "FlatheadLake",
        "slopeMapMeasurements.html",
        name,
        "SlopeToolDialog:Button:Dismiss",
        0,
        "SlopeToolDialog:Button:Export" ),
	m_program( program ),
	m_version( version ),
	m_parent( parent ),
    m_gridFrame(0),
    m_gridLayout(0),
	m_listView( 0 ),
	m_mrfComboLabel( 0 ),
	m_mrfLabel( 0 ),
	m_mrfEntry( 0 ),
	m_unitsComboLabel( 0 ),
	m_unitsComboBox( 0 ),
	m_contourIntervalLabel( 0 ),
	m_contourIntervalEntry( 0 ),
	m_contourIntervalUnits( 0 ),
	m_mapDistanceLabel( 0 ),
	m_mapDistanceEntry( 0 ),
	m_mapDistanceUnits( 0 ),
	m_numberContoursLabel( 0 ),
	m_numberContoursEntry( 0 ),
	m_slopePercentLabel( 0 ),
	m_slopePercentEntry( 0 ),
	m_slopePercentUnits( 0 ),
	m_slopeDegreesLabel( 0 ),
	m_slopeDegreesEntry( 0 ),
	m_slopeDegreesUnits( 0 ),
	m_slopeElevationLabel( 0 ),
	m_slopeElevationEntry( 0 ),
	m_slopeElevationUnits( 0 ),
	m_slopeHorizontalLabel( 0 ),
	m_slopeHorizontalEntry( 0 ),
	m_slopeHorizontalUnits( 0 ),
	m_descriptionLabel( 0 ),
	m_descriptionEntry( 0 ),
	m_calculateButton( 0 ),
	m_mrf()
{
	int row = 0;
    // Hidden frame to contain a grid layout.
    m_gridFrame = new QFrame( m_page->m_contentFrame, "m_gridFrame" );
    Q_CHECK_PTR( m_gridFrame );
    m_gridFrame->setFrameStyle( QFrame::NoFrame );

    // Create grid layout with 18 rows, 3 columns, 0 border, and 2 spacing.
    m_gridLayout = new QGridLayout( m_gridFrame, 13, 3, 0, 2,
        QString( "%1:Grid" ).arg( name ) );
    Q_CHECK_PTR( m_gridLayout );

	// Map representative fraction combo box label
	m_mrfComboLabel = new QLabel( "Map Representative Fraction Selector", m_gridFrame );
    Q_CHECK_PTR( m_mrfComboLabel );
    m_gridLayout->addMultiCellWidget( m_mrfComboLabel, row, row, 0, 2,AlignCenter );
	row++;

	//--------------------------------------------------------------------------
	// Map representative fraction list view
	//--------------------------------------------------------------------------

    // Create the scrolling listview to display choices
    m_listView = new QListView( m_gridFrame, "m_listView" );
    Q_CHECK_PTR( m_listView );

    // Default columns
    m_listView->setColumnWidthMode( 0, QListView::Maximum );
    m_listView->setMultiSelection( false );
    m_listView->setRootIsDecorated( false );
    m_listView->setAllColumnsShowFocus( true );
    m_listView->setItemMargin( 2 );

    QString text("");
    m_listView->addColumn( "1:x" );
    m_listView->setColumnWidthMode( 0, QListView::Maximum );
    translate( text, "Wizard:MapFraction:In/Mi" );
    m_listView->addColumn( text );
    m_listView->setColumnWidthMode( 1, QListView::Maximum );
    translate( text, "Wizard:MapFraction:Mi/In" );
    m_listView->addColumn( text );
    m_listView->setColumnWidthMode( 2, QListView::Maximum );
    translate( text, "Wizard:MapFraction:Cm/Km" );
    m_listView->addColumn( text );
    m_listView->setColumnWidthMode( 3, QListView::Maximum );
    translate( text, "Wizard:MapFraction:Km/Cm" );
    m_listView->addColumn( text );
    m_listView->setColumnWidthMode( 4, QListView::Maximum );
    // Sort on the mi/in
    m_listView->setSorting( 2, true );

    // Add the list items
    unsigned int items = sizeof(SlopeToolMrfs) / sizeof(double);
    QListViewItem *item;
    QString str0, str1, str2, str3, str4;
    for ( unsigned int id=0; id < items; id++ )
    {
        str0.sprintf( "%8.0f", SlopeToolMrfs[id] );             // representative fraction
        str1.sprintf( "%7.4f", 63360. / SlopeToolMrfs[id] );    // in/mi
        str2.sprintf( "%8.5f", SlopeToolMrfs[id] / 63360. );    // mi/in
        str3.sprintf( "%7.4f", 100000. / SlopeToolMrfs[id] );   // cm/km
        str4.sprintf( "%7.4f", SlopeToolMrfs[id] / 100000. );   // km/cm
        item = new QListViewItem( m_listView, str0, str1, str2, str3, str4 );
        Q_CHECK_PTR( item );
    }
    // Override the default listview min and max widths
    m_listView->setMinimumWidth( m_listView->sizeHint().width() );

    // Set the initial size for this widget so the entire picture,
    // a good chunk of both the listView and textBrowser are visible.
    int width = widthHint() + m_listView->sizeHint().width();
	int height = 120;
    resize( width, height );
	m_listView->setMinimumHeight( height );
	m_listView->setMaximumHeight( height );
    m_gridLayout->addMultiCellWidget( m_listView, row, row, 0, 2 );
	m_gridLayout->setRowStretch( row, 10 );
	row++;
    // Allow a double click to select a single item
    connect( m_listView, SIGNAL( doubleClicked( QListViewItem * ) ),
             this,       SLOT( mrfDoubleClicked( QListViewItem * ) ) );

	//--------------------------------------------------------------------------
	// Map representative fraction entry
	//--------------------------------------------------------------------------

	// Map representative fraction label
	m_mrfLabel = new QLabel( "Map Rep Fraction", m_gridFrame );
    Q_CHECK_PTR( m_mrfLabel );
    m_gridLayout->addWidget( m_mrfLabel, row, 0, AlignLeft );

	// Map representative fraction entry
	m_mrfEntry = new QLineEdit( "", m_gridFrame, "m_mrfEntry" );
    Q_CHECK_PTR( m_mrfEntry );
	m_mrfEntry->setValidator( new QIntValidator( 1, 2000000, m_mrfEntry ) );
    m_gridLayout->addWidget( m_mrfEntry, row, 1, AlignLeft );
	connect( m_mrfEntry, SIGNAL(textChanged(const QString &)), this, SLOT(clearOutputs() ) );
	row++;

	//--------------------------------------------------------------------------
	// Units
	//--------------------------------------------------------------------------

	// Units combo box label
	m_unitsComboLabel = new QLabel( "Units", m_gridFrame );
    Q_CHECK_PTR( m_unitsComboLabel );
    m_gridLayout->addWidget( m_unitsComboLabel, row, 0, AlignLeft );

	// Units combo box
    m_unitsComboBox = new QComboBox( false, m_gridFrame, "m_unitsComboBox" );
    Q_CHECK_PTR( m_unitsComboBox );
    m_unitsComboBox->insertStrList( SlopeToolUnits, sizeof(SlopeToolUnits)/sizeof(const char *) );
    m_gridLayout->addWidget( m_unitsComboBox, row, 1, AlignLeft );
    m_unitsComboBox->setCurrentItem( 0 );
	row++;
    connect( m_unitsComboBox, SIGNAL(activated(int)), this, SLOT(unitsActivated(int)) );

	//--------------------------------------------------------------------------
	// Contour interval
	//--------------------------------------------------------------------------

	// Contour interval label
	m_contourIntervalLabel = new QLabel( "Contour Interval", m_gridFrame );
    Q_CHECK_PTR( m_contourIntervalLabel );
    m_gridLayout->addWidget( m_contourIntervalLabel, row, 0, AlignLeft );

	// Contour interval entry
	m_contourIntervalEntry = new QLineEdit( "", m_gridFrame, "m_contourIntervalEntry" );
    Q_CHECK_PTR( m_contourIntervalEntry );
	m_contourIntervalEntry->setValidator( new QDoubleValidator( 0., 99999., 2, m_contourIntervalEntry ) );
    m_gridLayout->addWidget( m_contourIntervalEntry, row, 1, AlignLeft );
	connect( m_contourIntervalEntry, SIGNAL(textChanged(const QString &)), this, SLOT(clearOutputs()) );

	// Contour interval units
	m_contourIntervalUnits = new QLabel( "ft", m_gridFrame );
    Q_CHECK_PTR( m_contourIntervalUnits );
    m_gridLayout->addWidget( m_contourIntervalUnits, row, 2, AlignLeft );
	row++;

	//--------------------------------------------------------------------------
	// Map distance
	//--------------------------------------------------------------------------

	// Map Distance label
	m_mapDistanceLabel = new QLabel( "Map Distance", m_gridFrame );
    Q_CHECK_PTR( m_mapDistanceLabel );
    m_gridLayout->addWidget( m_mapDistanceLabel, row, 0, AlignLeft );

	// Map distance entry
	m_mapDistanceEntry = new QLineEdit( "", m_gridFrame, "m_mapDistanceEntry" );
    Q_CHECK_PTR( m_mapDistanceEntry );
	m_mapDistanceEntry->setValidator( new QDoubleValidator( 0., 99999., 2, m_mapDistanceEntry ) );
    m_gridLayout->addWidget( m_mapDistanceEntry, row, 1, AlignLeft );
	connect( m_mapDistanceEntry, SIGNAL(textChanged(const QString &)), this, SLOT(clearOutputs()) );

	// Map distance units
	m_mapDistanceUnits = new QLabel( "in", m_gridFrame );
    Q_CHECK_PTR( m_mapDistanceUnits );
    m_gridLayout->addWidget( m_mapDistanceUnits, row, 2, AlignLeft );
	row++;

	//--------------------------------------------------------------------------
	// Number of contours
	//--------------------------------------------------------------------------

	// Number of contours label
	m_numberContoursLabel = new QLabel( "Number of Contours", m_gridFrame );
    Q_CHECK_PTR( m_numberContoursLabel );
    m_gridLayout->addWidget( m_numberContoursLabel, row, 0, AlignLeft );

	// Number of contours
	m_numberContoursEntry = new QLineEdit( "", m_gridFrame, "m_numberContoursEntry" );
    Q_CHECK_PTR( m_numberContoursEntry );
	m_numberContoursEntry->setValidator( new QDoubleValidator( 0., 99999., 2, m_numberContoursEntry ) );
    m_gridLayout->addWidget( m_numberContoursEntry, row, 1, AlignLeft );
	connect( m_numberContoursEntry, SIGNAL(textChanged(const QString &)), this, SLOT(clearOutputs()) );
	row++;

	// Caclulate Button
	m_calculateButton = new QPushButton( "Calculate", m_gridFrame, "m_calculateButton" );
	m_gridLayout->addMultiCellWidget( m_calculateButton, row, row, 0, 2 );
	row++;
    connect( m_calculateButton, SIGNAL(clicked()), this, SLOT(calculateButtonClicked()) );

	//--------------------------------------------------------------------------
	// Slope percent output
	//--------------------------------------------------------------------------

	// Slope percent label
	m_slopePercentLabel = new QLabel( "Slope Steepness", m_gridFrame );
    Q_CHECK_PTR( m_slopePercentLabel );
    m_gridLayout->addWidget( m_slopePercentLabel, row, 0, AlignLeft );

	// Slope percent entry
	m_slopePercentEntry = new QLineEdit( "", m_gridFrame, "m_slopePercentEntry" );
    Q_CHECK_PTR( m_slopePercentEntry );
	m_slopePercentEntry->setReadOnly( true );
    m_gridLayout->addWidget( m_slopePercentEntry, row, 1, AlignLeft );

	// Slope percent units
	m_slopePercentUnits = new QLabel( "%", m_gridFrame );
    Q_CHECK_PTR( m_slopePercentUnits );
    m_gridLayout->addWidget( m_slopePercentUnits, row, 2, AlignLeft );
	row++;

	//--------------------------------------------------------------------------
	// Slope degrees output
	//--------------------------------------------------------------------------

	// Slope degrees label
	m_slopeDegreesLabel = new QLabel( "Slope Steepness", m_gridFrame );
    Q_CHECK_PTR( m_slopeDegreesLabel );
    m_gridLayout->addWidget( m_slopeDegreesLabel, row, 0, AlignLeft );

	// Slope degrees entry
	m_slopeDegreesEntry = new QLineEdit( "", m_gridFrame, "m_slopeDegreesEntry" );
    Q_CHECK_PTR( m_slopeDegreesEntry );
    m_gridLayout->addWidget( m_slopeDegreesEntry, row, 1, AlignLeft );

	// Slope degrees units
	m_slopeDegreesUnits = new QLabel( "degrees", m_gridFrame );
    Q_CHECK_PTR( m_slopeDegreesUnits );
	m_slopeDegreesEntry->setReadOnly( true );
	m_gridLayout->addWidget( m_slopeDegreesUnits, row, 2, AlignLeft );
	row++;

	//--------------------------------------------------------------------------
	// Slope elevation change output
	//--------------------------------------------------------------------------

	// Slope elevation change label
	m_slopeElevationLabel = new QLabel( "Slope Elevation Change", m_gridFrame );
    Q_CHECK_PTR( m_slopeElevationLabel );
    m_gridLayout->addWidget( m_slopeElevationLabel, row, 0, AlignLeft );

	// Slope elevation change entry
	m_slopeElevationEntry = new QLineEdit( "", m_gridFrame, "m_slopeElevationEntry" );
    Q_CHECK_PTR( m_slopeElevationEntry );
	m_slopeElevationEntry->setReadOnly( true );
    m_gridLayout->addWidget( m_slopeElevationEntry, row, 1, AlignLeft );

	// Slope elevation change units
	m_slopeElevationUnits = new QLabel( "ft", m_gridFrame );
    Q_CHECK_PTR( m_slopeElevationUnits );
    m_gridLayout->addWidget( m_slopeElevationUnits, row, 2, AlignLeft );
	row++;

	//--------------------------------------------------------------------------
	// Slope horizontal distance output
	//--------------------------------------------------------------------------

	// Slope horizontal distance label
	m_slopeHorizontalLabel = new QLabel( "Slope Horizontal Distance", m_gridFrame );
    Q_CHECK_PTR( m_slopeHorizontalLabel );
    m_gridLayout->addWidget( m_slopeHorizontalLabel, row, 0, AlignLeft );

	// Slope horizontal distance entry
	m_slopeHorizontalEntry = new QLineEdit( "", m_gridFrame, "m_slopeHorizontalEntry" );
    Q_CHECK_PTR( m_slopeHorizontalEntry );
	m_slopeHorizontalEntry->setReadOnly( true );
    m_gridLayout->addWidget( m_slopeHorizontalEntry, row, 1, AlignLeft );

	// Slope horizontal distance units
	m_slopeHorizontalUnits = new QLabel( "ft", m_gridFrame );
    Q_CHECK_PTR( m_slopeHorizontalUnits );
    m_gridLayout->addWidget( m_slopeHorizontalUnits, row, 2, AlignLeft );
	row++;

	//--------------------------------------------------------------------------
	// Export description
	//--------------------------------------------------------------------------

	// Description label
	m_descriptionLabel = new QLabel( "Description", m_gridFrame );
    Q_CHECK_PTR( m_descriptionLabel );
    m_gridLayout->addWidget( m_descriptionLabel, row, 0, AlignLeft );

	// Description entry
	m_descriptionEntry = new QLineEdit( "", m_gridFrame, "m_descriptionEntry" );
    Q_CHECK_PTR( m_descriptionEntry );
    m_gridLayout->addMultiCellWidget( m_descriptionEntry, row, row, 1, 2 );
    return;
}

//------------------------------------------------------------------------------
/*! \brief SlopeToolDialog destructor.
 */

SlopeToolDialog::~SlopeToolDialog( void )
{
	delete m_listView;
	delete m_mrfComboLabel;
	delete m_mrfLabel;
	delete m_mrfEntry;
	delete m_unitsComboLabel;
	delete m_unitsComboBox;
	delete m_contourIntervalLabel;
	delete m_contourIntervalEntry;
	delete m_contourIntervalUnits;
	delete m_mapDistanceLabel;
	delete m_mapDistanceEntry;
	delete m_mapDistanceUnits;
	delete m_numberContoursLabel;
	delete m_numberContoursEntry;
	delete m_slopePercentLabel;
	delete m_slopePercentEntry;
	delete m_slopePercentUnits;
	delete m_slopeDegreesLabel;
	delete m_slopeDegreesEntry;
	delete m_slopeDegreesUnits;
	delete m_slopeElevationLabel;
	delete m_slopeElevationEntry;
	delete m_slopeElevationUnits;
	delete m_slopeHorizontalLabel;
	delete m_slopeHorizontalEntry;
	delete m_slopeHorizontalUnits;
	delete m_descriptionLabel;
	delete m_descriptionEntry;
	delete m_calculateButton;
	delete m_gridLayout;
	delete m_gridFrame;
}

//------------------------------------------------------------------------------
/*! \brief Slot callback for m_calculateButton clicked().
 */

void SlopeToolDialog::calculateButtonClicked( void )
{
	update();
}

//------------------------------------------------------------------------------
/*! \brief Reimplemented virtual callback for the Export button.
 */

void SlopeToolDialog::clear( void )
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
        translate( text, "SlopeToolDialog:FileOpen:Error", fileName );
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
        "    <h3>%s %s Slope Tool</h3>\n"
        "    <hr>\n"
        "    <table>\n"
        "      <tr>\n"
        "        <th align='center' colspan='3' bgcolor='#8dcff4'>%s</th>\n"
        "      </tr>\n",
        m_program.latin1(),
        m_version.latin1(),
        m_descriptionEntry->text().latin1()
    );
    const char* input   = "      <tr><td align='left'>%s</td><td align='center'>%s</td><td>%s</td></tr>\n";
    const char* output  = "      <tr><td align='left'>%s</td><td align='center' bgcolor='#d4ecfb'>%s</td><td>%s</td></tr>\n";
    const char* divider = "      <tr><td align='left'><hr></td><td align='center'><hr></td><td><hr></td></tr>\n";

    fprintf( fptr, input,
		m_mrfLabel->text().latin1(),
		m_mrfEntry->text().latin1(),
		"1:x" );
	fprintf( fptr, input,
		m_contourIntervalLabel->text().latin1(),
		m_contourIntervalEntry->text().latin1(),
		m_contourIntervalUnits->text().latin1() );
	fprintf( fptr, input,
		m_mapDistanceLabel->text().latin1(),
		m_mapDistanceEntry->text().latin1(),
		m_mapDistanceUnits->text().latin1() );
	fprintf( fptr, input,
		m_numberContoursLabel->text().latin1(),
		m_numberContoursEntry->text().latin1(),
		"" );
	fprintf( fptr, divider );
	fprintf( fptr, output,
		m_slopePercentLabel->text().latin1(),
		m_slopePercentEntry->text().latin1(),
		"%" );
	fprintf( fptr, output,
		m_slopeDegreesLabel->text().latin1(),
		m_slopeDegreesEntry->text().latin1(),
		"degrees" );
	fprintf( fptr, output,
		m_slopeElevationLabel->text().latin1(),
		m_slopeElevationEntry->text().latin1(),
		m_slopeElevationUnits->text().latin1() );
	fprintf( fptr, output,
		m_slopeHorizontalLabel->text().latin1(),
		m_slopeHorizontalEntry->text().latin1(),
		m_slopeHorizontalUnits->text().latin1() );
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

//------------------------------------------------------------------------------
/*! \brief Slot callback when any input entry field is changed.
 */

void SlopeToolDialog::clearOutputs( void )
{
	m_slopeDegreesEntry->setText( "" );
	m_slopePercentEntry->setText( "" );
	m_slopeHorizontalEntry->setText( "" );
	m_slopeElevationEntry->setText( "" );
}

//------------------------------------------------------------------------------
/*! \brief Slot callback for m_itemView doubleClicked().
 */

void SlopeToolDialog::mrfDoubleClicked( QListViewItem *item )
{
	m_mrfEntry->setText( item->text( 0 ).stripWhiteSpace() );
}

//------------------------------------------------------------------------------
/*! \brief Slot callback for m_unitsComboBox activated().
 */

void SlopeToolDialog::unitsActivated( int id )
{
	static const char *big[] = { "ft", "m" };
	static const char *little[] = { "in", "cm" };
	m_contourIntervalUnits->setText( big[id] );
	m_mapDistanceUnits->setText( little[id] );
	m_slopeElevationUnits->setText( big[id] );
	m_slopeHorizontalUnits->setText( big[id] );
	// Clear inputs and outputs
	m_mapDistanceEntry->setText( "" );
	m_numberContoursEntry->setText( "" );
	m_contourIntervalEntry->setText( "" );
	clearOutputs();
}

//------------------------------------------------------------------------------
/*! \brief Validates method 1 entry fields and fills in the result.
 */

void SlopeToolDialog::update( void )
{
	static const double ft[] = { 1., 3.28084 };
	static const double in[] = { 1., 0.393701 };
   // Access current input values
	int units       = m_unitsComboBox->currentItem();
    double interval = m_contourIntervalEntry->text().toDouble() * ft[units];
    double count    = m_numberContoursEntry->text().toDouble();
    double fraction = m_mrfEntry->text().toDouble();
	double distance = m_mapDistanceEntry->text().toDouble() * in[units];
    
	// Calculate results
    double rise     = interval * count / ft[units];
    double reach    = fraction * distance / 12. / ft[units];
	double percent  = ( reach < 0.01 ) ? 0. : rise / reach;
	double degrees  = ( reach < 0.01 ) ? 0. : atan( percent ) * 180. / 3.141592654;

    // Store results
	QString str;
	m_slopeDegreesEntry->setText( str.setNum( degrees, 'f', 0 ) );
	m_slopePercentEntry->setText( str.setNum( 100. * percent, 'f', 0 ) );
	m_slopeHorizontalEntry->setText( str.setNum( reach, 'f', 0 ) );
	m_slopeElevationEntry->setText( str.setNum( rise, 'f', 0 ) );
}

//------------------------------------------------------------------------------
//  End of slopetooldialog.cpp
//------------------------------------------------------------------------------

