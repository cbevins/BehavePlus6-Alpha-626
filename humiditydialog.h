//------------------------------------------------------------------------------
/*! \file humiditydialog.h
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2013 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus relative humidity dialog class declarations.
 */

#ifndef _HUMIDITYDIALOG_H_
/*! \def _HUMIDITYDIALOG_H_
 *  \brief Prevent redundant includes.
 */
#define _HUMIDITYDIALOG_H_ 1

// Forward class references
#include "appdialog.h"
class QButtonGroup;
class QEvent;
class QFrame;
class QGridLayout;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QPopupMenu;
class QPushButton;
class HumidityPage;

//------------------------------------------------------------------------------
/*! \class HumidityDialog humiditydialog.h
 *
 *  \brief A tabbed dialog for various methods of estimating relative humidity.
 */

class HumidityDialog : public AppTabDialog
{
// Enable signals and slots
    Q_OBJECT

// Public methods
public:
    HumidityDialog( QWidget *parent,
        const QString &captionKey="HumidityDialog:Caption",
        const char *name="humidityDialog",
        const QString &acceptKey="HumidityDialog:Button:Dismiss",
        const QString &rejectKey="" ) ;
    ~HumidityDialog( void ) ;

    virtual HumidityPage *addPage( int method, const QString &tabKey, int rows,
        const QString &pictureFile, const QString &pictureName,
        const QString &htmlFile ) ;

// Protected methods
protected:
    virtual bool event( QEvent *e ) ;

// Private data members
public:
    QWidget                *m_parent;  //!< Pointer to the parent QWidget
    QPtrList<HumidityPage> *m_pageList;//!< List of pointers to child HumidityPages
};

//------------------------------------------------------------------------------
/*! \class HumidityPage humiditydialog.h
 *
 *  \brief Defines a single tabbed page for the HumidityDialog.
 */

class HumidityPage : public AppPage
{
// Enable signals and slots
    Q_OBJECT
// Public methods
public:
    HumidityPage( int method, HumidityDialog *dialog, int rows,
        const QString &pictureFile, const QString &pictureName,
        const QString &htmlFile, const char *name=0 ) ;
    ~HumidityPage( void ) ;

// Protected methods
protected:
    void addInput( const QString &nameKey, const QString &units,
        int minVal, int maxVal, int initVal, int lineStep, int pageStep ) ;
    void addOutput( int id, const QString &nameKey, const QString &units ) ;
	void mark( int nOutputs ) ;
	void unmark( int nOutputs );
    bool update1( int whom );
    bool update2( int whom );
    bool update3( int whom );
    void updateRh1( double db, double wb, double se, bool metricResults=false ) ;
    void updateRh2( double db, double wd, double se, bool metricResults=false ) ;
    void updateRh3( double db, double dp ) ;

// Protected slots
protected slots:
    bool calculate1( void );
    bool calculate2( void );
    bool calculate3( void );
    bool dbChanged1( void );
    bool dbChanged2( void );
    bool dbChanged3( void );
    bool dpChanged3( void );
    bool seChanged1( void );
    bool seChanged2( void );
    bool wbChanged1( void );
    bool wdChanged2( void );
    bool useEnglish( void );
    bool useMetric( void );

// Public data members
public:
    HumidityDialog   *m_dialog;     //!< Pointer to parent PropertyDialog
    QFrame           *m_gridFrame;  //!< Frame to hold the QGridLayout.
    QGridLayout      *m_gridLayout; //!< Pointer to the grid layout widget
    // Units selection widgets
    QButtonGroup*     m_group;
    QRadioButton*     m_radio[2];   //!< Pointer to units radio buttons
    // Input and output widgets
    QLabel*           m_name[6];    //!< Field name labels.
    QLabel*           m_units[6];   //!< Field units labels.
    QSpinBox*         m_spin[3];    //!< Input spin boxes.
    QLineEdit*        m_output[3];  //!< Output result fields.
	QPushButton*      m_calculateButton1;
	QPushButton*      m_calculateButton2;
	QPushButton*      m_calculateButton3;
    int               m_method;     //!< Methods 1-3
    int               m_rows;       //!< Internal counter for grid rows
	bool			  m_english;	//!< TRUE if current units are English
};

#endif

//------------------------------------------------------------------------------
//  End of humiditydialog.h
//------------------------------------------------------------------------------
