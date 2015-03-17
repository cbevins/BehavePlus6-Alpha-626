//------------------------------------------------------------------------------
/*! \file slopetooldialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2010 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus slope from map measurements dialog class declarations.
 */

#ifndef _SLOPETOOLDIALOG_H_
/*! \def _SLOPETOOLDIALOG_H_
 *  \brief Prevent redundant includes.
 */
#define _SLOPETOOLDIALOG_H_ 1

// Forward class references
#include "appdialog.h"

class QFrame;
class QGridLayout;
class QLineEdit;
class QComboBox;
class QLabel;
class QListView;
class QListViewItem;
class QPushButton;
class QStringList;

//------------------------------------------------------------------------------
/*! \class SlopeToolDialog slopetooldialog.h
 *
 *  \brief A tabbed dialog to determine slope from map measurements.
 */

class SlopeToolDialog : public AppDialog
{
// Enable signals and slots
    Q_OBJECT

// Public methods
public:
    SlopeToolDialog( QWidget *parent,
        const char *name,
        const QString& program,
		const QString& version ) ;
    ~SlopeToolDialog( void ) ;

// Protected methods
	void update( void ) ;

// Protected slots
protected slots:
    void calculateButtonClicked( void );
    void clear( void ) ;            // Reimplemented virtual callback for Export button
	void clearOutputs( void ) ;
	void mrfDoubleClicked( QListViewItem *item );
	void unitsActivated( int id );

// Public data members
public:
    QString      m_program;
    QString      m_version;
    QWidget     *m_parent;			//!< Pointer to the parent QWidget
    QFrame      *m_gridFrame;		//!< Frame to hold the QGridLayout.
    QGridLayout *m_gridLayout;		//!< Pointer to the grid layout widget
	QListView   *m_listView;
	QLabel		*m_mrfComboLabel;
	QLabel		*m_mrfLabel;
	QLineEdit	*m_mrfEntry;
	QLabel		*m_unitsComboLabel;
	QComboBox	*m_unitsComboBox;
	QLabel		*m_contourIntervalLabel;
	QLineEdit	*m_contourIntervalEntry;
	QLabel		*m_contourIntervalUnits;
	QLabel		*m_mapDistanceLabel;
	QLineEdit	*m_mapDistanceEntry;
	QLabel		*m_mapDistanceUnits;
	QLabel		*m_numberContoursLabel;
	QLineEdit	*m_numberContoursEntry;
	QLabel		*m_slopePercentLabel;
	QLineEdit	*m_slopePercentEntry;
	QLabel		*m_slopePercentUnits;
	QLabel		*m_slopeDegreesLabel;
	QLineEdit	*m_slopeDegreesEntry;
	QLabel		*m_slopeDegreesUnits;
	QLabel		*m_slopeElevationLabel;
	QLineEdit	*m_slopeElevationEntry;
	QLabel		*m_slopeElevationUnits;
	QLabel		*m_slopeHorizontalLabel;
	QLineEdit	*m_slopeHorizontalEntry;
	QLabel		*m_slopeHorizontalUnits;
	QLabel		*m_descriptionLabel;
	QLineEdit	*m_descriptionEntry;
	QPushButton *m_calculateButton;
	QStringList  m_mrf;
};


#endif

//------------------------------------------------------------------------------
//  End of slopetooldialog.h
//------------------------------------------------------------------------------

