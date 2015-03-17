//------------------------------------------------------------------------------
/*! \file sundialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus Sun/Moon calendar dialog class.
 */

#ifndef _SUNDIALOG_H_
/*! \def _SUNDIALOG_H_
 *  \brief Prevent redundant includes.
 */
#define _SUNDIALOG_H_ 1

// Forward class references
#include "appdialog.h"
class DateTime;
class GlobalPosition;
class PropertyDict;
class QCheckBox;
class QComboBox;
class QGridLayout;
class QLineEdit;
class QListView;
class QListViewItem;
class QPopupMenu;
class QSpinBox;

#include <stdio.h>

//------------------------------------------------------------------------------
/*! \class SunDialog sundialog.h
 *
 *  \brief BehavePlus Sun/Moon calendar dialog class.
 */

class SunDialog : public AppTabDialog
{
// Enable signals and slots
    Q_OBJECT
public:
    SunDialog( QWidget *parent, DateTime *dt, GlobalPosition *gp,
        PropertyDict *pd, const char *name="sunDialog" ) ;
    ~SunDialog( void );

protected:
    void page1( void );
    void page2( void );

protected slots:
    void browse( void ) ;
    void store( void ) ;

// Data members
private:
    AppPage         *m_page1;
    QFrame          *m_gridFrame1;
    QGridLayout     *m_gridLayout1;
    QLabel          *m_locationLabel;
    QLineEdit       *m_locationLineEdit;
    QPushButton     *m_browseButton;
    QLabel          *m_zoneLabel;
    QSpinBox        *m_zoneSpinBox;
    QLabel          *m_zoneDescLabel;
    QLabel          *m_spacer1Label;
    QLabel          *m_degreesLabel;
    QLabel          *m_minutesLabel;
    QLabel          *m_secondsLabel;
    QLabel          *m_lonLabel;
    QComboBox       *m_lonEWComboBox;
    QSpinBox        *m_lonDegSpinBox;
    QSpinBox        *m_lonMinSpinBox;
    QSpinBox        *m_lonSecSpinBox;
    QLabel          *m_latLabel;
    QComboBox       *m_latNSComboBox;
    QSpinBox        *m_latDegSpinBox;
    QSpinBox        *m_latMinSpinBox;
    QSpinBox        *m_latSecSpinBox;
    QLabel          *m_spacer2Label;
    QLabel          *m_monthLabel;
    QComboBox       *m_monthComboBox;
    QLabel          *m_yearLabel;
    QSpinBox        *m_yearSpinBox;

    AppPage         *m_page2;
    QFrame          *m_gridFrame2;
    QGridLayout     *m_gridLayout2;
    QCheckBox       *m_showCalendar;
    QCheckBox       *m_showChart;
    QCheckBox       *m_showSeasons;
    QCheckBox       *m_sunRise;
    QCheckBox       *m_sunSet;
    QCheckBox       *m_moonRise;
    QCheckBox       *m_moonSet;
    QCheckBox       *m_civilDawn;
    QCheckBox       *m_civilDusk;
    QCheckBox       *m_sunTimes;
    QCheckBox       *m_moonTimes;
    QCheckBox       *m_moonPhase;

    DateTime        *m_dt;
    GlobalPosition  *m_gp;
    PropertyDict    *m_pd;
};

//------------------------------------------------------------------------------
/*! \class GeoCatalogDialog sundialog.h
 *
 *  \brief BehavePlus geographic place browser dialog.
 *  Displays all the geographic catalog files in the folder
 *  and allows the user to select one.
 */

class GeoCatalogDialog : public AppDialog
{
// Enable signals and slots
    Q_OBJECT

/*! \enum ContextMenuOption
 *  \brief Enumerates the types of Document context menu options available.
 */

enum ContextMenuOption
{
    ContextSelect=0,
    ContextPrintVisibleView=1,
    ContextPrintEntireView=2
};

public:
    GeoCatalogDialog( QWidget *widget, const char *name="geoPlaceDialog" ) ;
    ~GeoCatalogDialog( void ) ;
    const QString &catalogSelection( void ) const;
    const QString &fileSelection( void ) const;

protected slots:
    void contextMenuActivated( int id ) ;
    void doubleClicked( QListViewItem *lvi ) ;
    void rightButtonClicked( QListViewItem *lvi, const QPoint &p, int column ) ;
    void store( void );

// Data members
private:
    QString         m_catalog;      //!< Current selection catalog name
    QString         m_file;         //!< Current selection file name
    QString         m_geoDir;       //!< Geographic data directory name
    QListView      *m_listView;     //!< Pointer to geographic file selection list
    QPopupMenu     *m_contextMenu;  //!< Pointer to listview context menu
    QListViewItem  *m_lvi;          //!< Pointer to context menu listview item
};

//------------------------------------------------------------------------------
/*! \class GeoPlaceDialog sundialog.h
 *
 *  \brief BehavePlus geographic place browser dialog.
 *  Displays all the named geographic places within a GeoPlaceCatalog
 *  and allows the user to select one.
 */

class GeoPlaceDialog : public AppDialog
{
// Enable signals and slots
    Q_OBJECT

/*! \enum ContextMenuOption
 *  \brief Enumerates the types of Document context menu options available.
 */

enum ContextMenuOption
{
    ContextSelect=0,
    ContextPrintVisibleView=1,
    ContextPrintEntireView=2
};

// Public methods
public:
    GeoPlaceDialog( const QString &placeName, const QString &fileName,
        QWidget *widget, const char *name="geoPlaceDialog" ) ;
    ~GeoPlaceDialog( void ) ;

// Protected slots
protected slots:
    void contextMenuActivated( int id ) ;
    void doubleClicked( QListViewItem *lvi ) ;
    void rightButtonClicked( QListViewItem *lvi, const QPoint &p, int column ) ;
    void store( void );

// Data members
public:
    QString         m_catalog;      //!< Current catalog place name
    QString         m_county;       //!< Current selection county/district/region
    QString         m_elev;         //!< Current selection elevation
    QString         m_gmt;          //!< Current selection GMT hour difference
    QString         m_lat;          //!< Current selection latitude
    QString         m_lon;          //!< Current selection longitude
    QString         m_place;        //!< Current select place name
    QString         m_state;        //!< Current selection state/provence/country
    QString         m_type;         //!< Current select geographic feature type
    QListView      *m_listView;     //!< Pointer to the selection list
    QPopupMenu     *m_contextMenu;  //!< Pointer to listview context menu
    QListViewItem  *m_lvi;          //!< Pointer to context menu listview item
};

#endif

//------------------------------------------------------------------------------
//  End of sundialog.h
//------------------------------------------------------------------------------

