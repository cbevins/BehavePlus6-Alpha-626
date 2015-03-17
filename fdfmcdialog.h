//------------------------------------------------------------------------------
/*! \file fdfmcdialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief BehavePlus fine dead fuel moisture correction tool dialog class
 *  declarations.
 */

#ifndef _FDFMCDIALOG_H_
/*! \def _FDFMCDIALOG_H_
 *  \brief Prevent redundant includes.
 */
#define _FDFMCDIALOG_H_ 1

// Forward class references
#include "appdialog.h"
class QComboBox;
class QFrame;
class QGridLayout;
class QLabel;
class QLineEdit;

//------------------------------------------------------------------------------
/*! \class FdfmcDialog fdfmcdialog.h
 *
 *  \brief BehavePlus fine dead fuel moisture correction dialog.
 */

static const int Rows = 15;

class FdfmcDialog : public AppDialog
{
// Enable signals and slots
    Q_OBJECT

// Public methods
public:
    FdfmcDialog( QWidget *parent, const char *name="fdfmcDialog",
        const QString& program="BehavePlus", const QString& version="3.0.0" ) ;
    ~FdfmcDialog( void ) ;

// Protected slots
protected slots:
    void clear( void ) ;            // Reimplemented virtual callback for Export button
    void descriptionChanged( const QString &descritpion ) ;
    void update( int ) ;

// Public data members
private:
    QFrame      *m_gridFrame;       //!< Pointer to grid frame
    QGridLayout *m_gridLayout;      //!< Pointer to the layout widget
    QLabel      *m_lbl[Rows];       //!< Pointer to array of labels
    QComboBox   *m_dbComboBox;      //!< Pointer to the dry bulb temperature combo box
    QComboBox   *m_rhComboBox;      //!< Pointer to the relative humidity combo box
    QComboBox   *m_monComboBox;     //!< Pointer to the month combo box
    QComboBox   *m_todComboBox;     //!< Pointer to the time of day combo box
    QComboBox   *m_elevComboBox;    //!< Pointer to the elevational difference combo box
    QComboBox   *m_slpComboBox;     //!< Pointer to the slope combo box
    QComboBox   *m_aspComboBox;     //!< Pointer to the aspect combo box
    QComboBox   *m_shdComboBox;     //!< Pointer to the fuel shading from sun combo box
    QLineEdit   *m_corLineEdit;     //!< Pointer to correction fuel moisture result
    QLineEdit   *m_refLineEdit;     //!< Pointer to reference fuel moisture result
    QLineEdit   *m_resLineEdit;     //!< Pointer to corrected fuel moisture result
    QLineEdit   *m_descriptEdit;    //!< Pointer to description entry field.
    QString      m_program;
    QString      m_version;
    int          m_asp;
    int          m_cor;
    int          m_db;
    int          m_elev;
    int          m_mon;
    int          m_ref;
    int          m_res;
    int          m_rh;
    int          m_slp;
    int          m_shd;
    int          m_tod;
};

#endif

//------------------------------------------------------------------------------
//  End of fdfmcdialog.h
//------------------------------------------------------------------------------

