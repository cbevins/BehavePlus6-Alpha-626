//------------------------------------------------------------------------------
/*! \file appearancedialog.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus AppearanceDialog class methods.
 */

// Custom include files
#include "appearancedialog.h"
#include "appdialog.h"
#include "appproperty.h"
#include "bpdocument.h"
#include "propertydialog.h"
#include "property.h"
#include "varcheckbox.h"

// Qt include files
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>

/*! \var StdColors
 *  \brief Number of ColorValues[] and ColorKeys[] used as true color names.
 */

static const int StdColors = 15;

/*! \var LineColors
 *  \brief Adds the "Rainbow" choice to the combo boxes.
 */

static const int LineColors = 16;

/*! \var ColorKeys[]
 *  \brief Translation keys for the various color combo boxes.
 */

static const QString ColorKeys[LineColors] =
{
    "AppearanceDialog:Color:White",
    "AppearanceDialog:Color:Black",
    "AppearanceDialog:Color:Gray",
    "AppearanceDialog:Color:Red",
    "AppearanceDialog:Color:Green",
    "AppearanceDialog:Color:Blue",
    "AppearanceDialog:Color:Magenta",
    "AppearanceDialog:Color:Cyan",
    "AppearanceDialog:Color:Yellow",
    "AppearanceDialog:Color:DarkRed",
    "AppearanceDialog:Color:DarkGreen",
    "AppearanceDialog:Color:DarkBlue",
    "AppearanceDialog:Color:DarkMagenta",
    "AppearanceDialog:Color:DarkCyan",
    "AppearanceDialog:Color:DarkYellow",
    "AppearanceDialog:Color:RainbowColors"
};

/*! \var ColorValues[]
 *  \brief Associated property values for the various color combo boxes.
 */

static const QString ColorValues[LineColors] =
{
    "white",        "black",        "gray",
    "red",          "green",        "blue",
    "magenta",      "cyan",         "yellow",
    "darkRed",      "darkGreen",    "darkBlue",
    "darkMagenta",  "darkCyan",     "darkYellow", "rainbow"
};

/*! \var RowColorKeys[]
 *  \brief Translation keys for the Table row color combo boxes.
 */

static const int RowColors = 14;
static const QString RowColorKeys[RowColors] =
{
    "AppearanceDialog:BgColors:LightGrey",
    "AppearanceDialog:BgColors:LightPink",
    "AppearanceDialog:BgColors:LightGreen",
    "AppearanceDialog:BgColors:LightBlue",
    "AppearanceDialog:BgColors:LightCyan",
    "AppearanceDialog:BgColors:LightYellow",
    "AppearanceDialog:BgColors:LightGoldenrod",
    "AppearanceDialog:BgColors:LightCoral",
    "AppearanceDialog:BgColors:LightSalmon",
    "AppearanceDialog:BgColors:LightSlateGrey",
    "AppearanceDialog:BgColors:LightSlateBlue",
    "AppearanceDialog:BgColors:LightSkyBlue",
    "AppearanceDialog:BgColors:LightSteelBlue",
    "AppearanceDialog:BgColors:LightSeaGreen"
};

/*! \var RowColorValues[]
 *  \brief Associated property values for the Table row color combo box.
 */

static const QString RowColorValues[RowColors] =
{
    "lightGrey",      "lightPink",      "lightGreen",     "lightBlue",
    "lightCyan",      "lightYellow",    "lightGoldenrod", "lightCoral",
    "lightSalmon",    "lightSlateGrey", "lightSlateBlue", "lightSkyBlue",
    "lightSteelBlue", "lightSeaGreen"
};

/*! \var GraphOriginKeys[]
 *  \brief Translation keys for the Graph Title combo boxes.
 */

static const int GraphTitles = 2;
static const QString GraphTitleKeys[] =
{
    "AppearanceDialog:GraphTitle:Short",
    "AppearanceDialog:GraphTitle:Long"
};

/*! \var GraphTitleValues[]
 *  \brief Associated property values for the Graph Title combo box.
 *  The "Short" option sets graphTitleActive to FALSE.
 *  The "Long" option sets graphTitleActive to TRUE.
 */

static const QString GraphTitleValues[] =
{
    "false",
    "true"
};

/*! \var GraphOriginKeys[]
 *  \brief Translation keys for the Graph Axis Origins combo boxes.
 */

static const int GraphOrigins = 2;
static const QString GraphOriginKeys[GraphOrigins] =
{
    "AppearanceDialog:GraphOrigin:Zero",
    "AppearanceDialog:GraphOrigin:MinValue"
};

/*! \var GraphOriginValues[]
 *  \brief Associated proeprty values for the Graph Axis Origins combo boxes.
 *  The "Zero" option sets graphXOriginAtZero & graphYOriginAtZero to TRUE.
 *  The "Min Val" option sets graphXOriginAtZero & graphYOriginAtZero to FALSE.
 */

static const QString GraphOriginValues[GraphOrigins] =
{
    "true",
    "false"
};

//------------------------------------------------------------------------------
/*! \brief AppearanceDialog constructor.
 *
 *  \param bp   Pointer to the parent BpDocument widget.
 *  \param name Internal widget p_name.
 */

AppearanceDialog::AppearanceDialog( BpDocument *bp, const QString &captionKey,
    const char *p_name, const QString &acceptKey, const QString &rejectKey ) :
    PropertyTabDialog( bp, bp->property(), captionKey, p_name,
        acceptKey, rejectKey ),
    m_bp(bp),
    m_tabPosition(0),
    m_showBrowser(0),
    m_showPicture(0),
    m_borderColor(0),
    m_borderWidth(0)
{
    int r = 0;
    PropertyPage  *p;
    // Picture names
    QString Bmw( "Evening In The Bob Marshall Wilderness" );
    QString Twr( "Teller Wildlife Refuge" );

    // Add the "Application" page
    // NOTE: These are application-wide properties
    // that must be stored in the application's PropertyDict
    // as well as the BpDocument's PropertyDict
    // TO DO: add controls for appFixedFont{Color,Family,Size}.
    // TO DO: add controls for appPropFont{Color,Family,Size}.
    p = addPage( "AppearanceDialog:Application:Tab", 4, 2,
        "EveningInTheBob1.png", Bmw, "applicationAppearance.html" );

    m_showBrowser =
        p->addCheck( "appShowBrowser",
                     "AppearanceDialog:Appearance:ShowHelp", "",
                     0, 0, 0, 1 );
    m_showPicture =
        p->addCheck( "appShowPicture",
                     "AppearanceDialog:Appearance:ShowPicture", "",
                     1, 0, 1, 1 );
        p->addLabel( "AppearanceDialog:Appearance:BorderColor",
                     2, 0, 2, 0 );
    m_borderColor =
        p->addCombo( "pageBorderColor",
                     ColorKeys, ColorValues, StdColors,
                     2, 1, 2, 1 );
        p->addLabel( "AppearanceDialog:Appearance:BorderWidth",
                     3, 0, 3, 0 );
    m_borderWidth =
        p->addSpin(  "pageBorderWidth", 0, 9, 1,
                     3, 1, 3, 1 );

    // Add the "Graph Size" page
    // TO DO: add controls for graphSubtitleFont{Color,Family,Size}.
    // TO DO: add controls for graphTextFont{Color,Family,Size}.
    // TO DO: add controls for graphTitleFont{Color,Family,Size}.
    // TO DO: add controls for graphValueFont{Color,Family,Size}.
    p = addPage( "AppearanceDialog:GraphSize:Tab", 4, 2,
        "EveningInTheBob2.png", Bmw, "graphSize.html" );

        p->addLabel( "AppearanceDialog:GraphSize:GraphSize",
                     0, 0, 0, 0 );
        p->addSpin(  "graphScaleHeight", 25, 100, 5,
                     0, 1, 0, 1, 0, 100.0 );
        p->addLabel( "AppearanceDialog:GraphSize:GraphTitle",
                     1, 0, 1, 0 );
        p->addCombo( "graphTitleActive",
                     GraphTitleKeys, GraphTitleValues, GraphTitles,
                     1, 1, 1, 1 );
        p->addLabel( "AppearanceDialog:GraphSize:XAxisOrigin",
                     2, 0, 2, 0 );
        p->addCombo( "graphXOriginAtZero",
                     GraphOriginKeys, GraphOriginValues, GraphOrigins,
                     2, 1, 2, 1 );
        p->addLabel( "AppearanceDialog:GraphSize:YAxisOrigin",
                     3, 0, 3, 0 );
        p->addCombo( "graphYOriginAtZero",
                     GraphOriginKeys, GraphOriginValues, GraphOrigins,
                     3, 1, 3, 1 );

    // Add the "Graph Elements" page
    p = addPage( "AppearanceDialog:GraphElements:Tab", 10, 2,
        "EveningInTheBob3.png", Bmw, "graphElements.html" );

        p->addLabel( "AppearanceDialog:GraphElements:Background",
                     0, 0, 0, 0 );
        p->addCombo( "graphBackgroundColor",
                     ColorKeys, ColorValues, StdColors,
                     0, 1, 0, 1 );
        p->addLabel(  "AppearanceDialog:GraphElements:RainbowColors",
                     1, 0, 1, 0 );
        p->addSpin(  "graphRainbowColors", 3, 18, 1,
                     1, 1, 1, 1 );
        p->addLabel( "AppearanceDialog:GraphElements:BarColor",
                     2, 0, 2, 0 );
        QComboBox *cb =
        p->addCombo( "graphBarColor",
                     ColorKeys, ColorValues, LineColors,
                     2, 1, 2, 1 );
        // Make the entry fields the same size as this combo box entry
        int l_width = cb->sizeHint().width();
        p->addLabel( "AppearanceDialog:GraphElements:CurvePoints",
                     3, 0, 3, 0 );
        p->addSpin(  "graphLinePoints", 4, 100, 10,
                     3, 1, 3, 1);
        p->addLabel( "AppearanceDialog:GraphElements:CurveColor",
                     4, 0, 4, 0 );
        p->addCombo( "graphLineColor",
                     ColorKeys, ColorValues, LineColors,
                     4, 1, 4, 1 );
        p->addLabel( "AppearanceDialog:GraphElements:CurveWidth",
                     5, 0, 5, 0 );
        p->addSpin(  "graphLineWidth", 0, 9, 1,
                     5, 1, 5, 1 );
        p->addLabel( "AppearanceDialog:GraphElements:AxleColor",
                     6, 0, 6, 0 );
        p->addCombo( "graphAxleColor",
                     ColorKeys, ColorValues, StdColors,
                     6, 1, 6, 1 );
        p->addLabel( "AppearanceDialog:GraphElements:AxleWidth",
                     7, 0, 7, 0 );
        p->addSpin(  "graphAxleWidth", 1, 9, 1,
                     7, 1, 7, 1 );
        p->addLabel( "AppearanceDialog:GraphElements:GridColor",
                     8, 0, 8, 0 );
        p->addCombo( "graphGridColor",
                     ColorKeys, ColorValues, StdColors,
                     8, 1, 8, 1 );
        p->addLabel( "AppearanceDialog:GraphElements:GridWidth",
                     9, 0, 9, 0 );
        p->addSpin(  "graphGridWidth", 0, 9, 1,
                     9, 1, 9, 1 );

    // Add the "Page Tabs" page
    p = addPage( "AppearanceDialog:PageTabs:Tab", 6, 2,
        "TellerWildlifeRefuge1.png", Twr, "pageTabs.html" );

        p->addCheck( "tabActive",
                     "AppearanceDialog:PageTabs:TabActive", "",
                     0, 0, 0, 1 );
        p->addLabel( "AppearanceDialog:PageTabs:TabsPerPage",
                     1, 0, 1, 0 );
        QSpinBox *tabsPerPage = p->addSpin( "tabTabsPerPage", 0, 10, 1,
                     1, 1, 1, 1 );
        connect( tabsPerPage, SIGNAL( valueChanged(int) ),
                this,        SLOT( tabsValueChanged(int) ) );
        p->addLabel( "AppearanceDialog:PageTabs:TabsPosition",
                     2, 0, 2, 0 );
        m_tabPosition = p->addSpin(  "tabPosition",
                     0, m_bp->property()->integer( "tabTabsPerPage" ), 1,
                     2, 1, 2, 1 );
        p->addLabel( "AppearanceDialog:PageTabs:Text",
                     3, 0, 3, 0 );
    QLineEdit *le =
        p->addEntry( "tabText",
                     3, 1, 3, 1 );
        le->setFixedWidth( l_width );
        p->addLabel( "AppearanceDialog:PageTabs:FontColor",
                     4, 0, 4, 0 );
        p->addCombo( "tabFontColor",
                     ColorKeys, ColorValues, StdColors,
                     4, 1, 4, 1 );
        p->addLabel( "AppearanceDialog:PageTabs:FontSize",
                     5, 0, 5, 0 );
        p->addSpin(  "tabFontSize", 8, 24, 1,
                     5, 1, 5, 1 );

    // Add the "Tables" page
    // TO DO: add controls for tableSubtitleFont{Color,Family,Size}.
    // TO DO: add controls for tableTextFont{Color,Family,Size}.
    // TO DO: add controls for tableTitleFont{Color,Family,Size}.
    // TO DO: add controls for tableValueFont{Color,Family,Size}.
    p = addPage( "AppearanceDialog:Tables:Tab", 2, 2,
        "TellerWildlifeRefuge2.png", Twr, "tablesAppearance.html" );

        p->addCheck( "tableRowBackgroundColorActive",
                    "AppearanceDialog:Tables:ShadeRowsActive", "",
                    0, 0, 0, 1 );
        p->addLabel( "AppearanceDialog:Table:ShadeRowsColor",
                     1, 0, 1, 0 );
        p->addCombo( "tableRowBackgroundColor",
                     RowColorKeys, RowColorValues, RowColors,
                     1, 1, 1, 1 );
        p->addCheck( "tableShadingBlank",
                    "AppearanceDialog:Tables:RxVariablesBlank", "",
                    2, 0, 2, 1 );

    // Add the "Worksheet" page
    // TO DO: add control for worksheetmaskColor property.
    // TO DO: add controls for worksheetEntryFont{Color,Family,Size}.
    // TO DO: add controls for worksheetTextFont{Color,Family,Size}.
    // TO DO: add controls for worksheetTitleFont{Color,Family,Size}.
    // TO DO: add controls for worksheetValueFont{Color,Family,Size}.
    r = 0;
    p = addPage( "AppearanceDialog:Worksheet:Tab", 11, 2,
        "TellerWildlifeRefuge3.png", Twr, "worksheetAppearance.html" );

        p->addCheck( "docRxActive",
                     "AppearanceDialog:Worksheet:DocRxActive", "",
                      r, 0, r, 1 ); r++;
        p->addCheck( "docFireActive",
                     "AppearanceDialog:Worksheet:DocFireActive", "",
                      r, 0, r, 1 ); r++;
        p->addCheck( "docTrainingActive",
                     "AppearanceDialog:Worksheet:DocTrainingActive", "",
                      r, 0, r, 1 ); r++;
        p->addLabel( "!",
                      r, 0, r, 0 ); r++;
        p->addCheck( "worksheetShowChoices",
                     "AppearanceDialog:Worksheet:ShowChoices", "",
                      r, 0, r, 1 ); r++;
        p->addCheck( "worksheetShowUsedChoices",
                     "AppearanceDialog:Worksheet:ShowUsedChoices", "",
                      r, 0, r, 1 ); r++;
        p->addLabel( "!",
                      r, 0, r, 0 ); r++;
        p->addCheck( "worksheetShowOutputVars",
                     "AppearanceDialog:Worksheet:ShowOutputVars", "",
                      r, 0, r, 1 ); r++;
        p->addLabel( "!",
                      r, 0, r, 0 ); r++;
        p->addCheck( "worksheetNotesActive",
                     "AppearanceDialog:Worksheet:ShowNotes", "",
                      r, 0, r, 1 ); r++;
        p->addLabel( "AppearanceDialog:Worksheet:NotesLines",
                      r, 0, r, 0 ); r++;
        p->addSpin(  "worksheetNotesLines", 1, 10, 1,
                      r, 1, r, 1 ); r++;

    // Show then delete the dialog.
    resize( sizeHint() );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Re-implemented method to store current button states into their
 *  associated property values.  This is the OptionDialog's Accept button
 *  callback.
 */

void AppearanceDialog::store( void )
{
    // Store the application wide properties
    appProperty()->boolean( "appShowBrowser", m_showBrowser->isChecked() );
    appProperty()->boolean( "appShowPicture", m_showPicture->isChecked() );
    appProperty()->color( "pageBorderColor",
        ColorValues[ m_borderColor->currentItem() ] );
    appProperty()->integer( "pageBorderWidth", m_borderWidth->value() );

    // Store the BpDocument-specific properties
    PropertyTabDialog::store();
    return;
}

//------------------------------------------------------------------------------
/*! \brief m_pageTabsPerPageSpinBox callback slot to ensure the user doesn't
 *  select a tab position beyond the number of available tabs.
*/

void AppearanceDialog::tabsValueChanged( int value )
{
    m_tabPosition->setRange( 0, value );
    if ( m_tabPosition->value() > value )
    {
        m_tabPosition->setValue( value );
    }
    return;
}

//------------------------------------------------------------------------------
//  End of appearancedialog.cpp
//------------------------------------------------------------------------------

